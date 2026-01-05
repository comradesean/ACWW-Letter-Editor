#include "savefile.h"
#include "acww_encoding.h"
#include <QFile>
#include <QDebug>
#include <QVariantMap>

// Letter slot offsets for EUR/USA region (includes 4-byte header at start of each letter)
// Inventory slots (10 per player)
static const uint32_t INVENTORY_OFFSETS[4][10] = {
    // Player 1
    {0x1154, 0x1248, 0x133C, 0x1430, 0x1524, 0x1618, 0x170C, 0x1800, 0x18F4, 0x19E8},
    // Player 2
    {0x33E0, 0x34D4, 0x35C8, 0x36BC, 0x37B0, 0x38A4, 0x3998, 0x3A8C, 0x3B80, 0x3C74},
    // Player 3
    {0x566C, 0x5760, 0x5854, 0x5948, 0x5A3C, 0x5B30, 0x5C24, 0x5D18, 0x5E0C, 0x5F00},
    // Player 4
    {0x78F8, 0x79EC, 0x7AE0, 0x7BD4, 0x7CC8, 0x7DBC, 0x7EB0, 0x7FA4, 0x8098, 0x818C}
};

// Inventory backup offsets
static const uint32_t INVENTORY_BACKUP_OFFSETS[4][10] = {
    // Player 1
    {0x17134, 0x17228, 0x1731C, 0x17410, 0x17504, 0x175F8, 0x176EC, 0x177E0, 0x178D4, 0x179C8},
    // Player 2
    {0x193C0, 0x194B4, 0x195A8, 0x1969C, 0x19790, 0x19884, 0x19978, 0x19A6C, 0x19B60, 0x19C54},
    // Player 3
    {0x1B64C, 0x1B740, 0x1B834, 0x1B928, 0x1BA1C, 0x1BB10, 0x1BC04, 0x1BCF8, 0x1BDEC, 0x1BEE0},
    // Player 4
    {0x1D8D8, 0x1D9CC, 0x1DAC0, 0x1DBB4, 0x1DCA8, 0x1DD9C, 0x1DE90, 0x1DF84, 0x1E078, 0x1E16C}
};

// Mailbox slots (10 per player)
static const uint32_t MAILBOX_OFFSETS[4][10] = {
    // Player 1
    {0x1200C, 0x12100, 0x121F4, 0x122E8, 0x123DC, 0x124D0, 0x125C4, 0x126B8, 0x127AC, 0x128A0},
    // Player 2
    {0x12998, 0x12A8C, 0x12B80, 0x12C74, 0x12D68, 0x12E5C, 0x12F50, 0x13044, 0x13138, 0x1322C},
    // Player 3
    {0x13324, 0x13418, 0x1350C, 0x13600, 0x136F4, 0x137E8, 0x138DC, 0x139D0, 0x13AC4, 0x13BB8},
    // Player 4
    {0x13CB0, 0x13DA4, 0x13E98, 0x13F8C, 0x14080, 0x14174, 0x14268, 0x1435C, 0x14450, 0x14544}
};

// Mailbox backup offsets
static const uint32_t MAILBOX_BACKUP_OFFSETS[4][10] = {
    // Player 1
    {0x27FEC, 0x280E0, 0x281D4, 0x282C8, 0x283BC, 0x284B0, 0x285A4, 0x28698, 0x2878C, 0x28880},
    // Player 2
    {0x28978, 0x28A6C, 0x28B60, 0x28C54, 0x28D48, 0x28E3C, 0x28F30, 0x29024, 0x29118, 0x2920C},
    // Player 3
    {0x29304, 0x293F8, 0x294EC, 0x295E0, 0x296D4, 0x297C8, 0x298BC, 0x299B0, 0x29AA4, 0x29B98},
    // Player 4
    {0x29C90, 0x29D84, 0x29E78, 0x29F6C, 0x2A060, 0x2A154, 0x2A248, 0x2A33C, 0x2A430, 0x2A524}
};

// Bank storage slots (75 per player) - Player 1 only shown, others calculated
static const uint32_t BANK_PLAYER1_START = 0x2E20C;
static const uint32_t BANK_PLAYER_STRIDE = 0x4778;  // Offset between players

