#pragma once
#include <QImage>
#include <QString>
#include <QColor>
#include <map>
#include <vector>
#include <cstdint>

class NDSRom;

struct GlyphInfo {
    int index;          // Index in the glyph atlas
    int displayWidth;   // Variable width for rendering
    QImage mask;        // 1bpp glyph mask (white = text, transparent = background)
};

class FontLoader {
public:
    static constexpr int GLYPH_WIDTH = 16;
    static constexpr int GLYPH_HEIGHT = 16;

    bool load(NDSRom& rom);

    bool hasGlyph(QChar ch) const;
    const GlyphInfo* getGlyph(QChar ch) const;
    int glyphHeight() const { return GLYPH_HEIGHT; }
    int glyphWidth() const { return GLYPH_WIDTH; }

    // Get display width for a character (or default if not found)
    int charWidth(QChar ch) const;

    // Get a glyph rendered in a specific color
    QImage getColoredGlyph(QChar ch, const QColor& color) const;

private:
    bool loadFontA(NDSRom& rom);
    bool loadFontASub(NDSRom& rom);
    void buildCharacterMap();

    // fontA glyphs (main font - ASCII/Latin-1)
    std::map<uint16_t, GlyphInfo> m_glyphs;  // glyph code -> glyph info

    // fontASub glyphs (special characters: star, heart, music note, etc.)
    std::map<uint16_t, GlyphInfo> m_subGlyphs;  // glyph code -> glyph info

    // Unicode code point -> font source and glyph code
    // Uses uint32_t to support emoji/supplementary plane characters
    struct GlyphMapping {
        bool useSubFont;   // true = fontASub, false = fontA
        uint16_t glyphCode;
    };
    std::map<uint32_t, GlyphMapping> m_charMap;

    // fontA data
    std::vector<uint16_t> m_charCodes;
    std::vector<uint8_t> m_charWidths;
    std::vector<uint8_t> m_imgData;
    int m_numChars = 0;

    // fontASub data
    std::vector<uint16_t> m_subCharCodes;
    std::vector<uint8_t> m_subCharWidths;
    std::vector<uint8_t> m_subImgData;
    int m_subNumChars = 0;
};
