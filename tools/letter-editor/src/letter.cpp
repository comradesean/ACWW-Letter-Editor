#include "letter.h"
#include <QDebug>

// ACWW Western character encoding table (256 entries)
// Maps byte values 0x00-0xFF to Unicode characters
static const char* acwwCharTable[256] = {
    "\0", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",        // 0x00-0x0F
    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "a", "b", "c", "d", "e",         // 0x10-0x1F
    "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u",         // 0x20-0x2F
    "v", "w", "x", "y", "z", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "f",         // 0x30-0x3F
    "s", "O", "Z", "s", "o", "z", "Y", "A", "A", "A", "A", "A", "A", "A", "C", "E",         // 0x40-0x4F (accented)
    "E", "E", "E", "I", "I", "I", "I", "D", "N", "O", "O", "O", "O", "O", "O", "U",         // 0x50-0x5F
    "U", "U", "U", "Y", "T", "s", "a", "a", "a", "a", "a", "a", "a", "c", "e", "e",         // 0x60-0x6F
    "e", "e", "i", "i", "i", "i", "d", "n", "o", "o", "o", "o", "o", "o", "u", "u",         // 0x70-0x7F
    "u", "u", "y", "t", "y", " ", "\n", "!", "\"", "#", "$", "%", "&", "'", "(", ")",       // 0x80-0x8F
    "*", "+", ",", "-", ".", "/", ":", ";", "<", "=", ">", "?", "@", "[", "{", "]",         // 0x90-0x9F
    "|", "_", "}", ",", ".", ".", "~", "L", "+", "+", "^", "%", "<", "`", "\"", "*",        // 0xA0-0xAF
    "-", "'", "-", "\"", "T", ">", " ", "~", "Y", "|", "S", "!", "c", "L", " ", "c",        // 0xB0-0xBF
    "a", "<", "-", "-", "R", "o", "+", "2", "3", "-", "s", "P", ">", "1", "o",              // 0xC0-0xCE
    ">", ".", "1", "1", "3", " ", " ", " ", " ", "?", "x", "/", " ", "*", " ", " ",         // 0xCF-0xDF (special chars)
    " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ",         // 0xE0-0xEF
    " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "          // 0xF0-0xFF
};

// Decode ACWW bytes to QString
static QString decodeAcwwText(const QByteArray& data) {
    QString result;
    for (int i = 0; i < data.size(); i++) {
        uint8_t byte = static_cast<uint8_t>(data[i]);
        if (byte == 0) {
            // Null byte - end of string or padding
            continue;
        }
        result += QString::fromUtf8(acwwCharTable[byte]);
    }
    return result.trimmed();
}

