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
    void buildCharacterMap();

    std::map<uint16_t, GlyphInfo> m_glyphs;  // ACWW char code -> glyph info
    std::map<QChar, uint16_t> m_asciiToAcww; // ASCII/Unicode -> ACWW char code

    std::vector<uint16_t> m_charCodes;
    std::vector<uint8_t> m_charWidths;
    std::vector<uint8_t> m_imgData;
    int m_numChars = 0;
};