// Player data structure offsets (EUR/USA)
// Player data starts at 0x0C in the save file
// Each player block is 0x228C bytes
// Offsets within each player's block:
//   0x2276: Town ID (2 bytes)
//   0x2278: Town Name (8 bytes)
//   0x2280: Player ID (2 bytes)
//   0x2282: Player Name (8 bytes)
static const uint32_t PLAYER_DATA_START = 0x0C;
static const uint32_t PLAYER_DATA_SIZE = 0x228C;
static const uint32_t PLAYER_TOWN_ID_OFFSET_EUR = 0x2276;
static const uint32_t PLAYER_TOWN_NAME_OFFSET_EUR = 0x2278;
static const uint32_t PLAYER_ID_OFFSET_EUR = 0x2280;
static const uint32_t PLAYER_NAME_OFFSET_EUR = 0x2282;
static const uint32_t PLAYER_GENDER_OFFSET_EUR = 0x228A;  // 0=male, 1=female

static const uint32_t PLAYER_NAME_OFFSETS[4] = {
    PLAYER_DATA_START + PLAYER_NAME_OFFSET_EUR,
    PLAYER_DATA_START + PLAYER_DATA_SIZE + PLAYER_NAME_OFFSET_EUR,
    PLAYER_DATA_START + PLAYER_DATA_SIZE * 2 + PLAYER_NAME_OFFSET_EUR,
    PLAYER_DATA_START + PLAYER_DATA_SIZE * 3 + PLAYER_NAME_OFFSET_EUR
};

static const uint32_t PLAYER_TOWN_OFFSETS[4] = {
    PLAYER_DATA_START + PLAYER_TOWN_NAME_OFFSET_EUR,
    PLAYER_DATA_START + PLAYER_DATA_SIZE + PLAYER_TOWN_NAME_OFFSET_EUR,
    PLAYER_DATA_START + PLAYER_DATA_SIZE * 2 + PLAYER_TOWN_NAME_OFFSET_EUR,
    PLAYER_DATA_START + PLAYER_DATA_SIZE * 3 + PLAYER_TOWN_NAME_OFFSET_EUR
};

static const uint32_t PLAYER_ID_OFFSETS[4] = {
    PLAYER_DATA_START + PLAYER_ID_OFFSET_EUR,
    PLAYER_DATA_START + PLAYER_DATA_SIZE + PLAYER_ID_OFFSET_EUR,
    PLAYER_DATA_START + PLAYER_DATA_SIZE * 2 + PLAYER_ID_OFFSET_EUR,
    PLAYER_DATA_START + PLAYER_DATA_SIZE * 3 + PLAYER_ID_OFFSET_EUR
};

static const uint32_t TOWN_ID_OFFSETS[4] = {
    PLAYER_DATA_START + PLAYER_TOWN_ID_OFFSET_EUR,
    PLAYER_DATA_START + PLAYER_DATA_SIZE + PLAYER_TOWN_ID_OFFSET_EUR,
    PLAYER_DATA_START + PLAYER_DATA_SIZE * 2 + PLAYER_TOWN_ID_OFFSET_EUR,
    PLAYER_DATA_START + PLAYER_DATA_SIZE * 3 + PLAYER_TOWN_ID_OFFSET_EUR
};

