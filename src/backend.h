#pragma once
#include <QObject>
#include <QString>
#include <QStringList>
#include <QImage>
#include <QUrl>
#include <QVariantList>
#include <QVector>
#include "letterconstants.h"
#include "stationery_loader.h"
#include "font_loader.h"
#include "cloth_loader.h"
#include "icon_loader.h"
#include "nds_rom.h"
#include "savefile.h"
#include "letter.h"
#include "itemdatabase.h"

class Backend : public QObject {
    Q_OBJECT

    // ========================================
    // ROM/Application State
    // ========================================
    Q_PROPERTY(bool loaded READ isLoaded NOTIFY loadedChanged)
    Q_PROPERTY(QStringList paperNames READ paperNames NOTIFY loadedChanged)

    // ========================================
    // Letter Structure Data (stored in .ltr/save file)
    // ========================================
    // Stationery
    Q_PROPERTY(int currentPaper READ currentPaper WRITE setCurrentPaper NOTIFY currentPaperChanged)

    // Letter content
    Q_PROPERTY(QString letterText READ letterText WRITE setLetterText NOTIFY letterTextChanged)

    // Recipient info (stored in letter structure)
    Q_PROPERTY(QString recipientName READ recipientName WRITE setRecipientName NOTIFY recipientInfoChanged)
    Q_PROPERTY(QString recipientTown READ recipientTown WRITE setRecipientTown NOTIFY recipientInfoChanged)
    Q_PROPERTY(int recipientTownId READ recipientTownId WRITE setRecipientTownId NOTIFY recipientInfoChanged)
    Q_PROPERTY(int recipientPlayerId READ recipientPlayerId WRITE setRecipientPlayerId NOTIFY recipientInfoChanged)

    // Sender info (stored in letter structure)
    Q_PROPERTY(QString senderName READ senderName WRITE setSenderName NOTIFY senderInfoChanged)
    Q_PROPERTY(QString senderTown READ senderTown WRITE setSenderTown NOTIFY senderInfoChanged)
    Q_PROPERTY(int senderTownId READ senderTownId WRITE setSenderTownId NOTIFY senderInfoChanged)
    Q_PROPERTY(int senderPlayerId READ senderPlayerId WRITE setSenderPlayerId NOTIFY senderInfoChanged)

    // Attached item
    Q_PROPERTY(int attachedItem READ attachedItem WRITE setAttachedItem NOTIFY attachedItemChanged)

    // Recipient metadata (individual bytes from 0x18-0x1B)
    Q_PROPERTY(int recipientGender READ recipientGender WRITE setRecipientGender NOTIFY letterMetadataChanged)
    Q_PROPERTY(int recipientRelation READ recipientRelation WRITE setRecipientRelation NOTIFY letterMetadataChanged)

    // Sender metadata (individual bytes from 0x30-0x33)
    Q_PROPERTY(int senderGender READ senderGender WRITE setSenderGender NOTIFY letterMetadataChanged)
    Q_PROPERTY(int senderRelation READ senderRelation WRITE setSenderRelation NOTIFY letterMetadataChanged)

    // Other letter metadata
    Q_PROPERTY(int namePosition READ namePosition WRITE setNamePosition NOTIFY letterMetadataChanged)
    Q_PROPERTY(int letterIconFlags READ letterIconFlags WRITE setLetterIconFlags NOTIFY letterMetadataChanged)
    Q_PROPERTY(int letterSource READ letterSource WRITE setLetterSource NOTIFY letterMetadataChanged)
    Q_PROPERTY(bool isGiftWrapped READ isGiftWrapped WRITE setGiftWrapped NOTIFY letterMetadataChanged)
    Q_PROPERTY(bool isLetterOpened READ isLetterOpened WRITE setLetterOpened NOTIFY letterMetadataChanged)

