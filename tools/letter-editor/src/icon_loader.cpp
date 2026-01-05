#include "icon_loader.h"
#include "nds_rom.h"
#include "lz77.h"
#include <QDebug>

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

    uint32_t colorToArgb(const Color& c) {
        return (static_cast<uint32_t>(c.a) << 24) |
               (static_cast<uint32_t>(c.r) << 16) |
               (static_cast<uint32_t>(c.g) << 8) |
               static_cast<uint32_t>(c.b);
    }
}

bool IconLoader::load(NDSRom& rom) {
    m_loaded = false;
    m_tileData.clear();
    m_palettes.clear();

    // Load all palettes from b_obj_itm.bpl
    if (!loadBplFile(rom, "/menu/inventory/b_obj_itm.bpl")) {
        qDebug() << "IconLoader: Failed to load palette";
        return false;
    }

    // Load obj0.bch (top halves)
    std::vector<uint8_t> obj0Data;
    if (!loadBchFile(rom, "/menu/inventory/obj0.bch", obj0Data)) {
        qDebug() << "IconLoader: Failed to load obj0.bch";
        return false;
    }

    // Load obj1.bch (bottom halves)
    std::vector<uint8_t> obj1Data;
    if (!loadBchFile(rom, "/menu/inventory/obj1.bch", obj1Data)) {
        qDebug() << "IconLoader: Failed to load obj1.bch";
        return false;
    }

    // Validate tile counts
    if (obj0Data.size() < TILES_PER_FILE * BYTES_PER_TILE) {
        qDebug() << "IconLoader: obj0.bch too small:" << obj0Data.size();
        return false;
    }
    if (obj1Data.size() < TILES_PER_FILE * BYTES_PER_TILE) {
        qDebug() << "IconLoader: obj1.bch too small:" << obj1Data.size();
        return false;
    }

    // Concatenate tile data: obj0 (top halves) + obj1 (bottom halves)
    m_tileData.reserve(obj0Data.size() + obj1Data.size());
    m_tileData.insert(m_tileData.end(), obj0Data.begin(), obj0Data.end());
    m_tileData.insert(m_tileData.end(), obj1Data.begin(), obj1Data.end());

    // Render icon sheets for each palette
    for (int i = 0; i < NUM_PALETTES; i++) {
        renderIconSheet(i);
    }

    m_loaded = true;
    qDebug() << "IconLoader: Loaded" << TOTAL_ICONS << "icons with" << m_palettes.size() << "palettes";
    return true;
}

bool IconLoader::loadBchFile(NDSRom& rom, const std::string& path, std::vector<uint8_t>& outData) {
    auto data = rom.readFile(path);
    if (data.empty()) {
        return false;
    }

    // Check for LZ77 compression
    if (LZ77::isCompressed(data)) {
        data = LZ77::decompress(data);
        if (data.empty()) {
            return false;
        }
    }

    outData = std::move(data);
    return true;
}

bool IconLoader::loadBplFile(NDSRom& rom, const std::string& path) {
    auto data = rom.readFile(path);
    if (data.empty()) {
        return false;
    }

    // Check for LZ77 compression
    if (LZ77::isCompressed(data)) {
        data = LZ77::decompress(data);
        if (data.empty()) {
            return false;
        }
    }

    // BPL format: 32 bytes per sub-palette (16 colors × 2 bytes)
    int numPalettes = static_cast<int>(data.size() / 32);
    if (numPalettes < 1) {
        return false;
    }

    // Limit to NUM_PALETTES
    numPalettes = std::min(numPalettes, NUM_PALETTES);

    m_palettes.resize(numPalettes);
    for (int p = 0; p < numPalettes; p++) {
        m_palettes[p].resize(COLORS_PER_PALETTE);
        int baseOffset = p * 32;

        for (int i = 0; i < COLORS_PER_PALETTE; i++) {
            uint16_t col = data[baseOffset + i * 2] | (data[baseOffset + i * 2 + 1] << 8);
            Color c = decodeRGB555(col);
            // Color index 0 is transparent
            if (i == 0) {
                c.a = 0;
            }
            m_palettes[p][i] = colorToArgb(c);
        }
    }

    return true;
}

void IconLoader::renderIconSheet(int paletteIndex) {
    if (paletteIndex < 0 || paletteIndex >= static_cast<int>(m_palettes.size())) {
        return;
    }

    QImage& sheet = m_iconSheets[paletteIndex];
    sheet = QImage(OUTPUT_WIDTH, OUTPUT_HEIGHT, QImage::Format_ARGB32);
    sheet.fill(Qt::transparent);

    // Render tiles linearly at 32 tiles per row
    // Total tiles: 320 (160 from obj0 + 160 from obj1)
    // Layout: 32 tiles wide × 10 tiles tall = 256×80 pixels
    constexpr int TILES_PER_ROW = 32;  // 16 icons × 2 tiles per icon
    int totalTiles = static_cast<int>(m_tileData.size() / BYTES_PER_TILE);

    for (int t = 0; t < totalTiles && t < 320; t++) {
        int destX = (t % TILES_PER_ROW) * TILE_SIZE;
        int destY = (t / TILES_PER_ROW) * TILE_SIZE;
        renderTile(t, m_palettes[paletteIndex], destX, destY, sheet);
    }
}

void IconLoader::renderTile(int tileIndex, const std::vector<uint32_t>& palette,
                            int destX, int destY, QImage& destImage) {
    size_t tileOffset = tileIndex * BYTES_PER_TILE;
    if (tileOffset + BYTES_PER_TILE > m_tileData.size()) {
        return;
    }

    // 4bpp tile decoding (NDS_FORMAT.md Section 8.7)
    // Each row is 4 bytes (8 pixels × 4 bits = 32 bits)
    for (int row = 0; row < TILE_SIZE; row++) {
        for (int col = 0; col < 4; col++) {  // 4 bytes per row
            uint8_t byte = m_tileData[tileOffset + row * 4 + col];

            // Lower nibble = left pixel, upper nibble = right pixel
            uint8_t pixel0Index = byte & 0x0F;
            uint8_t pixel1Index = (byte >> 4) & 0x0F;

            int x0 = destX + col * 2;
            int x1 = destX + col * 2 + 1;
            int y = destY + row;

            if (x0 < OUTPUT_WIDTH && y < OUTPUT_HEIGHT && pixel0Index < palette.size()) {
                destImage.setPixel(x0, y, palette[pixel0Index]);
            }
            if (x1 < OUTPUT_WIDTH && y < OUTPUT_HEIGHT && pixel1Index < palette.size()) {
                destImage.setPixel(x1, y, palette[pixel1Index]);
            }
        }
    }
}

const QImage& IconLoader::getIconSheet(int paletteIndex) const {
    static QImage empty;
    if (!m_loaded || paletteIndex < 0 || paletteIndex >= NUM_PALETTES) {
        return empty;
    }
    return m_iconSheets[paletteIndex];
}

QImage IconLoader::getIcon(int index, int paletteIndex) const {
    if (!m_loaded || index < 0 || index >= TOTAL_ICONS ||
        paletteIndex < 0 || paletteIndex >= NUM_PALETTES) {
        return QImage();
    }

    const QImage& sheet = m_iconSheets[paletteIndex];
    if (sheet.isNull()) {
        return QImage();
    }

    // Calculate icon position in the sheet
    // Icons are 16x16, arranged 16 per row
    int col = index % ICONS_PER_ROW;
    int row = index / ICONS_PER_ROW;
    int x = col * ICON_SIZE;
    int y = row * ICON_SIZE;

    return sheet.copy(x, y, ICON_SIZE, ICON_SIZE);
}
