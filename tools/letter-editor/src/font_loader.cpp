#include "font_loader.h"
#include "nds_rom.h"
#include "lz77.h"
#include <QColor>
#include <QDebug>

// Note: The font file uses ASCII-like glyph encoding (A=0x41, B=0x42, etc.)
// This is DIFFERENT from ACWW letter text encoding (A=0x01, B=0x02, etc.)
// The letter text encoding table (acwwCharTable) is defined in backend.cpp,
// letter.cpp, and savefile.cpp where it's used for decoding/encoding letter data.

bool FontLoader::load(NDSRom& rom) {
    if (!loadFontA(rom)) {
        return false;
    }
    // fontASub is optional - contains special symbols (heart, star, music note)
    loadFontASub(rom);
    buildCharacterMap();
    return true;
}

bool FontLoader::loadFontA(NDSRom& rom) {
    auto headData = rom.readFile("/font/fontA_head.bin");
    auto attrData = rom.readFile("/font/fontA_attr.bin");
    auto imgData = rom.readFile("/font/fontA_img.bin");

    if (headData.empty() || attrData.empty() || imgData.empty()) {
        return false;
    }

    if (LZ77::isCompressed(headData)) headData = LZ77::decompress(headData);
    if (LZ77::isCompressed(attrData)) attrData = LZ77::decompress(attrData);
    if (LZ77::isCompressed(imgData)) imgData = LZ77::decompress(imgData);

    // Parse header
    m_numChars = headData[0] | (headData[1] << 8);

    // Parse attributes (4 bytes per char: char code + width)
    m_charCodes.clear();
    m_charWidths.clear();
    for (int i = 0; i < m_numChars && i * 4 + 3 < static_cast<int>(attrData.size()); i++) {
        uint16_t charCode = attrData[i * 4] | (attrData[i * 4 + 1] << 8);
        uint16_t width = attrData[i * 4 + 2] | (attrData[i * 4 + 3] << 8);
        m_charCodes.push_back(charCode);
        m_charWidths.push_back(static_cast<uint8_t>(width));
    }

    m_imgData = imgData;

    // Build glyph images
    int bytesPerGlyph = (GLYPH_WIDTH * GLYPH_HEIGHT) / 8;  // 1bpp

    for (int g = 0; g < m_numChars; g++) {
        int glyphDataOffset = g * bytesPerGlyph;
        if (glyphDataOffset + bytesPerGlyph > static_cast<int>(m_imgData.size())) continue;

        QImage glyphMask(GLYPH_WIDTH, GLYPH_HEIGHT, QImage::Format_ARGB32);
        glyphMask.fill(Qt::transparent);

        // Decode 1bpp glyph (MSB first, row-major) - store as white mask
        for (int py = 0; py < GLYPH_HEIGHT; py++) {
            for (int px = 0; px < GLYPH_WIDTH; px++) {
                int bitIdx = py * GLYPH_WIDTH + px;
                int byteIdx = bitIdx / 8;
                int bitOffset = 7 - (bitIdx % 8);  // MSB first

                uint8_t byte = m_imgData[glyphDataOffset + byteIdx];
                uint8_t bit = (byte >> bitOffset) & 1;

                if (bit) {
                    glyphMask.setPixelColor(px, py, QColor(255, 255, 255, 255));  // White mask
                }
            }
        }

        GlyphInfo info;
        info.index = g;
        info.displayWidth = m_charWidths[g];
        info.mask = glyphMask;

        m_glyphs[m_charCodes[g]] = info;
    }

    return true;
}

bool FontLoader::loadFontASub(NDSRom& rom) {
    auto headData = rom.readFile("/font/fontASub_head.bin");
    auto attrData = rom.readFile("/font/fontASub_attr.bin");
    auto imgData = rom.readFile("/font/fontASub_img.bin");

    if (headData.empty() || attrData.empty() || imgData.empty()) {
        qDebug() << "fontASub not found, special characters will not be available";
        return false;
    }

    if (LZ77::isCompressed(headData)) headData = LZ77::decompress(headData);
    if (LZ77::isCompressed(attrData)) attrData = LZ77::decompress(attrData);
    if (LZ77::isCompressed(imgData)) imgData = LZ77::decompress(imgData);

    // Parse header
    m_subNumChars = headData[0] | (headData[1] << 8);

    // Parse attributes (4 bytes per char: char code + width)
    m_subCharCodes.clear();
    m_subCharWidths.clear();
    for (int i = 0; i < m_subNumChars && i * 4 + 3 < static_cast<int>(attrData.size()); i++) {
        uint16_t charCode = attrData[i * 4] | (attrData[i * 4 + 1] << 8);
        uint16_t width = attrData[i * 4 + 2] | (attrData[i * 4 + 3] << 8);
        m_subCharCodes.push_back(charCode);
        m_subCharWidths.push_back(static_cast<uint8_t>(width));
    }

    m_subImgData = imgData;

    // Build glyph images for fontASub
    int bytesPerGlyph = (GLYPH_WIDTH * GLYPH_HEIGHT) / 8;  // 1bpp

    for (int g = 0; g < m_subNumChars; g++) {
        int glyphDataOffset = g * bytesPerGlyph;
        if (glyphDataOffset + bytesPerGlyph > static_cast<int>(m_subImgData.size())) continue;

        QImage glyphMask(GLYPH_WIDTH, GLYPH_HEIGHT, QImage::Format_ARGB32);
        glyphMask.fill(Qt::transparent);

        // Decode 1bpp glyph (MSB first, row-major) - store as white mask
        for (int py = 0; py < GLYPH_HEIGHT; py++) {
            for (int px = 0; px < GLYPH_WIDTH; px++) {
                int bitIdx = py * GLYPH_WIDTH + px;
                int byteIdx = bitIdx / 8;
                int bitOffset = 7 - (bitIdx % 8);  // MSB first

                uint8_t byte = m_subImgData[glyphDataOffset + byteIdx];
                uint8_t bit = (byte >> bitOffset) & 1;

                if (bit) {
                    glyphMask.setPixelColor(px, py, QColor(255, 255, 255, 255));  // White mask
                }
            }
        }

        GlyphInfo info;
        info.index = g;
        info.displayWidth = m_subCharWidths[g];
        info.mask = glyphMask;

        m_subGlyphs[m_subCharCodes[g]] = info;
    }

    qDebug() << "fontASub loaded:" << m_subNumChars << "special character glyphs";
    return true;
}

