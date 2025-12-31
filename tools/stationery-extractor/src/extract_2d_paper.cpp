#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <algorithm>
#include "nds_rom.h"
#include "lz77.h"

struct Color { uint8_t r, g, b, a; };

Color decodeRGB555(uint16_t color) {
    Color c;
    c.r = ((color >> 0) & 0x1F) * 255 / 31;
    c.g = ((color >> 5) & 0x1F) * 255 / 31;
    c.b = ((color >> 10) & 0x1F) * 255 / 31;
    c.a = 255;
    return c;
}

void writePNGChunk(std::ofstream& file, const char* type, const std::vector<uint8_t>& data) {
    static uint32_t crcTable[256];
    static bool crcTableComputed = false;
    if (!crcTableComputed) {
        for (uint32_t n = 0; n < 256; n++) {
            uint32_t c = n;
            for (int k = 0; k < 8; k++) c = (c & 1) ? (0xedb88320L ^ (c >> 1)) : (c >> 1);
            crcTable[n] = c;
        }
        crcTableComputed = true;
    }
    auto crc32 = [](const uint8_t* buf, size_t len, uint32_t crc = 0xffffffff) {
        for (size_t n = 0; n < len; n++) crc = crcTable[(crc ^ buf[n]) & 0xff] ^ (crc >> 8);
        return crc ^ 0xffffffff;
    };
    uint32_t length = data.size();
    uint8_t lenBytes[4] = {(uint8_t)(length >> 24), (uint8_t)(length >> 16), (uint8_t)(length >> 8), (uint8_t)(length)};
    file.write(reinterpret_cast<char*>(lenBytes), 4);
    file.write(type, 4);
    if (!data.empty()) file.write(reinterpret_cast<const char*>(data.data()), data.size());
    std::vector<uint8_t> crcData(4 + data.size());
    memcpy(crcData.data(), type, 4);
    if (!data.empty()) memcpy(crcData.data() + 4, data.data(), data.size());
    uint32_t crc = crc32(crcData.data(), crcData.size());
    uint8_t crcBytes[4] = {(uint8_t)(crc >> 24), (uint8_t)(crc >> 16), (uint8_t)(crc >> 8), (uint8_t)(crc)};
    file.write(reinterpret_cast<char*>(crcBytes), 4);
}

void writePNG(const char* filename, const Color* pixels, int width, int height) {
    std::ofstream file(filename, std::ios::binary);
    const uint8_t signature[] = {137, 80, 78, 71, 13, 10, 26, 10};
    file.write(reinterpret_cast<const char*>(signature), 8);
    std::vector<uint8_t> ihdr(13);
    ihdr[0] = (width >> 24) & 0xff; ihdr[1] = (width >> 16) & 0xff; ihdr[2] = (width >> 8) & 0xff; ihdr[3] = width & 0xff;
    ihdr[4] = (height >> 24) & 0xff; ihdr[5] = (height >> 16) & 0xff; ihdr[6] = (height >> 8) & 0xff; ihdr[7] = height & 0xff;
    ihdr[8] = 8; ihdr[9] = 6; ihdr[10] = 0; ihdr[11] = 0; ihdr[12] = 0;
    writePNGChunk(file, "IHDR", ihdr);
    std::vector<uint8_t> rawData;
    for (int y = 0; y < height; y++) {
        rawData.push_back(0);
        for (int x = 0; x < width; x++) {
            const Color& c = pixels[y * width + x];
            rawData.push_back(c.r); rawData.push_back(c.g); rawData.push_back(c.b); rawData.push_back(c.a);
        }
    }
    std::vector<uint8_t> compressed;
    compressed.push_back(0x78); compressed.push_back(0x01);
    size_t remaining = rawData.size(), offset = 0;
    while (remaining > 0) {
        size_t blockSize = std::min(remaining, (size_t)65535);
        bool lastBlock = (remaining <= 65535);
        compressed.push_back(lastBlock ? 0x01 : 0x00);
        compressed.push_back(blockSize & 0xff); compressed.push_back((blockSize >> 8) & 0xff);
        compressed.push_back(~blockSize & 0xff); compressed.push_back((~blockSize >> 8) & 0xff);
        for (size_t i = 0; i < blockSize; i++) compressed.push_back(rawData[offset + i]);
        offset += blockSize; remaining -= blockSize;
    }
    uint32_t a = 1, b = 0;
    for (size_t i = 0; i < rawData.size(); i++) { a = (a + rawData[i]) % 65521; b = (b + a) % 65521; }
    uint32_t adler = (b << 16) | a;
    compressed.push_back((adler >> 24) & 0xff); compressed.push_back((adler >> 16) & 0xff);
    compressed.push_back((adler >> 8) & 0xff); compressed.push_back(adler & 0xff);
    writePNGChunk(file, "IDAT", compressed);
    writePNGChunk(file, "IEND", std::vector<uint8_t>());
}

