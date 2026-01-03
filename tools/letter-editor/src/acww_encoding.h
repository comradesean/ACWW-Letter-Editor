#pragma once

#include <QString>
#include <QByteArray>
#include <cstdint>

// ACWW Western character encoding table (256 entries)
// Maps byte values 0x00-0xFF to Unicode characters
// Note: Some characters have alternate interpretations:
//   0x3F: ⨍ (script f) - could also be ƒ (Latin small f with hook)
//   0x65: β (Greek beta) - could also be ß (German eszett), visually similar
//   0xDC: h (plain h) - could also be ℎ (mathematical italic h)
inline const char* acwwCharTable[256] = {
    // 0x00-0x0F: Null + A-O
    "\0", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
    // 0x10-0x1F: P-Z + a-e
    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "a", "b", "c", "d", "e",
    // 0x20-0x2F: f-u
    "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u",
    // 0x30-0x3F: v-z, 0-9, script f
    "v", "w", "x", "y", "z", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "⨍",
    // 0x40-0x4F: Extended Latin (Œ, Ž, etc.) and accented capitals
    "ⓢ", "Œ", "Ž", "š", "œ", "ž", "Ÿ", "À", "Á", "Â", "Ã", "Ä", "Å", "Æ", "Ç", "È",
    // 0x50-0x5F: More accented capitals
    "É", "Ê", "Ë", "Ì", "Í", "Î", "Ï", "Đ", "Ñ", "Ò", "Ó", "Ô", "Õ", "Ö", "Ø", "Ù",
    // 0x60-0x6F: Accented capitals continued + Greek beta + lowercase accented
    "Ú", "Û", "Ü", "Ý", "Þ", "β", "à", "á", "â", "ã", "ä", "å", "æ", "ç", "è", "é",
    // 0x70-0x7F: Lowercase accented continued
    "ê", "ë", "ì", "í", "î", "ï", "ð", "ñ", "ò", "ó", "ô", "õ", "ö", "ø", "ù", "ú",
    // 0x80-0x8F: More lowercase accented + space, newline, punctuation
    "û", "ü", "ý", "þ", "ÿ", " ", "\n", "!", "\"", "#", "$", "%", "&", "´", "(", ")",
    // 0x90-0x9F: Punctuation continued
    "*", "+", ",", "-", ".", "/", ":", ";", "<", "=", ">", "?", "@", "[", "\\", "]",
    // 0xA0-0xAF: More punctuation and special chars
    "^", "_", "´", "{", "|", "}", "~", "€", ",", "„", "…", "†", "‡", "＾", "‰", "‹",
    // 0xB0-0xBF: Quotes, bullets, dashes, special
    "'", "'", """, """, "•", "–", "—", "˜", "™", "›", " ", "¡", "¢", "£", "¤", "¥",
    // 0xC0-0xCF: Currency, legal, fractions
    "¦", "§", "¨", "©", "ª", "«", "¬", "-", "®", "¯", "°", "±", "²", "³", "´", "µ",
    // 0xD0-0xDF: More special chars, h, star, heart, music note
    "¶", "·", "¸", "¹", "º", "»", "¼", "½", "¾", "¿", "×", "÷", "h", "★", "❤", "♪",
    // 0xE0-0xEF: Unused/reserved
    " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ",
    // 0xF0-0xFF: Unused/reserved
    " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "
};

// Decode ACWW bytes to QString
inline QString decodeAcwwText(const QByteArray& data) {
    QString result;
    for (int i = 0; i < data.size(); i++) {
        uint8_t byte = static_cast<uint8_t>(data[i]);
        if (byte == 0) continue;
        result += QString::fromUtf8(acwwCharTable[byte]);
    }
    return result.trimmed();
}

// Encode QString to ACWW bytes
inline QByteArray encodeAcwwText(const QString& text, int maxLen) {
    QByteArray result(maxLen, 0);
    int pos = 0;

    for (int i = 0; i < text.length() && pos < maxLen; i++) {
        QChar ch = text[i];
        uint8_t encoded = 0x85; // Default to space

        // Find the character in the encoding table
        if (ch == '\n') {
            encoded = 0x86; // Newline
        } else if (ch == ' ') {
            encoded = 0x85; // Space
        } else {
            // Search for matching character
            for (int j = 1; j < 256; j++) {
                if (QString::fromUtf8(acwwCharTable[j]) == QString(ch)) {
                    encoded = static_cast<uint8_t>(j);
                    break;
                }
            }
        }

        result[pos++] = static_cast<char>(encoded);
    }

    return result;
}