void FontLoader::buildCharacterMap() {
    // Build Unicode to font glyph code mapping
    // fontA uses ASCII-like glyph codes (A=0x41, etc.)
    // fontASub contains special symbols at codes 0x20-0x27
    m_charMap.clear();

    // Map fontA glyphs (ASCII/Latin-1 characters)
    for (const auto& pair : m_glyphs) {
        uint16_t code = pair.first;
        if (code > 0 && code < 0x100) {
            m_charMap[code] = {false, code};  // useSubFont=false
        }
    }

    // Map extended characters where Unicode code point differs from fontA glyph code
    // fontA uses Windows-1252/Latin-1 compatible glyph positions
    m_charMap[0x2022] = {false, 0x95};  // • BULLET (Windows-1252 position)
    m_charMap[0x0152] = {false, 0x8C};  // Œ LATIN CAPITAL LIGATURE OE
    m_charMap[0x0153] = {false, 0x9C};  // œ LATIN SMALL LIGATURE OE
    m_charMap[0x20AC] = {false, 0x80};  // € EURO SIGN
    m_charMap[0x03B2] = {false, 0xDF};  // β GREEK SMALL LETTER BETA (using ß position)
    m_charMap[0x203E] = {false, 0xAF};  // ‾ OVERLINE (using macron ¯ as substitute)

    // Map special characters from fontASub
    // fontASub glyph layout (0x20-based, in order):
    //   0x20 = space/empty
    //   0x21 = water droplet 💧
    //   0x22 = return/enter block
    //   0x23 = x block
    //   0x24 = star ★
    //   0x25 = right arrow block
    //   0x26 = heart ❤
    //   0x27 = music note ♪
    if (!m_subGlyphs.empty()) {
        // Map Unicode special characters to fontASub glyph codes
        m_charMap[0x2605] = {true, 0x24};   // ★ BLACK STAR
        m_charMap[0x2764] = {true, 0x26};   // ❤ HEAVY BLACK HEART
        m_charMap[0x2665] = {true, 0x26};   // ♥ BLACK HEART SUIT (alias for ❤)
        m_charMap[0x266A] = {true, 0x27};   // ♪ EIGHTH NOTE
        m_charMap[0xE000] = {true, 0x21};   // 💧 DROPLET (stored as PUA U+E000)
        m_charMap[0x1F4A7] = {true, 0x21};  // 💧 DROPLET (for paste detection)

        qDebug() << "fontASub special character mappings enabled";
    }

    qDebug() << "Font has" << m_glyphs.size() << "main glyphs and" << m_subGlyphs.size() << "special glyphs";
}

bool FontLoader::hasGlyph(QChar ch) const {
    uint32_t cp = ch.unicode();
    auto mapIt = m_charMap.find(cp);
    if (mapIt == m_charMap.end()) {
        return false;
    }

    const auto& mapping = mapIt->second;
    if (mapping.useSubFont) {
        return m_subGlyphs.find(mapping.glyphCode) != m_subGlyphs.end();
    } else {
        return m_glyphs.find(mapping.glyphCode) != m_glyphs.end();
    }
}

const GlyphInfo* FontLoader::getGlyph(QChar ch) const {
    uint32_t cp = ch.unicode();
    auto mapIt = m_charMap.find(cp);
    if (mapIt == m_charMap.end()) {
        return nullptr;
    }

    const auto& mapping = mapIt->second;
    if (mapping.useSubFont) {
        auto glyphIt = m_subGlyphs.find(mapping.glyphCode);
        if (glyphIt == m_subGlyphs.end()) {
            return nullptr;
        }
        return &glyphIt->second;
    } else {
        auto glyphIt = m_glyphs.find(mapping.glyphCode);
        if (glyphIt == m_glyphs.end()) {
            return nullptr;
        }
        return &glyphIt->second;
    }
}

int FontLoader::charWidth(QChar ch) const {
    const GlyphInfo* glyph = getGlyph(ch);
    if (glyph) {
        return glyph->displayWidth;
    }
    return GLYPH_WIDTH / 2;  // Default width for unknown chars
}

QImage FontLoader::getColoredGlyph(QChar ch, const QColor& color) const {
    const GlyphInfo* glyph = getGlyph(ch);
    if (!glyph || glyph->mask.isNull()) {
        return QImage();
    }

    QImage colored(GLYPH_WIDTH, GLYPH_HEIGHT, QImage::Format_ARGB32);
    colored.fill(Qt::transparent);

    for (int py = 0; py < GLYPH_HEIGHT; py++) {
        for (int px = 0; px < GLYPH_WIDTH; px++) {
            QColor maskColor = glyph->mask.pixelColor(px, py);
            if (maskColor.alpha() > 0) {
                colored.setPixelColor(px, py, color);
            }
        }
    }

    return colored;
}