// Encode QString to ACWW bytes
static QByteArray encodeAcwwText(const QString& text, int maxLen) {
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

// Helper to read uint16 little-endian from byte array
static uint16_t readU16LE(const QByteArray& data, int offset) {
    if (offset + 1 >= data.size()) return 0;
    return static_cast<uint8_t>(data[offset]) |
           (static_cast<uint8_t>(data[offset + 1]) << 8);
}

// Helper to read uint32 little-endian from byte array
static uint32_t readU32LE(const QByteArray& data, int offset) {
    if (offset + 3 >= data.size()) return 0;
    return static_cast<uint8_t>(data[offset]) |
           (static_cast<uint8_t>(data[offset + 1]) << 8) |
           (static_cast<uint8_t>(data[offset + 2]) << 16) |
           (static_cast<uint8_t>(data[offset + 3]) << 24);
}

// Helper to write uint16 little-endian to byte array
static void writeU16LE(QByteArray& data, int offset, uint16_t value) {
    if (offset + 1 >= data.size()) return;
    data[offset] = static_cast<char>(value & 0xFF);
    data[offset + 1] = static_cast<char>((value >> 8) & 0xFF);
}

// Helper to write uint32 little-endian to byte array
static void writeU32LE(QByteArray& data, int offset, uint32_t value) {
    if (offset + 3 >= data.size()) return;
    data[offset] = static_cast<char>(value & 0xFF);
    data[offset + 1] = static_cast<char>((value >> 8) & 0xFF);
    data[offset + 2] = static_cast<char>((value >> 16) & 0xFF);
    data[offset + 3] = static_cast<char>((value >> 24) & 0xFF);
}

bool Letter::isEmpty() const {
    // A letter is empty if it has no recipient name and no body content
    return toPlayerName.isEmpty() && body.isEmpty() && greeting.isEmpty();
}

QString Letter::getGreetingWithName() const {
    if (namePosition > 0 && namePosition <= greeting.length() && !toPlayerName.isEmpty()) {
        return greeting.left(namePosition) + toPlayerName + greeting.mid(namePosition);
    }
    return greeting;
}

QByteArray Letter::toBytes() const {
    QByteArray data(LetterFormat::SIZE, 0);

    // Write recipient (To) field
    writeU16LE(data, LetterFormat::TO_TOWN_ID_OFFSET, toTownId);
    QByteArray toTownEncoded = encodeAcwwText(toTownName, LetterFormat::TOWN_SIZE);
    for (int i = 0; i < LetterFormat::TOWN_SIZE; i++) {
        data[LetterFormat::TO_TOWN_OFFSET + i] = toTownEncoded[i];
    }
    writeU16LE(data, LetterFormat::TO_PLAYER_ID_OFFSET, toPlayerId);
    QByteArray toNameEncoded = encodeAcwwText(toPlayerName, LetterFormat::NAME_SIZE);
    for (int i = 0; i < LetterFormat::NAME_SIZE; i++) {
        data[LetterFormat::TO_NAME_OFFSET + i] = toNameEncoded[i];
    }

    // Write receiver flags
    writeU32LE(data, LetterFormat::RECEIVER_FLAGS_OFFSET, receiverFlags);

    // Write sender (From) field
    writeU16LE(data, LetterFormat::FROM_TOWN_ID_OFFSET, fromTownId);
    QByteArray fromTownEncoded = encodeAcwwText(fromTownName, LetterFormat::TOWN_SIZE);
    for (int i = 0; i < LetterFormat::TOWN_SIZE; i++) {
        data[LetterFormat::FROM_TOWN_OFFSET + i] = fromTownEncoded[i];
    }
    writeU16LE(data, LetterFormat::FROM_PLAYER_ID_OFFSET, fromPlayerId);
    QByteArray fromNameEncoded = encodeAcwwText(fromPlayerName, LetterFormat::NAME_SIZE);
    for (int i = 0; i < LetterFormat::NAME_SIZE; i++) {
        data[LetterFormat::FROM_NAME_OFFSET + i] = fromNameEncoded[i];
    }

    // Write sender flags
    writeU32LE(data, LetterFormat::SENDER_FLAGS_OFFSET, senderFlags);

    // Write greeting
    QByteArray greetingEncoded = encodeAcwwText(greeting, LetterFormat::GREETING_SIZE);
    for (int i = 0; i < LetterFormat::GREETING_SIZE; i++) {
        data[LetterFormat::GREETING_OFFSET + i] = greetingEncoded[i];
    }

    // Write body
    QByteArray bodyEncoded = encodeAcwwText(body, LetterFormat::BODY_SIZE);
    for (int i = 0; i < LetterFormat::BODY_SIZE; i++) {
        data[LetterFormat::BODY_OFFSET + i] = bodyEncoded[i];
    }

    // Write signature
    QByteArray sigEncoded = encodeAcwwText(signature, LetterFormat::SIGNATURE_SIZE);
    for (int i = 0; i < LetterFormat::SIGNATURE_SIZE; i++) {
        data[LetterFormat::SIGNATURE_OFFSET + i] = sigEncoded[i];
    }

    // Write metadata
    data[LetterFormat::NAME_POS_OFFSET] = static_cast<char>(namePosition);
    data[LetterFormat::STATIONERY_OFFSET] = static_cast<char>(stationeryType & 0x3F);
    data[LetterFormat::STATUS_OFFSET] = static_cast<char>(status);
    data[LetterFormat::ORIGIN_OFFSET] = static_cast<char>(originType);
    writeU16LE(data, LetterFormat::ITEM_OFFSET, attachedItem);

    return data;
}

Letter Letter::fromBytes(const QByteArray& data) {
    Letter letter;

    if (data.size() < LetterFormat::SIZE) {
        qDebug() << "Letter::fromBytes: Invalid data size:" << data.size();
        return letter;
    }

    // Read recipient (To) field
    letter.toTownId = readU16LE(data, LetterFormat::TO_TOWN_ID_OFFSET);
    letter.toTownName = decodeAcwwText(data.mid(LetterFormat::TO_TOWN_OFFSET, LetterFormat::TOWN_SIZE));
    letter.toPlayerId = readU16LE(data, LetterFormat::TO_PLAYER_ID_OFFSET);
    letter.toPlayerName = decodeAcwwText(data.mid(LetterFormat::TO_NAME_OFFSET, LetterFormat::NAME_SIZE));

    // Read receiver flags
    letter.receiverFlags = readU32LE(data, LetterFormat::RECEIVER_FLAGS_OFFSET);

    // Read sender (From) field
    letter.fromTownId = readU16LE(data, LetterFormat::FROM_TOWN_ID_OFFSET);
    letter.fromTownName = decodeAcwwText(data.mid(LetterFormat::FROM_TOWN_OFFSET, LetterFormat::TOWN_SIZE));
    letter.fromPlayerId = readU16LE(data, LetterFormat::FROM_PLAYER_ID_OFFSET);
    letter.fromPlayerName = decodeAcwwText(data.mid(LetterFormat::FROM_NAME_OFFSET, LetterFormat::NAME_SIZE));

    // Read sender flags
    letter.senderFlags = readU32LE(data, LetterFormat::SENDER_FLAGS_OFFSET);

    // Read letter content
    letter.greeting = decodeAcwwText(data.mid(LetterFormat::GREETING_OFFSET, LetterFormat::GREETING_SIZE));
    letter.body = decodeAcwwText(data.mid(LetterFormat::BODY_OFFSET, LetterFormat::BODY_SIZE));
    letter.signature = decodeAcwwText(data.mid(LetterFormat::SIGNATURE_OFFSET, LetterFormat::SIGNATURE_SIZE));

    // Read metadata
    letter.namePosition = static_cast<uint8_t>(data[LetterFormat::NAME_POS_OFFSET]);
    letter.stationeryType = static_cast<uint8_t>(data[LetterFormat::STATIONERY_OFFSET]) & 0x3F;
    letter.status = static_cast<uint8_t>(data[LetterFormat::STATUS_OFFSET]);
    letter.originType = static_cast<uint8_t>(data[LetterFormat::ORIGIN_OFFSET]);
    letter.attachedItem = readU16LE(data, LetterFormat::ITEM_OFFSET);

    return letter;
}
