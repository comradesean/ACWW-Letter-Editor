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
        {"Butterfly paper",   0x1000,  48, 121, 130, 211,   0, 227},  // 0x00 - #307982 text, #D300E3 recipient
        {"Airmail paper",     0x1004,  73,  97, 121,  40,  81, 243},  // 0x01 - #496179 text, #2851F3 recipient
        {"New Year's cards",  0x1008,  73,  81,  48,   0, 146, 146},  // 0x02 - #495130 text, #009292 recipient
        {"Lacy paper",        0x100C,  81,  81,  81, 195,   8, 178},  // 0x03 - #515151 text, #C308B2 recipient
        {"Cloudy paper",      0x1010,  97,  97,  97,  40,  65, 251},  // 0x04 - #616161 text, #2841FB recipient
        {"Petal paper",       0x1014,  97,  97, 105, 251,   8, 162},  // 0x05 - #616169 text, #FB08A2 recipient
        {"Snowy paper",       0x1018,  40, 105, 113,   0, 138, 146},  // 0x06 - #286971 text, #008A92 recipient
        {"Maple-leaf paper",  0x101C, 251, 211, 162, 251, 178,  73},  // 0x07 - #FBD3A2 text, #FBB249 recipient
        {"Lined paper",       0x1020,  81,  81,  81,  81,  81,  81},  // 0x08 - #515151 text, #515151 recipient
        {"Notebook paper",    0x1024, 130, 113,  81, 162, 113,  16},  // 0x09 - #827151 text, #A27110 recipient
        {"Flowery paper",     0x1028,  65, 146, 130, 219,  89,  97},  // 0x0A - #419282 text, #DB5961 recipient
        {"Polka-dot paper",   0x102C,  81, 105,  97, 251,  56, 178},  // 0x0B - #516961 text, #FB38B2 recipient
        {"Bottle paper",      0x1030, 121, 97, 32, 121, 97, 32},  // 0x0C - same color for recipient
        {"Ribbon paper",      0x1034, 121,  65,  48, 121,  65,  48},  // 0x0D - #794130 text, #794130 recipient
        {"Sparkly paper",     0x1038,   0,  89, 105,  24, 113, 186},  // 0x0E - #005969 text, #1871BA recipient
        {"Vine paper",        0x103C,   8, 121,  81,   0, 162, 113},  // 0x0F - #087951 text, #00A271 recipient
        {"Formal paper",      0x1040, 130,  16,  32, 130,  16,  32},  // 0x10 - #821020 text, #821020 recipient
        {"Snowman paper",     0x1044, 121, 97, 32, 192, 64, 64},  // 0x11
        {"Card paper",        0x1048,  89,  97, 105,  89,  89,  89},  // 0x12 - #596169 text, #595959 recipient
        {"Leopard paper",     0x104C,  48,  48,  40, 113,  65,   0},  // 0x13 - #303028 text, #714100 recipient
        {"Cow paper",         0x1050,  89,  89,  65, 243, 105, 105},  // 0x14 - #595941 text, #F36969 recipient
        {"Camouflage paper",  0x1054, 243, 235, 178, 251, 251, 251},  // 0x15 - #F3EBB2 text, #FBFBFB recipient
        {"Hamburger paper",   0x1058, 251,  48,   0, 251, 235,  16},  // 0x16 - #FB3000 text, #FBEB10 recipient
        {"Piano paper",       0x105C, 211, 121, 186, 211, 121, 186},  // 0x17 - #D379BA text, #D379BA recipient
        {"Nook paper",        0x1060, 138,  97,  56, 154,  97,  40},  // 0x18 - #8A6138 text, #9A6128 recipient
        {"Fox paper",         0x1064,  32, 138, 121,  24, 105,  97},  // 0x19 - #208A79 text, #186961 recipient
        {"Birthday cards",    0x1068, 105,  89,  97, 113,  97,  73},  // 0x1A - #695961 text, #716149 recipient
        {"Four-leaf paper",   0x106C,  65,  81,   0,  73, 105,   0},  // 0x1B - #415100 text, #496900 recipient
        {"Town-hall paper",   0x1070,  65, 113,  89,   0, 121,  24},  // 0x1C - #417159 text, #007918 recipient
        {"Tortimer paper",    0x1074,  65, 113, 89,   0, 121, 24},  // 0x1D - #417159 text, #007918 recipient
        {"Insurance paper",   0x1078, 121, 97, 32, 192, 64, 64},  // 0x1E
        {"Academy paper",     0x107C, 121, 121, 113, 203, 203,  56},  // 0x1F - #797971 text, #CBCB38 recipient
        {"Lovely paper",      0x1080, 195,   0, 154, 219,   0, 113},  // 0x20 - #C3009A text, #DB0071 recipient
        {"Rainbow paper",     0x1084,  97, 105,  97, 251,  65, 251},  // 0x21 - #616961 text, #FB41FB recipient
        {"Egyptian paper",    0x1088, 146,  65,  56,  65, 138, 138},  // 0x22 - #924138 text, #418A8A recipient
        {"Lotus paper",       0x108C,  89,  40,  32, 243,   0, 138},  // 0x23 - #592820 text, #F3008A recipient
        {"Tile paper",        0x1090,  40,  65, 113,   0,  32, 178},  // 0x24 - #284171 text, #0020B2 recipient
        {"Mosaic paper",      0x1094, 170,  73, 178,  89, 121,  40},  // 0x25 - #AA49B2 text, #597928 recipient
        {"Elegant paper",     0x1098, 251, 186, 130, 251, 186, 130},  // 0x26 - #FBBA82 text, #FBBA82 recipient
        {"Town view paper",   0x109C,  89,  89,  65, 251,  40, 178},  // 0x27 - #595941 text, #FB28B2 recipient
        {"Chinese paper",     0x10A0, 105,  56,  56, 219,  24,   8},  // 0x28 - #693838 text, #DB1808 recipient
        {"Ocean paper",       0x10A4, 251, 251, 251,  97, 211, 219},  // 0x29 - #FBFBFB text, #61D3DB recipient
        {"Industrial paper",  0x10A8, 121, 113,  65, 121,  48,  48},  // 0x2A - #797141 text, #793030 recipient
        {"Fireworks paper",   0x10AC, 243, 243, 251,   0, 251, 251},  // 0x2B - #F3F3FB text, #00FBFB recipient
        {"Floral paper",      0x10B0,  97, 105,  56, 195,   0, 113},  // 0x2C - #616938 text, #C30071 recipient
        {"Mushroom paper",    0x10B4, 186, 211, 235, 251, 251, 251},  // 0x2D - #BAD3EB text, #FBFBFB recipient
        {"Star paper",        0x10B8,  40,  32,  97,  56, 146, 178},  // 0x2E - #282061 text, #3892B2 recipient
        {"Composer paper",    0x10BC, 105,  65, 146, 105,  65, 146},  // 0x2F - #694192 text, #694192 recipient
        {"Bathtub paper",     0x10C0,  73, 113, 130,  56,  40, 235},  // 0x30 - #497182 text, #3828EB recipient
        {"SMB3 paper",        0x10C4,  32,  32,  65,   0,  32,  16},  // 0x31 - #202041 text, #002010 recipient
        {"Cool paper",        0x10C8,  16, 105, 146,   0, 130, 170},  // 0x32 - #106992 text, #0082AA recipient
        {"Forest paper",      0x10CC,  48,  73,  56,  81,  24,   0},  // 0x33 - #304938 text, #511800 recipient
        {"Bubble paper",      0x10D0, 130,  65,  65, 130,   8,  65},  // 0x34 - #824141 text, #820841 recipient
        {"Buttercup paper",   0x10D4,  89, 113,  89, 251, 121,  56},  // 0x35 - #597159 text, #FB7938 recipient
        {"Tartan paper",      0x10D8, 219, 219, 251, 203, 243, 251},  // 0x36 - #DBDBFB text, #CBF3FB recipient
        {"Plaid paper",       0x10DC, 251, 203, 203, 251, 251, 251},  // 0x37 - #FBCBCB text, #FBFBFB recipient
        {"Lemon-lime paper",  0x10E0,   0,  73, 178,   0,  73, 178},  // 0x38 - #0049B2 text, #0049B2 recipient
        {"Crater paper",      0x10E4,  32,  16,  73,  56,   8,  32},  // 0x39 - #201049 text, #380820 recipient
        {"Bejeweled paper",   0x10E8, 154, 121,  65, 162,  81,  56},  // 0x3A - #9A7941 text, #A25138 recipient
        {"Geometric paper",   0x10EC,  16,  16,  73,  48,   0,  24},  // 0x3B - #101049 text, #300018 recipient
        {"Southwest paper",   0x10F0,  56,  56,  56,  24, 113, 121},  // 0x3C - #383838 text, #187179 recipient
        {"Night sky paper",   0x10F4, 243, 251, 130, 243, 251, 130},  // 0x3D - #F3FB82 text, #F3FB82 recipient
        {"Chic paper",        0x10F8, 251, 251, 146, 227, 227, 227},  // 0x3E - #FBFB92 text, #E3E3E3 recipient
        {"Goldfish paper",    0x10FC,  73,  97, 138, 251, 105,  73},  // 0x3F - #49618A text, #FB6949 recipient
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