    // ========================================
    // GUI/Display Properties (derived, not stored)
    // ========================================
    // Parsed letter sections for display
    Q_PROPERTY(QString letterHeader READ letterHeader NOTIFY letterTextChanged)
    Q_PROPERTY(QString letterBody READ letterBody NOTIFY letterTextChanged)
    Q_PROPERTY(QString letterFooter READ letterFooter NOTIFY letterTextChanged)

    // Visual highlight positions for recipient name in header
    Q_PROPERTY(int recipientNameStart READ recipientNameStart WRITE setRecipientNameStart NOTIFY recipientNamePositionChanged)
    Q_PROPERTY(int recipientNameEnd READ recipientNameEnd WRITE setRecipientNameEnd NOTIFY recipientNamePositionChanged)

    // Derived item name for display
    Q_PROPERTY(QString attachedItemName READ attachedItemName NOTIFY attachedItemChanged)

    // Stranger detection (recipientRelation == 7)
    Q_PROPERTY(bool isRecipientStranger READ isRecipientStranger NOTIFY letterMetadataChanged)
    Q_PROPERTY(QString displayRecipientName READ displayRecipientName NOTIFY recipientDisplayNameChanged)

    // ========================================
    // Save File State
    // ========================================
    Q_PROPERTY(bool saveLoaded READ isSaveLoaded NOTIFY saveLoadedChanged)
    Q_PROPERTY(QString saveFilePath READ saveFilePath NOTIFY saveLoadedChanged)
    Q_PROPERTY(QString saveRegion READ saveRegion NOTIFY saveLoadedChanged)
    Q_PROPERTY(bool saveModified READ isSaveModified NOTIFY saveModifiedChanged)
    Q_PROPERTY(bool bankInitialized READ isBankInitialized NOTIFY saveLoadedChanged)
    Q_PROPERTY(int currentPlayer READ currentPlayer WRITE setCurrentPlayer NOTIFY currentPlayerChanged)
    Q_PROPERTY(int currentStorageType READ currentStorageType WRITE setCurrentStorageType NOTIFY currentStorageTypeChanged)
    Q_PROPERTY(int currentSlot READ currentSlot WRITE setCurrentSlot NOTIFY currentSlotChanged)
    Q_PROPERTY(QStringList playerNames READ playerNames NOTIFY saveLoadedChanged)
    Q_PROPERTY(int slotCount READ slotCount NOTIFY currentStorageTypeChanged)

public:
    explicit Backend(QObject* parent = nullptr);

    // ROM/Application State getters
    bool isLoaded() const { return m_loaded; }
    QStringList paperNames() const { return m_paperNames; }

    // Letter Structure Data getters
    int currentPaper() const { return m_currentPaper; }
    QString letterText() const { return m_letterText; }
    QString recipientName() const { return m_recipientName; }
    QString recipientTown() const { return m_recipientTown; }
    int recipientTownId() const { return m_recipientTownId; }
    int recipientPlayerId() const { return m_recipientPlayerId; }
    QString senderName() const { return m_senderName; }
    QString senderTown() const { return m_senderTown; }
    int senderTownId() const { return m_senderTownId; }
    int senderPlayerId() const { return m_senderPlayerId; }
    int attachedItem() const { return m_attachedItem; }
    int recipientGender() const { return m_recipientGender; }
    int recipientRelation() const { return m_recipientRelation; }
    int senderGender() const { return m_senderGender; }
    int senderRelation() const { return m_senderRelation; }
    int namePosition() const { return m_namePosition; }
    int letterIconFlags() const { return m_letterIconFlags; }
    int letterSource() const { return m_letterSource; }
    bool isGiftWrapped() const { return (m_letterIconFlags & 0x40) != 0; }
    bool isLetterOpened() const;

    // GUI/Display getters (derived, not stored)
    QString letterHeader() const { return m_letterHeader; }
    QString letterBody() const { return m_letterBody; }
    QString letterFooter() const { return m_letterFooter; }
    int recipientNameStart() const { return m_recipientNameStart; }
    int recipientNameEnd() const { return m_recipientNameEnd; }
    QString attachedItemName() const;
    bool isRecipientStranger() const { return m_recipientRelation == 6 || m_recipientRelation == 7; }
    QString displayRecipientName() const;

