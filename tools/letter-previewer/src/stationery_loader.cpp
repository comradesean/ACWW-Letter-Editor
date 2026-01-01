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

    // Stationery data: name, item ID, text color, recipient color
    struct StationeryInfo {
        const char* name;
        uint16_t itemId;
        uint8_t textR, textG, textB;
        uint8_t recipientR, recipientG, recipientB;
    };

    // Default brown text (121, 97, 32), red recipient (192, 64, 64)
    static const StationeryInfo s_stationery[64] = {
        {"Butterfly paper",   0x1000, 121, 97, 32, 192, 64, 64},  // 0x00
        {"Airmail paper",     0x1004, 121, 97, 32, 192, 64, 64},  // 0x01
        {"New Year's cards",  0x1008, 121, 97, 32, 192, 64, 64},  // 0x02
        {"Lacy paper",        0x100C, 121, 97, 32, 192, 64, 64},  // 0x03
        {"Cloudy paper",      0x1010, 121, 97, 32, 192, 64, 64},  // 0x04
        {"Petal paper",       0x1014, 121, 97, 32, 192, 64, 64},  // 0x05
        {"Snowy paper",       0x1018, 121, 97, 32, 192, 64, 64},  // 0x06
        {"Maple-leaf paper",  0x101C, 121, 97, 32, 192, 64, 64},  // 0x07
        {"Lined paper",       0x1020, 121, 97, 32, 192, 64, 64},  // 0x08
        {"Notebook paper",    0x1024, 121, 97, 32, 192, 64, 64},  // 0x09
        {"Flowery paper",     0x1028, 121, 97, 32, 192, 64, 64},  // 0x0A
        {"Polka-dot paper",   0x102C, 121, 97, 32, 192, 64, 64},  // 0x0B
        {"Bottle paper",      0x1030, 121, 97, 32, 121, 97, 32},  // 0x0C - same color for recipient
        {"Ribbon paper",      0x1034, 121, 97, 32, 192, 64, 64},  // 0x0D
        {"Sparkly paper",     0x1038, 121, 97, 32, 192, 64, 64},  // 0x0E
        {"Vine paper",        0x103C, 121, 97, 32, 192, 64, 64},  // 0x0F
        {"Formal paper",      0x1040, 121, 97, 32, 192, 64, 64},  // 0x10
        {"Snowman paper",     0x1044, 121, 97, 32, 192, 64, 64},  // 0x11
        {"Card paper",        0x1048, 121, 97, 32, 192, 64, 64},  // 0x12
        {"Leopard paper",     0x104C, 121, 97, 32, 192, 64, 64},  // 0x13
        {"Cow paper",         0x1050, 121, 97, 32, 192, 64, 64},  // 0x14
        {"Camouflage paper",  0x1054, 121, 97, 32, 192, 64, 64},  // 0x15
        {"Hamburger paper",   0x1058, 121, 97, 32, 192, 64, 64},  // 0x16
        {"Piano paper",       0x105C, 121, 97, 32, 192, 64, 64},  // 0x17
        {"Nook paper",        0x1060, 121, 97, 32, 192, 64, 64},  // 0x18
        {"Fox paper",         0x1064, 121, 97, 32, 192, 64, 64},  // 0x19
        {"Birthday cards",    0x1068, 121, 97, 32, 192, 64, 64},  // 0x1A
        {"Four-leaf paper",   0x106C, 121, 97, 32, 192, 64, 64},  // 0x1B
        {"Town-hall paper",   0x1070, 121, 97, 32, 192, 64, 64},  // 0x1C
        {"Tortimer paper",    0x1074,  65, 113, 89,   0, 121, 24},  // 0x1D - #417159 text, #007918 recipient
        {"Insurance paper",   0x1078, 121, 97, 32, 192, 64, 64},  // 0x1E
        {"Academy paper",     0x107C, 121, 97, 32, 192, 64, 64},  // 0x1F
        {"Lovely paper",      0x1080, 195,   0, 154, 219,   0, 113},  // 0x20 - #C3009A text, #DB0071 recipient
        {"Rainbow paper",     0x1084, 121, 97, 32, 192, 64, 64},  // 0x21
        {"Egyptian paper",    0x1088, 121, 97, 32, 192, 64, 64},  // 0x22
        {"Lotus paper",       0x108C, 121, 97, 32, 192, 64, 64},  // 0x23
        {"Tile paper",        0x1090, 121, 97, 32, 192, 64, 64},  // 0x24
        {"Mosaic paper",      0x1094, 121, 97, 32, 192, 64, 64},  // 0x25
        {"Elegant paper",     0x1098, 121, 97, 32, 192, 64, 64},  // 0x26
        {"Town view paper",   0x109C, 121, 97, 32, 192, 64, 64},  // 0x27
        {"Chinese paper",     0x10A0, 121, 97, 32, 192, 64, 64},  // 0x28
        {"Ocean paper",       0x10A4, 121, 97, 32, 192, 64, 64},  // 0x29
        {"Industrial paper",  0x10A8, 121, 97, 32, 192, 64, 64},  // 0x2A
        {"Fireworks paper",   0x10AC, 121, 97, 32, 192, 64, 64},  // 0x2B
        {"Floral paper",      0x10B0, 121, 97, 32, 192, 64, 64},  // 0x2C
        {"Mushroom paper",    0x10B4, 121, 97, 32, 192, 64, 64},  // 0x2D
        {"Star paper",        0x10B8, 121, 97, 32, 192, 64, 64},  // 0x2E
        {"Composer paper",    0x10BC, 121, 97, 32, 192, 64, 64},  // 0x2F
        {"Bathtub paper",     0x10C0, 121, 97, 32, 192, 64, 64},  // 0x30
        {"SMB3 paper",        0x10C4, 121, 97, 32, 192, 64, 64},  // 0x31
        {"Cool paper",        0x10C8, 121, 97, 32, 192, 64, 64},  // 0x32
        {"Forest paper",      0x10CC, 121, 97, 32, 192, 64, 64},  // 0x33
        {"Bubble paper",      0x10D0, 121, 97, 32, 192, 64, 64},  // 0x34
        {"Buttercup paper",   0x10D4, 121, 97, 32, 192, 64, 64},  // 0x35
        {"Tartan paper",      0x10D8, 121, 97, 32, 192, 64, 64},  // 0x36
        {"Plaid paper",       0x10DC, 121, 97, 32, 192, 64, 64},  // 0x37
        {"Lemon-lime paper",  0x10E0, 121, 97, 32, 192, 64, 64},  // 0x38
        {"Crater paper",      0x10E4, 121, 97, 32, 192, 64, 64},  // 0x39
        {"Bejeweled paper",   0x10E8, 121, 97, 32, 192, 64, 64},  // 0x3A
        {"Geometric paper",   0x10EC, 121, 97, 32, 192, 64, 64},  // 0x3B
        {"Southwest paper",   0x10F0, 121, 97, 32, 192, 64, 64},  // 0x3C
        {"Night sky paper",   0x10F4, 121, 97, 32, 192, 64, 64},  // 0x3D
        {"Chic paper",        0x10F8, 121, 97, 32, 192, 64, 64},  // 0x3E
        {"Goldfish paper",    0x10FC, 121, 97, 32, 192, 64, 64},  // 0x3F
    };
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

QString StationeryLoader::getName(int index) const {
    if (index < 0 || index >= NUM_PAPERS) {
        return QString("Unknown");
    }
    return QString::fromUtf8(s_stationery[index].name);
}

uint16_t StationeryLoader::getItemId(int index) const {
    if (index < 0 || index >= NUM_PAPERS) {
        return 0;
    }
    return s_stationery[index].itemId;
}

QColor StationeryLoader::getTextColor(int index) const {
    if (index < 0 || index >= NUM_PAPERS) {
        return QColor(121, 97, 32);  // Default brown
    }
    const auto& s = s_stationery[index];
    return QColor(s.textR, s.textG, s.textB);
}

QColor StationeryLoader::getRecipientColor(int index) const {
    if (index < 0 || index >= NUM_PAPERS) {
        return QColor(192, 64, 64);  // Default red
    }
    const auto& s = s_stationery[index];
    return QColor(s.recipientR, s.recipientG, s.recipientB);
}
