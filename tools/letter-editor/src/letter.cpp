#include "letter.h"
#include "acww_encoding.h"
#include <QDebug>

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
    if (namePosition >= 0 && namePosition <= greeting.length() && !toPlayerName.isEmpty()) {
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
    letter.attachedItem = readU16LE(data, LetterFormat::ITEM_OFFSET);

    return letter;
}
