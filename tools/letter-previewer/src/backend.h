#pragma once
#include <QObject>
#include <QString>
#include <QStringList>
#include <QImage>
#include <QUrl>
#include "stationery_loader.h"
#include "font_loader.h"
#include "nds_rom.h"

class Backend : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool loaded READ isLoaded NOTIFY loadedChanged)
    Q_PROPERTY(QStringList paperNames READ paperNames NOTIFY loadedChanged)
    Q_PROPERTY(int currentPaper READ currentPaper WRITE setCurrentPaper NOTIFY currentPaperChanged)
    Q_PROPERTY(QString letterText READ letterText WRITE setLetterText NOTIFY letterTextChanged)
    Q_PROPERTY(QString recipientName READ recipientName WRITE setRecipientName NOTIFY recipientInfoChanged)
    Q_PROPERTY(QString recipientTown READ recipientTown WRITE setRecipientTown NOTIFY recipientInfoChanged)
    Q_PROPERTY(int recipientTownId READ recipientTownId WRITE setRecipientTownId NOTIFY recipientInfoChanged)
    Q_PROPERTY(int recipientPlayerId READ recipientPlayerId WRITE setRecipientPlayerId NOTIFY recipientInfoChanged)
    Q_PROPERTY(QString senderName READ senderName WRITE setSenderName NOTIFY senderInfoChanged)
    Q_PROPERTY(QString senderTown READ senderTown WRITE setSenderTown NOTIFY senderInfoChanged)
    Q_PROPERTY(int senderTownId READ senderTownId WRITE setSenderTownId NOTIFY senderInfoChanged)
    Q_PROPERTY(int senderPlayerId READ senderPlayerId WRITE setSenderPlayerId NOTIFY senderInfoChanged)
    Q_PROPERTY(QString letterHeader READ letterHeader NOTIFY letterTextChanged)
    Q_PROPERTY(QString letterBody READ letterBody NOTIFY letterTextChanged)
    Q_PROPERTY(QString letterFooter READ letterFooter NOTIFY letterTextChanged)
    Q_PROPERTY(int recipientNameStart READ recipientNameStart WRITE setRecipientNameStart NOTIFY recipientNamePositionChanged)
    Q_PROPERTY(int recipientNameEnd READ recipientNameEnd WRITE setRecipientNameEnd NOTIFY recipientNamePositionChanged)

public:
    explicit Backend(QObject* parent = nullptr);

    bool isLoaded() const { return m_loaded; }
    QStringList paperNames() const { return m_paperNames; }
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
    QString letterHeader() const { return m_letterHeader; }
    QString letterBody() const { return m_letterBody; }
    QString letterFooter() const { return m_letterFooter; }
    int recipientNameStart() const { return m_recipientNameStart; }
    int recipientNameEnd() const { return m_recipientNameEnd; }

    void setCurrentPaper(int index);
    void setLetterText(const QString& text);
    void setRecipientNameStart(int pos);
    void setRecipientNameEnd(int pos);
    void setRecipientName(const QString& name);
    void setRecipientTown(const QString& town);
    void setRecipientTownId(int id);
    void setRecipientPlayerId(int id);
    void setSenderName(const QString& name);
    void setSenderTown(const QString& town);
    void setSenderTownId(int id);
    void setSenderPlayerId(int id);

    Q_INVOKABLE bool loadRom(const QUrl& fileUrl);
    Q_INVOKABLE QImage getPaperImage(int index) const;
    Q_INVOKABLE QString findLocalRom() const;

    // LTR file import/export
    Q_INVOKABLE bool importLtr(const QUrl& fileUrl);
    Q_INVOKABLE bool exportLtr(const QUrl& fileUrl) const;
    Q_INVOKABLE bool exportPng(const QUrl& fileUrl, int scale = 2) const;

    // For canvas rendering
    const StationeryLoader& stationery() const { return m_stationery; }
    const FontLoader& font() const { return m_font; }

signals:
    void loadedChanged();
    void currentPaperChanged();
    void letterTextChanged();
    void paperChanged();
    void recipientNamePositionChanged();
    void recipientInfoChanged();
    void senderInfoChanged();

private:
    bool m_loaded = false;
    StationeryLoader m_stationery;
    FontLoader m_font;
    QStringList m_paperNames;
    int m_currentPaper = 0;
    QString m_letterText;
    QString m_recipientName;
    QString m_recipientTown;
    int m_recipientTownId = 0;
    int m_recipientPlayerId = 0;
    QString m_senderName;
    QString m_senderTown;
    int m_senderTownId = 0;
    int m_senderPlayerId = 0;
    QString m_letterHeader;
    QString m_letterBody;
    QString m_letterFooter;
    int m_recipientNameStart = -1;  // Position in header where recipient name starts
    int m_recipientNameEnd = -1;    // Position in header where recipient name ends
};
