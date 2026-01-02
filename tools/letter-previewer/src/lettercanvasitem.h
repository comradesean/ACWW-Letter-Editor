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
    Q_PROPERTY(int selectionStart READ selectionStart NOTIFY selectionChanged)
    Q_PROPERTY(int selectionEnd READ selectionEnd NOTIFY selectionChanged)
    Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY selectionChanged)

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

    int selectionStart() const { return m_selectionStart; }
    int selectionEnd() const { return m_selectionEnd; }
    bool hasSelection() const { return m_selectionStart >= 0 && m_selectionEnd >= 0 && m_selectionStart != m_selectionEnd; }

    Q_INVOKABLE void startSelection(qreal x, qreal y);
    Q_INVOKABLE void updateSelection(qreal x, qreal y);
    Q_INVOKABLE void clearSelection();
    Q_INVOKABLE void deleteSelection();
    Q_INVOKABLE void selectAll();
    Q_INVOKABLE void extendSelectionLeft();
    Q_INVOKABLE void extendSelectionRight();
    Q_INVOKABLE void extendSelectionHome();
    Q_INVOKABLE void extendSelectionEnd();
    Q_INVOKABLE void extendSelectionUp();
    Q_INVOKABLE void extendSelectionDown();
    Q_INVOKABLE void copySelection();
    Q_INVOKABLE void cutSelection();
    Q_INVOKABLE void paste();

    Q_INVOKABLE void insertChar(const QString& ch);
    Q_INVOKABLE void backspace();
    Q_INVOKABLE void deleteChar();
    Q_INVOKABLE void moveCursorLeft();
    Q_INVOKABLE void moveCursorRight();
    Q_INVOKABLE void moveCursorUp();
    Q_INVOKABLE void moveCursorDown();
    Q_INVOKABLE void moveCursorHome();
    Q_INVOKABLE void moveCursorEnd();
    Q_INVOKABLE void newLine();
    Q_INVOKABLE void clearText();
    Q_INVOKABLE void handleClick(qreal x, qreal y);
    Q_INVOKABLE void setLetterContent(const QString& header, const QString& body, const QString& footer);
    Q_INVOKABLE int calculateHeaderWidth(const QString& newRecipientName) const;

signals:
    void backendChanged();
    void textChanged();
    void cursorPositionChanged();
    void cursorVisibleChanged();
    void selectionChanged();
    void recipientNameClicked();

private slots:
    void toggleCursor();
    void onPaperChanged();

private:
    void renderText(QPainter* painter);
    void renderLine(QPainter* painter, const QString& text, int x, int y,
                    int cursorCol, const FontLoader& font, bool rightAlign,
                    const QColor& textColor, int selStart = -1, int selEnd = -1,
                    const QColor& selectionColor = QColor());
    void renderLineWithRecipient(QPainter* painter, const QString& text, int x, int y,
                                  int cursorCol, const FontLoader& font,
                                  const QColor& textColor, const QColor& recipientColor,
                                  int recipientStart, int recipientEnd,
                                  int selStart = -1, int selEnd = -1,
                                  const QColor& selectionColor = QColor());

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

    // Selection helper - convert widget coordinates to text position
    int charPosFromPoint(qreal x, qreal y) const;

    Backend* m_backend = nullptr;
    QString m_text;
    int m_cursorPos = 0;
    bool m_cursorVisible = true;
    QTimer m_cursorTimer;
    int m_selectionStart = -1;
    int m_selectionEnd = -1;
    int m_selectionAnchor = -1;  // Fixed point of selection during drag

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
    static constexpr int NAME_TOKEN_WIDTH = LetterConstants::NAME_TOKEN_WIDTH;
};
