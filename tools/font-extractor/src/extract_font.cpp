#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <cstdint>
#include <map>
#include "nds_rom.h"
#include "lz77.h"

struct Color { uint8_t r, g, b, a; };

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

struct FontInfo {
    std::string name;
    int numChars;
    int glyphWidth;
    int glyphHeight;
    std::vector<uint16_t> charCodes;
    std::vector<uint8_t> charWidths;
    std::vector<uint8_t> imgData;
};

bool loadFont(NDSRom& rom, const std::string& fontName, FontInfo& font) {
    font.name = fontName;

    std::string basePath = "/font/" + fontName;
    auto headData = rom.readFile(basePath + "_head.bin");
    auto attrData = rom.readFile(basePath + "_attr.bin");
    auto imgData = rom.readFile(basePath + "_img.bin");

    if (headData.empty() || attrData.empty() || imgData.empty()) {
        printf("Failed to load font %s\n", fontName.c_str());
        return false;
    }

    if (LZ77::isCompressed(headData)) headData = LZ77::decompress(headData);
    if (LZ77::isCompressed(attrData)) attrData = LZ77::decompress(attrData);
    if (LZ77::isCompressed(imgData)) imgData = LZ77::decompress(imgData);

    // Parse header
    font.numChars = headData[0] | (headData[1] << 8);
    font.glyphWidth = headData[4] | (headData[5] << 8);
    font.glyphHeight = headData[6] | (headData[7] << 8);

    // Parse attributes (4 bytes per char: char code + width)
    for (int i = 0; i < font.numChars && i * 4 + 3 < (int)attrData.size(); i++) {
        uint16_t charCode = attrData[i * 4] | (attrData[i * 4 + 1] << 8);
        uint16_t width = attrData[i * 4 + 2] | (attrData[i * 4 + 3] << 8);
        font.charCodes.push_back(charCode);
        font.charWidths.push_back(width);
    }

    font.imgData = imgData;

    printf("Font %s: %d chars, %dx%d glyphs, %zu bytes image data\n",
           fontName.c_str(), font.numChars, font.glyphWidth, font.glyphHeight, imgData.size());

    return true;
}

void extractFont(const FontInfo& font, const char* outDir) {
    int glyphW = font.glyphWidth;
    int glyphH = font.glyphHeight;
    int bytesPerGlyph = (glyphW * glyphH * 2) / 8;  // 2bpp

    // Arrange glyphs in a grid (16 per row)
    int glyphsPerRow = 16;
    int rows = (font.numChars + glyphsPerRow - 1) / glyphsPerRow;
    int sheetW = glyphsPerRow * glyphW;
    int sheetH = rows * glyphH;

    std::vector<Color> pixels(sheetW * sheetH, {0, 0, 0, 0});

    // 2bpp grayscale palette (white text on transparent)
    Color palette[4] = {
        {0, 0, 0, 0},       // 0 = transparent
        {85, 85, 85, 255},  // 1 = dark gray
        {170, 170, 170, 255}, // 2 = light gray
        {255, 255, 255, 255}  // 3 = white
    };

    for (int g = 0; g < font.numChars; g++) {
        int gridX = g % glyphsPerRow;
        int gridY = g / glyphsPerRow;
        int glyphDataOffset = g * bytesPerGlyph;

        if (glyphDataOffset + bytesPerGlyph > (int)font.imgData.size()) continue;

        // Decode 2bpp glyph
        for (int py = 0; py < glyphH; py++) {
            for (int px = 0; px < glyphW; px++) {
                int pixelIdx = py * glyphW + px;
                int byteIdx = pixelIdx / 4;
                int bitOffset = (pixelIdx % 4) * 2;

                uint8_t byte = font.imgData[glyphDataOffset + byteIdx];
                uint8_t colorIdx = (byte >> bitOffset) & 0x03;

                int imgX = gridX * glyphW + px;
                int imgY = gridY * glyphH + py;
                pixels[imgY * sheetW + imgX] = palette[colorIdx];
            }
        }
    }

    char outFile[256];
    snprintf(outFile, sizeof(outFile), "%s/%s.png", outDir, font.name.c_str());
    writePNG(outFile, pixels.data(), sheetW, sheetH);
    printf("Saved %s (%dx%d)\n", outFile, sheetW, sheetH);

    // Also save character map as text file
    char mapFile[256];
    snprintf(mapFile, sizeof(mapFile), "%s/%s_map.txt", outDir, font.name.c_str());
    std::ofstream map(mapFile);
    map << "# " << font.name << " character map\n";
    map << "# Format: index, char_code (hex), char_code (dec), width, character\n";
    for (int i = 0; i < (int)font.charCodes.size(); i++) {
        uint16_t code = font.charCodes[i];
        char ch = (code >= 0x20 && code < 0x7F) ? (char)code : '?';
        map << i << ", 0x" << std::hex << code << std::dec << ", " << code
            << ", " << (int)font.charWidths[i] << ", " << ch << "\n";
    }
    map.close();
    printf("Saved %s\n", mapFile);
}

int main(int argc, char* argv[]) {
    const char* outDir = "extracted_fonts";
    if (argc > 1) outDir = argv[1];

    NDSRom rom;
    rom.load("../../references/Welcome to Animal Crossing - Wild World (USA).nds");

    // Create output directory
    std::string mkdirCmd = std::string("mkdir -p ") + outDir;
    system(mkdirCmd.c_str());

    printf("Extracting fonts to %s/\n\n", outDir);

    // Extract all fonts
    const char* fontNames[] = {"fontA", "fontASub", "fontB", "fontC", "fontD"};

    for (const char* name : fontNames) {
        FontInfo font;
        if (loadFont(rom, name, font)) {
            extractFont(font, outDir);
            printf("\n");
        }
    }

    printf("Done!\n");
    return 0;
}