static const uint32_t PLAYER_GENDER_OFFSETS[4] = {
    PLAYER_DATA_START + PLAYER_GENDER_OFFSET_EUR,
    PLAYER_DATA_START + PLAYER_DATA_SIZE + PLAYER_GENDER_OFFSET_EUR,
    PLAYER_DATA_START + PLAYER_DATA_SIZE * 2 + PLAYER_GENDER_OFFSET_EUR,
    PLAYER_DATA_START + PLAYER_DATA_SIZE * 3 + PLAYER_GENDER_OFFSET_EUR
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

void SaveFile::close() {
    m_data.clear();
    m_filePath.clear();
    m_region = Region::Unknown;
    m_loaded = false;
    m_modified = false;

    emit loadedChanged();
    emit modifiedChanged();
}

QString SaveFile::regionName() const {
    switch (m_region) {
        case Region::EUR_USA: return "EUR/USA";
        case Region::JPN: return "Japan";
        case Region::KOR: return "Korea";
        default: return "Unknown";
    }
}

bool SaveFile::isBankInitialized() const {
    if (!m_loaded) return false;

    // Bank storage range: 0x2E20C to 0x3FFFB
    // If all bytes in this range are 0xFF, the bank is uninitialized
    static constexpr uint32_t BANK_START = 0x2E20C;
    static constexpr uint32_t BANK_END = 0x3FFFB;

    if (BANK_END >= static_cast<uint32_t>(m_data.size())) {
        return false;
    }

    for (uint32_t offset = BANK_START; offset <= BANK_END; offset++) {
        if (static_cast<uint8_t>(m_data[offset]) != 0xFF) {
            return true;  // Found non-0xFF byte, bank is initialized
        }
    }

    return false;  // All 0xFF, bank is uninitialized
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
    if (!m_loaded || player < 0 || player >= PLAYER_COUNT) {
        return false;
    }

    // Check if player has a non-empty name AND a valid player ID
    // Player ID of 0 means the slot is unused (zeroed)
    // Player ID of 0xFFFF means the slot is uninitialized
    QString name = getPlayerName(player);
    int playerId = getPlayerId(player);

    // A player exists if they have a valid player ID (not 0 or 0xFFFF)
    // OR if they have a non-empty name (covers edge cases)
    bool validPlayerId = (playerId != 0 && playerId != 0xFFFF);

    return validPlayerId || !name.isEmpty();
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

QString SaveFile::getPlayerTown(int player) const {
    if (!m_loaded || player < 0 || player >= PLAYER_COUNT) {
        return QString();
    }

    uint32_t offset = PLAYER_TOWN_OFFSETS[player];
    if (offset + 8 > static_cast<uint32_t>(m_data.size())) {
        return QString();
    }

    QByteArray townData = m_data.mid(offset, 8);
    return decodeAcwwText(townData);
}

int SaveFile::getPlayerId(int player) const {
    if (!m_loaded || player < 0 || player >= PLAYER_COUNT) {
        return 0;
    }

    uint32_t offset = PLAYER_ID_OFFSETS[player];
    if (offset + 2 > static_cast<uint32_t>(m_data.size())) {
        return 0;
    }

    return static_cast<uint8_t>(m_data[offset]) |
           (static_cast<uint8_t>(m_data[offset + 1]) << 8);
}

int SaveFile::getTownId(int player) const {
    if (!m_loaded || player < 0 || player >= PLAYER_COUNT) {
        return 0;
    }

    uint32_t offset = TOWN_ID_OFFSETS[player];
    if (offset + 2 > static_cast<uint32_t>(m_data.size())) {
        return 0;
    }

    return static_cast<uint8_t>(m_data[offset]) |
           (static_cast<uint8_t>(m_data[offset + 1]) << 8);
}

int SaveFile::getPlayerGender(int player) const {
    if (!m_loaded || player < 0 || player >= PLAYER_COUNT) {
        return 0;
    }

    uint32_t offset = PLAYER_GENDER_OFFSETS[player];
    if (offset >= static_cast<uint32_t>(m_data.size())) {
        return 0;
    }

    return static_cast<uint8_t>(m_data[offset]);  // 0=male, 1=female
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
        bool empty = letter.isEmpty();
        // 0xFFF1 means "no item" in ACWW
        bool hasItem = letter.attachedItem != 0 && letter.attachedItem != 0xFFF1;
        summary["isEmpty"] = empty;
        summary["recipientName"] = letter.toPlayerName;
        summary["senderName"] = letter.fromPlayerName;
        summary["hasAttachment"] = !empty && hasItem;
        summary["stationery"] = letter.stationeryType;
        summary["iconFlags"] = letter.iconFlags;
        summaries.append(summary);
    }

    return summaries;
}

QByteArray SaveFile::getRawLetterBytes(int player, int storageType, int slot) const {
    if (!m_loaded || player < 0 || player >= PLAYER_COUNT) {
        return QByteArray();
    }

    StorageType type = static_cast<StorageType>(storageType);
    int maxSlots = getSlotCount(storageType);
    if (slot < 0 || slot >= maxSlots) {
        return QByteArray();
    }

    uint32_t offset = getLetterOffset(player, type, slot);
    if (offset + LetterFormat::SIZE > static_cast<uint32_t>(m_data.size())) {
        return QByteArray();
    }

    return m_data.mid(offset, LetterFormat::SIZE);
}

void SaveFile::setRawLetterBytes(int player, int storageType, int slot, const QByteArray& data) {
    if (!m_loaded || player < 0 || player >= PLAYER_COUNT) {
        return;
    }

    if (data.size() != LetterFormat::SIZE) {
        qDebug() << "SaveFile::setRawLetterBytes: Invalid data size:" << data.size();
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

    // Copy the raw bytes directly to the save data
    for (int i = 0; i < LetterFormat::SIZE; i++) {
        m_data[offset + i] = data[i];
    }

    // Also update the backup copy
    uint32_t backupOffset = getBackupLetterOffset(player, type, slot);
    if (backupOffset + LetterFormat::SIZE <= static_cast<uint32_t>(m_data.size())) {
        for (int i = 0; i < LetterFormat::SIZE; i++) {
            m_data[backupOffset + i] = data[i];
        }
    }

    m_modified = true;
    emit modifiedChanged();
    emit letterModified(player, storageType, slot);
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
