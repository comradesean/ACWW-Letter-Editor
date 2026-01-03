#pragma once
#include <QString>
#include <QByteArray>
#include <cstdint>

// Letter structure constants (244 bytes total)
// First 4 bytes are a header (zeros), actual data starts at +0x04
namespace LetterFormat {
    constexpr int SIZE = 0xF4;  // 244 bytes

    // To field (recipient)
    constexpr int TO_TOWN_ID_OFFSET = 0x04;      // 2 bytes (uint16)
    constexpr int TO_TOWN_OFFSET = 0x06;         // 8 bytes (town name)
    constexpr int TO_PLAYER_ID_OFFSET = 0x0E;    // 2 bytes (uint16)
    constexpr int TO_NAME_OFFSET = 0x10;         // 8 bytes (player name)

    // Receiver flags
    constexpr int RECEIVER_FLAGS_OFFSET = 0x18;

    // From field (sender)
    constexpr int FROM_TOWN_ID_OFFSET = 0x1C;    // 2 bytes (uint16)
    constexpr int FROM_TOWN_OFFSET = 0x1E;       // 8 bytes (town name)
    constexpr int FROM_PLAYER_ID_OFFSET = 0x26;  // 2 bytes (uint16)
    constexpr int FROM_NAME_OFFSET = 0x28;       // 8 bytes (player name)

    // Sender flags
    constexpr int SENDER_FLAGS_OFFSET = 0x30;

    // Letter content
    constexpr int GREETING_OFFSET = 0x34;        // 24 bytes
    constexpr int GREETING_SIZE = 0x18;
    constexpr int BODY_OFFSET = 0x4C;            // 128 bytes
    constexpr int BODY_SIZE = 0x80;
    constexpr int SIGNATURE_OFFSET = 0xCC;       // 32 bytes
    constexpr int SIGNATURE_SIZE = 0x20;

    // Metadata
    constexpr int NAME_POS_OFFSET = 0xEC;        // 1 byte - intro index
    constexpr int STATIONERY_OFFSET = 0xED;      // 1 byte (0-63) - paper ID
    constexpr int STATUS_OFFSET = 0xEE;          // 1 byte (flags)
    constexpr int ORIGIN_OFFSET = 0xEF;          // 1 byte (letter origin type)
    constexpr int ITEM_OFFSET = 0xF0;            // 2 bytes (attached item, little-endian)

    // Name/town sizes
    constexpr int NAME_SIZE = 8;
    constexpr int TOWN_SIZE = 8;
}

// Letter status values
namespace LetterStatus {
    constexpr uint8_t UNREAD = 0x42;
    constexpr uint8_t OPENED = 0x03;
    constexpr uint8_t BOTTLE_OPENED = 0x06;
}

// Letter origin type values
namespace LetterOrigin {
    constexpr uint8_t WRITTEN = 0x00;
    constexpr uint8_t NINTENDO = 0x11;
    constexpr uint8_t BOTTLE = 0x0C;
}

struct Letter {
    // Recipient info (To field)
    uint16_t toTownId = 0;
    QString toTownName;
    uint16_t toPlayerId = 0;
    QString toPlayerName;
    uint32_t receiverFlags = 0;

    // Sender info (From field)
    uint16_t fromTownId = 0;
    QString fromTownName;
    uint16_t fromPlayerId = 0;
    QString fromPlayerName;
    uint32_t senderFlags = 0;

    // Letter content
    QString greeting;        // Template without recipient name inserted
    QString body;
    QString signature;

    // Metadata
    uint8_t namePosition = 0;     // Where to insert recipient name in greeting
    uint8_t stationeryType = 0;   // Paper ID (0-63)
    uint8_t status = LetterStatus::UNREAD;
    uint8_t originType = LetterOrigin::WRITTEN;
    uint16_t attachedItem = 0;

    // Check if the letter slot is empty (all zeros or no content)
    bool isEmpty() const;

    // Get greeting with recipient name inserted at namePosition
    QString getGreetingWithName() const;

    // Serialize to 244-byte LTR format
    QByteArray toBytes() const;

    // Deserialize from 244-byte LTR format
    static Letter fromBytes(const QByteArray& data);
};