void extractPaper(NDSRom& rom, int paperNum, const char* outDir) {
    char numStr[8];
    snprintf(numStr, sizeof(numStr), "%03d", paperNum);

    std::string chrPath = std::string("/menu/paper/chr/") + numStr + ".bch";
    std::string pltPath = std::string("/menu/paper/plt/") + numStr + ".bpl";
    std::string scrPath = std::string("/menu/paper/scr/") + numStr + ".bsc";

    auto chrData = rom.readFile(chrPath);
    auto pltData = rom.readFile(pltPath);
    auto scrData = rom.readFile(scrPath);

    if (chrData.empty() || pltData.empty() || scrData.empty()) {
        printf("Paper %03d: Failed to load files\n", paperNum);
        return;
    }

    if (LZ77::isCompressed(chrData)) chrData = LZ77::decompress(chrData);
    if (LZ77::isCompressed(pltData)) pltData = LZ77::decompress(pltData);
    if (LZ77::isCompressed(scrData)) scrData = LZ77::decompress(scrData);

    // BPL format: 96 bytes = 3 sub-palettes of 32 bytes (16 colors each)
    // Screen palette 13 → sub-palette 1 (offset 32)
    // Screen palette 14 → sub-palette 2 (offset 64)
    // Screen palette 0 → transparent
    Color palettes[16][16];
    memset(palettes, 0, sizeof(palettes));

    // Load palette 13 from sub-palette 1 (offset 32)
    for (int i = 0; i < 16 && 32 + i*2 + 1 < (int)pltData.size(); i++) {
        uint16_t col = pltData[32 + i*2] | (pltData[32 + i*2 + 1] << 8);
        palettes[13][i] = decodeRGB555(col);
    }
    palettes[13][0].a = 0;

    // Load palette 14 from sub-palette 2 (offset 64)
    for (int i = 0; i < 16 && 64 + i*2 + 1 < (int)pltData.size(); i++) {
        uint16_t col = pltData[64 + i*2] | (pltData[64 + i*2 + 1] << 8);
        palettes[14][i] = decodeRGB555(col);
    }
    palettes[14][0].a = 0;

    // Load palette 15 from sub-palette 0 (offset 0)
    for (int i = 0; i < 16 && i*2 + 1 < (int)pltData.size(); i++) {
        uint16_t col = pltData[i*2] | (pltData[i*2 + 1] << 8);
        palettes[15][i] = decodeRGB555(col);
    }
    palettes[15][0].a = 0;

    // Palette 0 is fully transparent
    for (int i = 0; i < 16; i++) {
        palettes[0][i] = {0, 0, 0, 0};
    }

    // CHR data: no header, tile 0 in CHR = screen tile 16
    // Screen tiles are numbered 16+, so subtract 16 to get CHR index
    size_t chrOffset = 0;
    int tileBase = 16;  // Screen tile numbers start at 16

    // DS tiles are 8x8 pixels, 4bpp = 32 bytes per tile
    int bytesPerTile = 32;

    // Screen map: 32x24 tiles for 256x192 display
    // Each entry is 2 bytes: tile number (10 bits) + flip bits + palette
    int mapWidth = 32;
    int mapHeight = 24;  // DS bottom screen is 256x192

    int imgWidth = mapWidth * 8;   // 256
    int imgHeight = mapHeight * 8; // 192
    std::vector<Color> pixels(imgWidth * imgHeight, {0, 0, 0, 255});

    for (int ty = 0; ty < mapHeight; ty++) {
        for (int tx = 0; tx < mapWidth; tx++) {
            int mapIdx = ty * mapWidth + tx;
            if (mapIdx * 2 + 1 >= (int)scrData.size()) continue;

            uint16_t tileEntry = scrData[mapIdx * 2] | (scrData[mapIdx * 2 + 1] << 8);
            int screenTileNum = tileEntry & 0x3FF;
            bool hFlip = (tileEntry >> 10) & 1;
            bool vFlip = (tileEntry >> 11) & 1;
            int palIdx = (tileEntry >> 12) & 0xF;

            // Convert screen tile number to CHR tile index
            int chrTileNum = screenTileNum - tileBase;
            if (chrTileNum < 0) continue;  // Skip invalid tiles

            size_t tileDataOffset = chrOffset + chrTileNum * bytesPerTile;
            if (tileDataOffset + bytesPerTile > chrData.size()) continue;

            // Render 8x8 tile
            for (int py = 0; py < 8; py++) {
                for (int px = 0; px < 8; px++) {
                    int srcX = hFlip ? (7 - px) : px;
                    int srcY = vFlip ? (7 - py) : py;

                    int pixelIdx = srcY * 8 + srcX;
                    int byteIdx = pixelIdx / 2;
                    uint8_t byte = chrData[tileDataOffset + byteIdx];
                    uint8_t colorIdx = (pixelIdx & 1) ? ((byte >> 4) & 0xF) : (byte & 0xF);

                    int imgX = tx * 8 + px;
                    int imgY = ty * 8 + py;

                    if (colorIdx < 16 && palIdx < 16) {
                        pixels[imgY * imgWidth + imgX] = palettes[palIdx][colorIdx];
                    }
                }
            }
        }
    }

    char outFile[256];
    snprintf(outFile, sizeof(outFile), "%s/paper_%03d.png", outDir, paperNum);
    writePNG(outFile, pixels.data(), imgWidth, imgHeight);
    printf("Extracted paper %03d\n", paperNum);
}

int main(int argc, char* argv[]) {
    const char* outDir = "extracted_2d_paper";
    if (argc > 1) outDir = argv[1];

    NDSRom rom;
    rom.load("../../references/Welcome to Animal Crossing - Wild World (USA).nds");

    printf("Extracting all 64 stationery papers to %s/\n", outDir);

    for (int i = 0; i < 64; i++) {
        extractPaper(rom, i, outDir);
    }

    printf("\nDone! Extracted 64 papers to %s/\n", outDir);
    return 0;
}