    // Save file getters
    bool isSaveLoaded() const { return m_saveFile.isLoaded(); }
    QString saveFilePath() const { return m_saveFile.filePath(); }
    QString saveRegion() const { return m_saveFile.regionName(); }
    bool isSaveModified() const { return m_saveFile.isModified(); }
    bool isBankInitialized() const { return m_saveFile.isBankInitialized(); }
    int currentPlayer() const { return m_currentPlayer; }
    int currentStorageType() const { return m_currentStorageType; }
    int currentSlot() const { return m_currentSlot; }
    QStringList playerNames() const { return m_saveFile.getPlayerNames(); }
    int slotCount() const { return m_saveFile.getSlotCount(m_currentStorageType); }

    // Save file setters
    void setCurrentPlayer(int player);
    void setCurrentStorageType(int type);
    void setCurrentSlot(int slot);

    // Letter Structure Data setters
    void setCurrentPaper(int index);
    void setLetterText(const QString& text);
    void setRecipientName(const QString& name);
    void setRecipientTown(const QString& town);
    void setRecipientTownId(int id);
    void setRecipientPlayerId(int id);
    void setSenderName(const QString& name);
    void setSenderTown(const QString& town);
    void setSenderTownId(int id);
    void setSenderPlayerId(int id);
    void setAttachedItem(int item);
    void setRecipientGender(int gender);
    void setRecipientRelation(int relation);
    void setSenderGender(int gender);
    void setSenderRelation(int relation);
    void setNamePosition(int pos);
    void setLetterIconFlags(int flags);
    void setLetterSource(int source);
    void setGiftWrapped(bool wrapped);
    void setLetterOpened(bool opened);

    // GUI/Display setters (visual state only)
    void setRecipientNameStart(int pos);
    void setRecipientNameEnd(int pos);

    Q_INVOKABLE bool loadRom(const QUrl& fileUrl);
    Q_INVOKABLE QImage getPaperImage(int index) const;
    Q_INVOKABLE QString findLocalRom() const;

    // LTR file import/export
    Q_INVOKABLE bool importLtr(const QUrl& fileUrl);
    Q_INVOKABLE bool exportLtr(const QUrl& fileUrl) const;
    Q_INVOKABLE bool exportPng(const QUrl& fileUrl, int scale = 2) const;
    Q_INVOKABLE QString getLetterHex() const;

    // Save file operations
    Q_INVOKABLE bool loadSave(const QUrl& fileUrl);
    Q_INVOKABLE bool saveSave(const QUrl& fileUrl = QUrl());
    Q_INVOKABLE void closeSave();
    Q_INVOKABLE void loadCurrentSlot();
    Q_INVOKABLE void saveCurrentSlot();
    Q_INVOKABLE QVariantList getSlotSummaries() const;
    Q_INVOKABLE void clearLetter();
    Q_INVOKABLE void importAddresseeFromSave();
    Q_INVOKABLE bool playerExists(int player) const;

    // Item database access for QML
    Q_INVOKABLE QString getItemName(int hexCode) const;
    Q_INVOKABLE QVariantList getItemList() const;
    Q_INVOKABLE QStringList getItemCategories() const;
    Q_INVOKABLE QVariantList getItemsByCategory(const QString& categoryName) const;

    // For canvas rendering
    const StationeryLoader& stationery() const { return m_stationery; }
    const FontLoader& font() const { return m_font; }
    const ClothLoader& cloth() const { return m_cloth; }
    const IconLoader& icons() const { return m_icons; }

