#pragma once
#include <QObject>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QVariantList>
#include "letter.h"

class SaveFile : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool loaded READ isLoaded NOTIFY loadedChanged)
    Q_PROPERTY(QString region READ regionName NOTIFY loadedChanged)
    Q_PROPERTY(QString filePath READ filePath NOTIFY loadedChanged)
    Q_PROPERTY(bool modified READ isModified NOTIFY modifiedChanged)

public:
    enum class Region {
        Unknown = 0,
        EUR_USA,    // Europe and USA
        JPN,        // Japan
        KOR         // Korea
    };
    Q_ENUM(Region)

    enum class StorageType {
        Inventory = 0,
        Mailbox = 1,
        Bank = 2
    };
    Q_ENUM(StorageType)

    // Slot counts per storage type
    static constexpr int INVENTORY_SLOTS = 10;
    static constexpr int MAILBOX_SLOTS = 10;
    static constexpr int BANK_SLOTS = 75;
    static constexpr int PLAYER_COUNT = 4;

    // Save file sizes
    static constexpr int SAVE_SIZE_256K = 0x40000;   // 256KB
    static constexpr int SAVE_SIZE_512K = 0x80000;   // 512KB (some dumps)

    explicit SaveFile(QObject* parent = nullptr);

    // File operations
    Q_INVOKABLE bool load(const QString& path);
    Q_INVOKABLE bool save(const QString& path = QString());
    Q_INVOKABLE void close();

    // Properties
    bool isLoaded() const { return m_loaded; }
    Region region() const { return m_region; }
    QString regionName() const;
    QString filePath() const { return m_filePath; }
    bool isModified() const { return m_modified; }

    // Player info
    Q_INVOKABLE QString getPlayerName(int player) const;
    Q_INVOKABLE QString getPlayerTown(int player) const;
    Q_INVOKABLE int getPlayerId(int player) const;
    Q_INVOKABLE int getTownId(int player) const;
    Q_INVOKABLE bool playerExists(int player) const;
    Q_INVOKABLE QStringList getPlayerNames() const;

    // Letter access
    Q_INVOKABLE Letter getLetter(int player, int storageType, int slot) const;
    Q_INVOKABLE void setLetter(int player, int storageType, int slot, const Letter& letter);
    Q_INVOKABLE QByteArray getRawLetterBytes(int player, int storageType, int slot) const;

    // Slot counts
    Q_INVOKABLE int getSlotCount(int storageType) const;

    // Letter summaries for UI (returns list of {isEmpty, recipientName, hasAttachment})
    Q_INVOKABLE QVariantList getSlotSummaries(int player, int storageType) const;

    // Checksum operations
    Q_INVOKABLE bool validateChecksum() const;
    Q_INVOKABLE void fixChecksum();

    // Region-specific constants
    struct RegionData {
        uint32_t checksumOffset;
        uint32_t saveCopyOffset;    // Where backup copy starts
        uint32_t saveCopySize;      // Size of data to copy/checksum
        uint32_t playerDataOffset;  // Start of player data
        uint32_t playerDataSize;    // Size of each player's data
    };
    static const RegionData& getRegionData(Region region);

signals:
    void loadedChanged();
    void modifiedChanged();
    void letterModified(int player, int storageType, int slot);

private:
    // Region detection
    Region detectRegion() const;

    // Letter offset calculation
    uint32_t getLetterOffset(int player, StorageType type, int slot) const;
    uint32_t getBackupLetterOffset(int player, StorageType type, int slot) const;

    // Checksum calculation
    uint16_t calculateChecksum() const;
    void copyToBackup();

    // Player data offsets
    uint32_t getPlayerNameOffset(int player) const;
    uint32_t getPlayerTownOffset(int player) const;
    uint32_t getPlayerIdOffset(int player) const;
    uint32_t getTownIdOffset(int player) const;

    QByteArray m_data;
    Region m_region = Region::Unknown;
    QString m_filePath;
    bool m_loaded = false;
    bool m_modified = false;
};
