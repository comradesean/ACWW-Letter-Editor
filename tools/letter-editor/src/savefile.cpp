#include "savefile.h"
#include <QFile>
#include <QDebug>
#include <QVariantMap>

// ACWW Western character encoding table for player names
static const char* acwwCharTable[256] = {
    "\0", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "a", "b", "c", "d", "e",
    "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u",
    "v", "w", "x", "y", "z", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "f",
    "s", "O", "Z", "s", "o", "z", "Y", "A", "A", "A", "A", "A", "A", "A", "C", "E",
    "E", "E", "E", "I", "I", "I", "I", "D", "N", "O", "O", "O", "O", "O", "O", "U",
    "U", "U", "U", "Y", "T", "s", "a", "a", "a", "a", "a", "a", "a", "c", "e", "e",
    "e", "e", "i", "i", "i", "i", "d", "n", "o", "o", "o", "o", "o", "o", "u", "u",
    "u", "u", "y", "t", "y", " ", "\n", "!", "\"", "#", "$", "%", "&", "'", "(", ")",
    "*", "+", ",", "-", ".", "/", ":", ";", "<", "=", ">", "?", "@", "[", "{", "]",
    "|", "_", "}", ",", ".", ".", "~", "L", "+", "+", "^", "%", "<", "`", "\"", "*",
    "-", "'", "-", "\"", "T", ">", " ", "~", "Y", "|", "S", "!", "c", "L", " ", "c",
    "a", "<", "-", "-", "R", "o", "+", "2", "3", "-", "s", "P", ">", "1", "o",
    ">", ".", "1", "1", "3", " ", " ", " ", " ", "?", "x", "/", " ", "*", " ", " ",
    " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ",
    " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "
};

static QString decodeAcwwText(const QByteArray& data) {
    QString result;
    for (int i = 0; i < data.size(); i++) {
        uint8_t byte = static_cast<uint8_t>(data[i]);
        if (byte == 0) continue;
        result += QString::fromUtf8(acwwCharTable[byte]);
    }
    return result.trimmed();
}

// Letter slot offsets for EUR/USA region
// Inventory slots (10 per player)
static const uint32_t INVENTORY_OFFSETS[4][10] = {
    // Player 1
    {0x1158, 0x124C, 0x1340, 0x1434, 0x1528, 0x161C, 0x1710, 0x1804, 0x18F8, 0x19EC},
    // Player 2
    {0x33E4, 0x34D8, 0x35CC, 0x36C0, 0x37B4, 0x38A8, 0x399C, 0x3A90, 0x3B84, 0x3C78},
    // Player 3
    {0x5670, 0x5764, 0x5858, 0x594C, 0x5A40, 0x5B34, 0x5C28, 0x5D1C, 0x5E10, 0x5F04},
    // Player 4
    {0x78FC, 0x79F0, 0x7AE4, 0x7BD8, 0x7CCC, 0x7DC0, 0x7EB4, 0x7FA8, 0x809C, 0x8190}
};

// Inventory backup offsets
static const uint32_t INVENTORY_BACKUP_OFFSETS[4][10] = {
    // Player 1
    {0x17138, 0x1722C, 0x17320, 0x17414, 0x17508, 0x175FC, 0x176F0, 0x177E4, 0x178D8, 0x179CC},
    // Player 2
    {0x193C4, 0x194B8, 0x195AC, 0x196A0, 0x19794, 0x19888, 0x1997C, 0x19A70, 0x19B64, 0x19C58},
    // Player 3
    {0x1B650, 0x1B744, 0x1B838, 0x1B92C, 0x1BA20, 0x1BB14, 0x1BC08, 0x1BCFC, 0x1BDF0, 0x1BEE4},
    // Player 4
    {0x1D8DC, 0x1D9D0, 0x1DAC4, 0x1DBB8, 0x1DCAC, 0x1DDA0, 0x1DE94, 0x1DF88, 0x1E07C, 0x1E170}
};

// Mailbox slots (10 per player)
static const uint32_t MAILBOX_OFFSETS[4][10] = {
    // Player 1
    {0x12010, 0x12104, 0x121F8, 0x122EC, 0x123E0, 0x124D4, 0x125C8, 0x126BC, 0x127B0, 0x128A4},
    // Player 2
    {0x1299C, 0x12A90, 0x12B84, 0x12C78, 0x12D6C, 0x12E60, 0x12F54, 0x13048, 0x1313C, 0x13230},
    // Player 3
    {0x13328, 0x1341C, 0x13510, 0x13604, 0x136F8, 0x137EC, 0x138E0, 0x139D4, 0x13AC8, 0x13BBC},
    // Player 4
    {0x13CB4, 0x13DA8, 0x13E9C, 0x13F90, 0x14084, 0x14178, 0x1426C, 0x14360, 0x14454, 0x14548}
};