    // Build visual glyph list for header (shared between canvas and PNG export)
    QVector<VisualGlyph> buildHeaderVisualGlyphs(const QString& templateText, int startX = LetterConstants::HEADER_LEFT, int glyphSpacing = LetterConstants::GLYPH_SPACING) const;

signals:
    // ROM/Application State
    void loadedChanged();

    // Letter Structure Data
    void currentPaperChanged();
    void letterTextChanged();
    void recipientInfoChanged();
    void senderInfoChanged();
    void attachedItemChanged();
    void letterMetadataChanged();

    // GUI/Display State
    void paperChanged();
    void recipientNamePositionChanged();
    void recipientDisplayNameChanged();

    // Warnings
    void unknownByteWarning(const QString& message);

    // Save File State
    void saveLoadedChanged();
    void saveModifiedChanged();
    void currentPlayerChanged();
    void currentStorageTypeChanged();
    void currentSlotChanged();

private:
    // ========================================
    // ROM/Application State
    // ========================================
    bool m_loaded = false;
    StationeryLoader m_stationery;
    FontLoader m_font;
    ClothLoader m_cloth;
    IconLoader m_icons;
    QStringList m_paperNames;

    // ========================================
    // Letter Structure Data (stored in .ltr/save file)
    // ========================================
    // Canonical letter data - single source of truth (244 bytes)
    QByteArray m_letterData;

    int m_currentPaper = 0;
    QString m_letterText;
    // Recipient info
    QString m_recipientName;
    QString m_recipientTown;
    int m_recipientTownId = 0;
    int m_recipientPlayerId = 0;
    // Sender info
    QString m_senderName;
    QString m_senderTown;
    int m_senderTownId = 0;
    int m_senderPlayerId = 0;
    // Attached item
    uint16_t m_attachedItem = 0xFFF1;  // 0xFFF1 = no item
    // Recipient metadata (bytes 0x18-0x1B)
    uint8_t m_recipientGender = 0;     // 0x18: 0=male, 1=female
    uint8_t m_recipientUnknown1 = 0;   // 0x19: always 0x00
    uint8_t m_recipientRelation = 0;   // 0x1A: relation type
    uint8_t m_recipientUnknown2 = 0;   // 0x1B: always 0x00
    // Sender metadata (bytes 0x30-0x33)
    uint8_t m_senderGender = 0;        // 0x30: 0=male, 1=female
    uint8_t m_senderUnknown1 = 0;      // 0x31: always 0x00
    uint8_t m_senderRelation = 0;      // 0x32: relation type
    uint8_t m_senderUnknown2 = 0;      // 0x33: always 0x00
    // Other letter metadata
    uint8_t m_namePosition = 0;        // Raw intro index for greeting
    uint8_t m_letterIconFlags = 0;     // Letter icon/flags
    uint8_t m_letterSource = 0;        // Letter source type

    // ========================================
    // GUI/Display State (derived, not stored)
    // ========================================
    QString m_letterHeader;            // Parsed from m_letterText
    QString m_letterBody;              // Parsed from m_letterText
    QString m_letterFooter;            // Parsed from m_letterText
    int m_recipientNameStart = -1;     // Visual highlight position in header
    int m_recipientNameEnd = -1;       // Visual highlight position in header

    // ========================================
    // Save File State
    // ========================================
    SaveFile m_saveFile;
    int m_currentPlayer = 0;
    int m_currentStorageType = 0;      // 0=Inventory, 1=Mailbox, 2=Bank
    int m_currentSlot = 0;

    // ========================================
    // Letter Data Helpers
    // ========================================
    void initLetterData();  // Initialize m_letterData to empty 244-byte buffer
    void syncLetterDataFromUI();  // Sync all UI state to m_letterData
    void syncUIFromLetterData();  // Sync all m_letterData to UI state

    // Field-specific writers to m_letterData
    void writeGreetingToData(const QString& greeting);
    void writeBodyToData(const QString& body);
    void writeSignatureToData(const QString& signature);
    void writeRecipientToData();
    void writeSenderToData();
    void writeMetadataToData();
};
