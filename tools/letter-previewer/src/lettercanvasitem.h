#pragma once
#include <QQuickPaintedItem>
#include <QImage>
#include <QString>
#include <QTimer>
#include <QVector>
#include <QPair>
#include "letterconstants.h"

class Backend;
class FontLoader;

class LetterCanvasItem : public QQuickPaintedItem {
    Q_OBJECT
    Q_PROPERTY(Backend* backend READ backend WRITE setBackend NOTIFY backendChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
    Q_PROPERTY(bool cursorVisible READ cursorVisible NOTIFY cursorVisibleChanged)

public:
    explicit LetterCanvasItem(QQuickItem* parent = nullptr);

    void paint(QPainter* painter) override;

    Backend* backend() const { return m_backend; }
    void setBackend(Backend* backend);

    QString text() const { return m_text; }
    void setText(const QString& text);

    int cursorPosition() const { return m_cursorPos; }
    void setCursorPosition(int pos);

    bool cursorVisible() const { return m_cursorVisible; }

    Q_INVOKABLE void insertChar(const QString& ch);
    Q_INVOKABLE void backspace();
    Q_INVOKABLE void deleteChar();
    Q_INVOKABLE void moveCursorLeft();
    Q_INVOKABLE void moveCursorRight();
    Q_INVOKABLE void newLine();
    Q_INVOKABLE void clearText();
    Q_INVOKABLE void handleClick(qreal x, qreal y);
    Q_INVOKABLE void setLetterContent(const QString& header, const QString& body, const QString& footer);

signals:
    void backendChanged();
    void textChanged();
    void cursorPositionChanged();
    void cursorVisibleChanged();

private slots:
    void toggleCursor();
    void onPaperChanged();

private:
    void renderText(QPainter* painter);
    void renderLine(QPainter* painter, const QString& text, int x, int y,
                    int cursorCol, const FontLoader& font, bool rightAlign,
                    const QColor& textColor);
    void renderLineWithRecipient(QPainter* painter, const QString& text, int x, int y,
                                  int cursorCol, const FontLoader& font,
                                  const QColor& textColor, const QColor& recipientColor,
                                  int recipientStart, int recipientEnd);

    // Text structure helpers (text = "header\nbody\nfooter")
    int getSection(int pos) const;  // 0=header, 1=body, 2=footer
    int getBodyStartPos() const;
    int getFooterStartPos() const;
    QString getHeader() const;
    QString getBody() const;
    QString getFooter() const;

    // Body wrapping - returns (lineText, startPosInBody) pairs
    QVector<QPair<QString, int>> wrapBodyText() const;

    // Click handling helper
    int findCharPosAtX(const QString& text, int targetX, int startX, const FontLoader& font) const;

    Backend* m_backend = nullptr;
    QString m_text;
    int m_cursorPos = 0;
    bool m_cursorVisible = true;
    QTimer m_cursorTimer;

    // Use shared constants from letterconstants.h
    static constexpr int HEADER_LEFT = LetterConstants::HEADER_LEFT;
    static constexpr int HEADER_TOP = LetterConstants::HEADER_TOP;
    static constexpr int BODY_LEFT = LetterConstants::BODY_LEFT;
    static constexpr int BODY_TOP = LetterConstants::BODY_TOP;
    static constexpr int BODY_LINES = LetterConstants::BODY_LINES;
    static constexpr int FOOTER_RIGHT = LetterConstants::FOOTER_RIGHT;
    static constexpr int FOOTER_TOP = LetterConstants::FOOTER_TOP;
    static constexpr int LINE_HEIGHT = LetterConstants::LINE_HEIGHT;
    static constexpr int GLYPH_SPACING = LetterConstants::GLYPH_SPACING;
    static constexpr int MAX_LINE_WIDTH = LetterConstants::MAX_LINE_WIDTH;
    static constexpr int MAX_HEADER_CHARS = LetterConstants::MAX_HEADER_CHARS;
    static constexpr int MAX_BODY_CHARS = LetterConstants::MAX_BODY_CHARS;
    static constexpr int MAX_FOOTER_CHARS = LetterConstants::MAX_FOOTER_CHARS;
};
