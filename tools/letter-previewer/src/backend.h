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

public:
    explicit Backend(QObject* parent = nullptr);

    bool isLoaded() const { return m_loaded; }
    QStringList paperNames() const { return m_paperNames; }
    int currentPaper() const { return m_currentPaper; }
    QString letterText() const { return m_letterText; }

    void setCurrentPaper(int index);
    void setLetterText(const QString& text);

    Q_INVOKABLE bool loadRom(const QUrl& fileUrl);
    Q_INVOKABLE QImage getPaperImage(int index) const;

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
};
