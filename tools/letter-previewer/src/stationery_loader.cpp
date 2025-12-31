#include "stationery_loader.h"
#include "nds_rom.h"
#include "lz77.h"
#include <cstring>

namespace {
    struct Color { uint8_t r, g, b, a; };

    Color decodeRGB555(uint16_t color) {
        Color c;
        c.r = ((color >> 0) & 0x1F) * 255 / 31;
        c.g = ((color >> 5) & 0x1F) * 255 / 31;
        c.b = ((color >> 10) & 0x1F) * 255 / 31;
        c.a = 255;
        return c;
    }
}

bool StationeryLoader::load(NDSRom& rom) {
    m_papers.clear();
    m_papers.reserve(NUM_PAPERS);

    for (int i = 0; i < NUM_PAPERS; i++) {
        QImage paper = loadPaper(rom, i);
        if (paper.isNull()) {
            return false;
        }
        m_papers.push_back(paper);
    }

    return true;
}

const QImage& StationeryLoader::getPaper(int index) const {
    static QImage empty;
    if (index < 0 || index >= static_cast<int>(m_papers.size())) {
        return empty;
    }
    return m_papers[index];
}

QImage StationeryLoader::loadPaper(NDSRom& rom, int paperNum) {
    char numStr[8];
    snprintf(numStr, sizeof(numStr), "%03d", paperNum);

    std::string chrPath = std::string("/menu/paper/chr/") + numStr + ".bch";
    std::string pltPath = std::string("/menu/paper/plt/") + numStr + ".bpl";
    std::string scrPath = std::string("/menu/paper/scr/") + numStr + ".bsc";

    auto chrData = rom.readFile(chrPath);
    auto pltData = rom.readFile(pltPath);
    auto scrData = rom.readFile(scrPath);

    if (chrData.empty() || pltData.empty() || scrData.empty()) {
        return QImage();
    }

    if (LZ77::isCompressed(chrData)) chrData = LZ77::decompress(chrData);
    if (LZ77::isCompressed(pltData)) pltData = LZ77::decompress(pltData);
    if (LZ77::isCompressed(scrData)) scrData = LZ77::decompress(scrData);

    // BPL format: 96 bytes = 3 sub-palettes of 32 bytes (16 colors each)
    Color palettes[16][16];
    memset(palettes, 0, sizeof(palettes));

    // Load palette 13 from sub-palette 1 (offset 32)
    for (int i = 0; i < 16 && 32 + i*2 + 1 < static_cast<int>(pltData.size()); i++) {
        uint16_t col = pltData[32 + i*2] | (pltData[32 + i*2 + 1] << 8);
        palettes[13][i] = decodeRGB555(col);
    }
    palettes[13][0].a = 0;

    // Load palette 14 from sub-palette 2 (offset 64)
    for (int i = 0; i < 16 && 64 + i*2 + 1 < static_cast<int>(pltData.size()); i++) {
        uint16_t col = pltData[64 + i*2] | (pltData[64 + i*2 + 1] << 8);
        palettes[14][i] = decodeRGB555(col);
    }
    palettes[14][0].a = 0;

    // Load palette 15 from sub-palette 0 (offset 0)
    for (int i = 0; i < 16 && i*2 + 1 < static_cast<int>(pltData.size()); i++) {
        uint16_t col = pltData[i*2] | (pltData[i*2 + 1] << 8);
        palettes[15][i] = decodeRGB555(col);
    }
    palettes[15][0].a = 0;

    // Palette 0 is fully transparent
    for (int i = 0; i < 16; i++) {
        palettes[0][i] = {0, 0, 0, 0};
    }

    // Screen dimensions
    constexpr int mapWidth = 32;
    constexpr int mapHeight = 24;
    constexpr int bytesPerTile = 32;
    constexpr int tileBase = 16;

    QImage image(PAPER_WIDTH, PAPER_HEIGHT, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    for (int ty = 0; ty < mapHeight; ty++) {
        for (int tx = 0; tx < mapWidth; tx++) {
            int mapIdx = ty * mapWidth + tx;
            if (mapIdx * 2 + 1 >= static_cast<int>(scrData.size())) continue;

            uint16_t tileEntry = scrData[mapIdx * 2] | (scrData[mapIdx * 2 + 1] << 8);
            int screenTileNum = tileEntry & 0x3FF;
            bool hFlip = (tileEntry >> 10) & 1;
            bool vFlip = (tileEntry >> 11) & 1;
            int palIdx = (tileEntry >> 12) & 0xF;

            int chrTileNum = screenTileNum - tileBase;
            if (chrTileNum < 0) continue;

            size_t tileDataOffset = chrTileNum * bytesPerTile;
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
                        const Color& c = palettes[palIdx][colorIdx];
                        image.setPixelColor(imgX, imgY, QColor(c.r, c.g, c.b, c.a));
                    }
                }
            }
        }
    }

    return image;
}