// Mailbox backup offsets
static const uint32_t MAILBOX_BACKUP_OFFSETS[4][10] = {
    // Player 1
    {0x27FF0, 0x280E4, 0x281D8, 0x282CC, 0x283C0, 0x284B4, 0x285A8, 0x2869C, 0x28790, 0x28884},
    // Player 2
    {0x2897C, 0x28A70, 0x28B64, 0x28C58, 0x28D4C, 0x28E40, 0x28F34, 0x29028, 0x2911C, 0x29210},
    // Player 3
    {0x29308, 0x293FC, 0x294F0, 0x295E4, 0x296D8, 0x297CC, 0x298C0, 0x299B4, 0x29AA8, 0x29B9C},
    // Player 4
    {0x29C94, 0x29D88, 0x29E7C, 0x29F70, 0x2A064, 0x2A158, 0x2A24C, 0x2A340, 0x2A434, 0x2A528}
};

// Bank storage slots (75 per player) - Player 1 only shown, others calculated
static const uint32_t BANK_PLAYER1_START = 0x2E210;
static const uint32_t BANK_PLAYER_STRIDE = 0x4778;  // Offset between players

// Player data structure offsets (EUR/USA)
// Player data starts at 0x0C in the save file
// Each player block is 0x228C bytes
// Player name is at offset 0x2282 within each player's block
static const uint32_t PLAYER_DATA_START = 0x0C;
static const uint32_t PLAYER_DATA_SIZE = 0x228C;
static const uint32_t PLAYER_NAME_OFFSET_EUR = 0x2282;

static const uint32_t PLAYER_NAME_OFFSETS[4] = {
    PLAYER_DATA_START + PLAYER_NAME_OFFSET_EUR,                           // Player 1: 0x228E
    PLAYER_DATA_START + PLAYER_DATA_SIZE + PLAYER_NAME_OFFSET_EUR,        // Player 2: 0x451A
    PLAYER_DATA_START + PLAYER_DATA_SIZE * 2 + PLAYER_NAME_OFFSET_EUR,    // Player 3: 0x67A6
    PLAYER_DATA_START + PLAYER_DATA_SIZE * 3 + PLAYER_NAME_OFFSET_EUR     // Player 4: 0x8A32
};

// Region data
static const SaveFile::RegionData EUR_USA_DATA = {
    0x15FDC,   // checksumOffset
    0x15FE0,   // saveCopyOffset (where backup starts)
    0x15FE0,   // saveCopySize
    0x000C,    // playerDataOffset
    0x228C     // playerDataSize
};

static const SaveFile::RegionData JPN_DATA = {
    0x12220,   // checksumOffset
    0x12224,   // saveCopyOffset
    0x12224,   // saveCopySize
    0x000C,    // playerDataOffset (estimated)
    0x1C00     // playerDataSize (estimated)
};

static const SaveFile::RegionData KOR_DATA = {
    0x173F8,   // checksumOffset
    0x173FC,   // saveCopyOffset
    0x173FC,   // saveCopySize
    0x000C,    // playerDataOffset (estimated)
    0x2000     // playerDataSize (estimated)
};

const SaveFile::RegionData& SaveFile::getRegionData(Region region) {
    switch (region) {
        case Region::JPN: return JPN_DATA;
        case Region::KOR: return KOR_DATA;
        case Region::EUR_USA:
        default: return EUR_USA_DATA;
    }
}

SaveFile::SaveFile(QObject* parent)
    : QObject(parent)
{
}

bool SaveFile::load(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "SaveFile::load: Failed to open file:" << path;
        return false;
    }

    m_data = file.readAll();
    file.close();

    // Validate file size
    if (m_data.size() != SAVE_SIZE_256K && m_data.size() != SAVE_SIZE_512K) {
        qDebug() << "SaveFile::load: Invalid file size:" << m_data.size();
        m_data.clear();
        return false;
    }

    // Detect region
    m_region = detectRegion();
    if (m_region == Region::Unknown) {
        qDebug() << "SaveFile::load: Unknown region";
        // Continue anyway, default to EUR/USA
        m_region = Region::EUR_USA;
    }

    m_filePath = path;
    m_loaded = true;
    m_modified = false;

    qDebug() << "SaveFile::load: Loaded" << path << "region:" << regionName();

    emit loadedChanged();
    return true;
}

