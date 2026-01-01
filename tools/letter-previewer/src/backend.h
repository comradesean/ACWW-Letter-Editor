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
    Q_PROPERTY(QString recipientName READ recipientName NOTIFY letterTextChanged)
    Q_PROPERTY(QString recipientTown READ recipientTown NOTIFY letterTextChanged)
    Q_PROPERTY(QString senderName READ senderName NOTIFY letterTextChanged)
    Q_PROPERTY(QString senderTown READ senderTown NOTIFY letterTextChanged)
    Q_PROPERTY(QString letterHeader READ letterHeader NOTIFY letterTextChanged)
    Q_PROPERTY(QString letterBody READ letterBody NOTIFY letterTextChanged)
    Q_PROPERTY(QString letterFooter READ letterFooter NOTIFY letterTextChanged)
    Q_PROPERTY(int recipientNameStart READ recipientNameStart NOTIFY letterTextChanged)
    Q_PROPERTY(int recipientNameEnd READ recipientNameEnd NOTIFY letterTextChanged)

public:
    explicit Backend(QObject* parent = nullptr);

    bool isLoaded() const { return m_loaded; }
    QStringList paperNames() const { return m_paperNames; }
    int currentPaper() const { return m_currentPaper; }
    QString letterText() const { return m_letterText; }
    QString recipientName() const { return m_recipientName; }
    QString recipientTown() const { return m_recipientTown; }
    QString senderName() const { return m_senderName; }
    QString senderTown() const { return m_senderTown; }
    QString letterHeader() const { return m_letterHeader; }
    QString letterBody() const { return m_letterBody; }
    QString letterFooter() const { return m_letterFooter; }
    int recipientNameStart() const { return m_recipientNameStart; }
    int recipientNameEnd() const { return m_recipientNameEnd; }

    void setCurrentPaper(int index);
    void setLetterText(const QString& text);

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

private:
    bool m_loaded = false;
    StationeryLoader m_stationery;
    FontLoader m_font;
    QStringList m_paperNames;
    int m_currentPaper = 0;
    QString m_letterText;
    QString m_recipientName;
    QString m_recipientTown;
    QString m_senderName;
    QString m_senderTown;
    QString m_letterHeader;
    QString m_letterBody;
    QString m_letterFooter;
    int m_recipientNameStart = -1;  // Position in header where recipient name starts
    int m_recipientNameEnd = -1;    // Position in header where recipient name ends
};
