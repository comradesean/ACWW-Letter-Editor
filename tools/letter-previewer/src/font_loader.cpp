#include "font_loader.h"
#include "nds_rom.h"
#include "lz77.h"
#include <QColor>

bool FontLoader::load(NDSRom& rom) {
    if (!loadFontA(rom)) {
        return false;
    }
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

        QImage glyphImg(GLYPH_WIDTH, GLYPH_HEIGHT, QImage::Format_ARGB32);
        glyphImg.fill(Qt::transparent);

        // Decode 1bpp glyph (MSB first, row-major)
        for (int py = 0; py < GLYPH_HEIGHT; py++) {
            for (int px = 0; px < GLYPH_WIDTH; px++) {
                int bitIdx = py * GLYPH_WIDTH + px;
                int byteIdx = bitIdx / 8;
                int bitOffset = 7 - (bitIdx % 8);  // MSB first

                uint8_t byte = m_imgData[glyphDataOffset + byteIdx];
                uint8_t bit = (byte >> bitOffset) & 1;

                if (bit) {
                    glyphImg.setPixelColor(px, py, QColor(121, 97, 32, 255));  // Brown text
                }
            }
        }

        GlyphInfo info;
        info.index = g;
        info.displayWidth = m_charWidths[g];
        info.bitmap = glyphImg;

        m_glyphs[m_charCodes[g]] = info;
    }

    return true;
}

void FontLoader::buildCharacterMap() {
    // Build ASCII to ACWW character code mapping
    // Based on the character codes from fontA which uses ASCII-like encoding
    m_asciiToAcww.clear();

    for (size_t i = 0; i < m_charCodes.size(); i++) {
        uint16_t code = m_charCodes[i];
        // ACWW uses mostly ASCII-compatible codes for basic characters
        if (code >= 0x20 && code < 0x7F) {
            m_asciiToAcww[QChar(code)] = code;
        }
    }
}

bool FontLoader::hasGlyph(QChar ch) const {
    auto it = m_asciiToAcww.find(ch);
    if (it == m_asciiToAcww.end()) {
        return false;
    }
    return m_glyphs.find(it->second) != m_glyphs.end();
}

const GlyphInfo* FontLoader::getGlyph(QChar ch) const {
    auto asciiIt = m_asciiToAcww.find(ch);
    if (asciiIt == m_asciiToAcww.end()) {
        return nullptr;
    }

    auto glyphIt = m_glyphs.find(asciiIt->second);
    if (glyphIt == m_glyphs.end()) {
        return nullptr;
    }

    return &glyphIt->second;
}

int FontLoader::charWidth(QChar ch) const {
    const GlyphInfo* glyph = getGlyph(ch);
    if (glyph) {
        return glyph->displayWidth;
    }
    return GLYPH_WIDTH / 2;  // Default width for unknown chars
}