bool SaveFile::save(const QString& path) {
    QString savePath = path.isEmpty() ? m_filePath : path;
    if (savePath.isEmpty()) {
        qDebug() << "SaveFile::save: No file path specified";
        return false;
    }

    if (!m_loaded) {
        qDebug() << "SaveFile::save: No save file loaded";
        return false;
    }

    // Fix checksum and copy to backup before saving
    fixChecksum();
    copyToBackup();

    QFile file(savePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "SaveFile::save: Failed to open file for writing:" << savePath;
        return false;
    }

    qint64 written = file.write(m_data);
    file.close();

    if (written != m_data.size()) {
        qDebug() << "SaveFile::save: Failed to write all data";
        return false;
    }

    m_filePath = savePath;
    m_modified = false;

    qDebug() << "SaveFile::save: Saved to" << savePath;

    emit modifiedChanged();
    return true;
}

QString SaveFile::regionName() const {
    switch (m_region) {
        case Region::EUR_USA: return "EUR/USA";
        case Region::JPN: return "Japan";
        case Region::KOR: return "Korea";
        default: return "Unknown";
    }
}

QString SaveFile::getPlayerName(int player) const {
    if (!m_loaded || player < 0 || player >= PLAYER_COUNT) {
        return QString();
    }

    uint32_t offset = PLAYER_NAME_OFFSETS[player];
    if (offset + 8 > static_cast<uint32_t>(m_data.size())) {
        return QString();
    }

    QByteArray nameData = m_data.mid(offset, 8);
    return decodeAcwwText(nameData);
}

bool SaveFile::playerExists(int player) const {
    QString name = getPlayerName(player);
    return !name.isEmpty();
}

QStringList SaveFile::getPlayerNames() const {
    QStringList names;
    for (int i = 0; i < PLAYER_COUNT; i++) {
        QString name = getPlayerName(i);
        if (name.isEmpty()) {
            name = QString("Player %1").arg(i + 1);
        }
        names.append(name);
    }
    return names;
}

Letter SaveFile::getLetter(int player, int storageType, int slot) const {
    if (!m_loaded || player < 0 || player >= PLAYER_COUNT) {
        return Letter();
    }

    StorageType type = static_cast<StorageType>(storageType);
    int maxSlots = getSlotCount(storageType);
    if (slot < 0 || slot >= maxSlots) {
        return Letter();
    }

    uint32_t offset = getLetterOffset(player, type, slot);
    if (offset + LetterFormat::SIZE > static_cast<uint32_t>(m_data.size())) {
        return Letter();
    }

    QByteArray letterData = m_data.mid(offset, LetterFormat::SIZE);
    return Letter::fromBytes(letterData);
}

void SaveFile::setLetter(int player, int storageType, int slot, const Letter& letter) {
    if (!m_loaded || player < 0 || player >= PLAYER_COUNT) {
        return;
    }

    StorageType type = static_cast<StorageType>(storageType);
    int maxSlots = getSlotCount(storageType);
    if (slot < 0 || slot >= maxSlots) {
        return;
    }

    uint32_t offset = getLetterOffset(player, type, slot);
    if (offset + LetterFormat::SIZE > static_cast<uint32_t>(m_data.size())) {
        return;
    }

    // Serialize letter and write to save data
    QByteArray letterData = letter.toBytes();
    for (int i = 0; i < LetterFormat::SIZE; i++) {
        m_data[offset + i] = letterData[i];
    }

    // Also write to backup location if applicable
    uint32_t backupOffset = getBackupLetterOffset(player, type, slot);
    if (backupOffset > 0 && backupOffset + LetterFormat::SIZE <= static_cast<uint32_t>(m_data.size())) {
        for (int i = 0; i < LetterFormat::SIZE; i++) {
            m_data[backupOffset + i] = letterData[i];
        }
    }

    m_modified = true;
    emit modifiedChanged();
    emit letterModified(player, storageType, slot);
}

int SaveFile::getSlotCount(int storageType) const {
    switch (static_cast<StorageType>(storageType)) {
        case StorageType::Inventory: return INVENTORY_SLOTS;
        case StorageType::Mailbox: return MAILBOX_SLOTS;
        case StorageType::Bank: return BANK_SLOTS;
        default: return 0;
    }
}

QVariantList SaveFile::getSlotSummaries(int player, int storageType) const {
    QVariantList summaries;
    int slotCount = getSlotCount(storageType);

    for (int i = 0; i < slotCount; i++) {
        Letter letter = getLetter(player, storageType, i);
        QVariantMap summary;
        summary["isEmpty"] = letter.isEmpty();
        summary["recipientName"] = letter.toPlayerName;
        summary["senderName"] = letter.fromPlayerName;
        summary["hasAttachment"] = letter.attachedItem != 0;
        summary["stationery"] = letter.stationeryType;
        summaries.append(summary);
    }

    return summaries;
}

