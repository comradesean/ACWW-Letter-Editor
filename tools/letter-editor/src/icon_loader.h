#pragma once
#include <QImage>
#include <cstdint>
#include <vector>
#include <string>

class NDSRom;

/**
 * IconLoader - Loads inventory icons from ROM
 *
 * Extracts obj0.bch + obj1.bch from /menu/inventory/ and combines them
 * into a single spritesheet of 80 16x16 icons.
 *
 * Format details (from NDS_FORMAT.md Section 8):
 * - BCH: 4bpp tile data, 32 bytes per 8x8 tile, no header
 * - BPL: RGB555 palette, 32 bytes per sub-palette (16 colors)
 * - obj0.bch: 160 tiles (5120 bytes) - TOP halves of icons
 * - obj1.bch: 160 tiles (5120 bytes) - BOTTOM halves of icons
 * - Combined: 80 complete 16x16 icons (2x2 tiles each)
 * - Output: 256x80 pixels (16 icons per row x 5 rows)
 * - b_obj_itm.bpl has 3 sub-palettes (indices 0, 1, 2)
 */
class IconLoader {
public:
    // Output dimensions
    static constexpr int ICONS_PER_ROW = 16;
    static constexpr int ICON_ROWS = 5;
    static constexpr int ICON_SIZE = 16;  // 16x16 pixels per icon
    static constexpr int OUTPUT_WIDTH = ICONS_PER_ROW * ICON_SIZE;   // 256
    static constexpr int OUTPUT_HEIGHT = ICON_ROWS * ICON_SIZE;      // 80

    // Tile constants
    static constexpr int TILE_SIZE = 8;          // 8x8 pixels per tile
    static constexpr int BYTES_PER_TILE = 32;    // 4bpp = 32 bytes per tile
    static constexpr int TILES_PER_FILE = 160;   // Each obj file has 160 tiles
    static constexpr int TOTAL_ICONS = 80;       // 160 tiles / 2 = 80 icons (top+bottom)

    // Palette constants
    static constexpr int NUM_PALETTES = 3;       // b_obj_itm.bpl has 3 sub-palettes
    static constexpr int COLORS_PER_PALETTE = 16;

    bool load(NDSRom& rom);

    // Get icon sheet rendered with default palette (0)
    const QImage& getIconSheet() const { return m_iconSheets[0]; }

    // Get icon sheet rendered with specific palette (0-2)
    const QImage& getIconSheet(int paletteIndex) const;

    // Get individual icon with default palette (0)
    QImage getIcon(int index) const { return getIcon(index, 0); }

    // Get individual icon with specific palette (0-2)
    QImage getIcon(int index, int paletteIndex) const;

    bool isLoaded() const { return m_loaded; }
    int iconCount() const { return TOTAL_ICONS; }
    int paletteCount() const { return NUM_PALETTES; }

private:
    bool loadBchFile(NDSRom& rom, const std::string& path, std::vector<uint8_t>& outData);
    bool loadBplFile(NDSRom& rom, const std::string& path);
    void renderIconSheet(int paletteIndex);
    void renderTile(int tileIndex, const std::vector<uint32_t>& palette,
                    int destX, int destY, QImage& destImage);

    std::vector<uint8_t> m_tileData;                              // Combined tile data
    std::vector<std::vector<uint32_t>> m_palettes;                // All sub-palettes
    QImage m_iconSheets[NUM_PALETTES];                            // Pre-rendered sheets per palette
    bool m_loaded = false;
};
