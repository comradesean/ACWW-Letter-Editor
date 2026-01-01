#pragma once
#include <QQuickPaintedItem>
#include <QImage>
#include <QString>
#include <QTimer>

class Backend;

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

    Backend* m_backend = nullptr;
    QString m_text;
    int m_cursorPos = 0;
    bool m_cursorVisible = true;
    QTimer m_cursorTimer;

    // Layout constants (in 1x coordinates)
    static constexpr int HEADER_LEFT = 48;
    static constexpr int HEADER_TOP = 40;
    static constexpr int BODY_LEFT = 48;
    static constexpr int BODY_TOP = 64;
    static constexpr int BODY_LINES = 4;
    static constexpr int FOOTER_RIGHT = 208;
    static constexpr int FOOTER_TOP = 136;
    static constexpr int LINE_HEIGHT = 16;
    static constexpr int GLYPH_SPACING = 1;
};