bool SaveFile::validateChecksum() const {
    if (!m_loaded) return false;

    const RegionData& rd = getRegionData(m_region);
    if (rd.checksumOffset + 2 > static_cast<uint32_t>(m_data.size())) {
        return false;
    }

    uint16_t stored = static_cast<uint8_t>(m_data[rd.checksumOffset]) |
                      (static_cast<uint8_t>(m_data[rd.checksumOffset + 1]) << 8);
    uint16_t calculated = calculateChecksum();

    return stored == calculated;
}

void SaveFile::fixChecksum() {
    if (!m_loaded) return;

    const RegionData& rd = getRegionData(m_region);
    if (rd.checksumOffset + 2 > static_cast<uint32_t>(m_data.size())) {
        return;
    }

    uint16_t checksum = calculateChecksum();

    // Write checksum (little-endian)
    m_data[rd.checksumOffset] = static_cast<char>(checksum & 0xFF);
    m_data[rd.checksumOffset + 1] = static_cast<char>((checksum >> 8) & 0xFF);

    qDebug() << "SaveFile::fixChecksum: New checksum:" << QString::number(checksum, 16).toUpper();
}

SaveFile::Region SaveFile::detectRegion() const {
    if (m_data.size() < 4) return Region::Unknown;

    // Check game identifier at offset 0x00
    // EUR/USA saves typically have specific patterns
    // For now, default to EUR/USA as it's most common

    // Game code check could be done at offset 0x00-0x03
    // but save files don't always contain this reliably

    // Check save data size patterns to determine region
    // EUR/USA: checksum at 0x15FDC
    // JPN: checksum at 0x12220
    // KOR: checksum at 0x173F8

    // Simple heuristic: check for valid data at expected offsets
    // Default to EUR/USA for now
    return Region::EUR_USA;
}

uint32_t SaveFile::getLetterOffset(int player, StorageType type, int slot) const {
    // Only EUR/USA offsets are fully documented
    if (m_region != Region::EUR_USA) {
        qDebug() << "SaveFile::getLetterOffset: Non-EUR/USA region not fully supported";
    }

    switch (type) {
        case StorageType::Inventory:
            if (slot >= 0 && slot < INVENTORY_SLOTS) {
                return INVENTORY_OFFSETS[player][slot];
            }
            break;

        case StorageType::Mailbox:
            if (slot >= 0 && slot < MAILBOX_SLOTS) {
                return MAILBOX_OFFSETS[player][slot];
            }
            break;

        case StorageType::Bank:
            if (slot >= 0 && slot < BANK_SLOTS) {
                // Bank slots are contiguous: base + (player * stride) + (slot * letter_size)
                return BANK_PLAYER1_START + (player * BANK_PLAYER_STRIDE) + (slot * LetterFormat::SIZE);
            }
            break;
    }

    return 0;
}

uint32_t SaveFile::getBackupLetterOffset(int player, StorageType type, int slot) const {
    switch (type) {
        case StorageType::Inventory:
            if (slot >= 0 && slot < INVENTORY_SLOTS) {
                return INVENTORY_BACKUP_OFFSETS[player][slot];
            }
            break;

        case StorageType::Mailbox:
            if (slot >= 0 && slot < MAILBOX_SLOTS) {
                return MAILBOX_BACKUP_OFFSETS[player][slot];
            }
            break;

        case StorageType::Bank:
            // Bank storage doesn't have a separate backup section in the documented format
            return 0;
    }

    return 0;
}

uint16_t SaveFile::calculateChecksum() const {
    const RegionData& rd = getRegionData(m_region);

    uint32_t sum = 0;

    // Sum all uint16 values in the save region except the checksum itself
    for (uint32_t offset = 0; offset < rd.saveCopySize && offset + 1 < static_cast<uint32_t>(m_data.size()); offset += 2) {
        // Skip the checksum location
        if (offset == rd.checksumOffset) {
            continue;
        }

        uint16_t value = static_cast<uint8_t>(m_data[offset]) |
                        (static_cast<uint8_t>(m_data[offset + 1]) << 8);
        sum += value;
    }

    // Return two's complement of the lower 16 bits
    return static_cast<uint16_t>((~sum + 1) & 0xFFFF);
}

void SaveFile::copyToBackup() {
    const RegionData& rd = getRegionData(m_region);

    // Copy primary save data to backup location
    if (rd.saveCopyOffset + rd.saveCopySize <= static_cast<uint32_t>(m_data.size())) {
        for (uint32_t i = 0; i < rd.saveCopySize; i++) {
            m_data[rd.saveCopyOffset + i] = m_data[i];
        }
        qDebug() << "SaveFile::copyToBackup: Copied" << rd.saveCopySize << "bytes to backup";
    }
}

uint32_t SaveFile::getPlayerNameOffset(int player) const {
    if (player < 0 || player >= PLAYER_COUNT) {
        return 0;
    }
    return PLAYER_NAME_OFFSETS[player];
}
