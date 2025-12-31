#pragma once
#include <QWidget>
#include <QImage>
#include <QString>
#include <QTimer>

class FontLoader;

class LetterCanvas : public QWidget {
    Q_OBJECT

public:
    explicit LetterCanvas(QWidget* parent = nullptr);

    void setBackground(const QImage& background, int backgroundIndex = 0);
    void setFont(const FontLoader* font);
    void setText(const QString& text);
    QString text() const { return m_text; }

    // TODO: Each background has its own font color
    // Background index -> RGB color mapping
    // Could be loaded from a config file or hardcoded table
    // static const QColor BACKGROUND_FONT_COLORS[64] = {
    //     QColor(121, 97, 32),   // Paper 00 - brown
    //     QColor(45, 45, 120),   // Paper 01 - dark blue
    //     QColor(80, 40, 40),    // Paper 02 - dark red
    //     // ... etc for all 64 backgrounds
    // };
    QColor getFontColorForBackground(int index) const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private slots:
    void toggleCursor();

private:
    void renderText(QPainter& painter);

    QImage m_background;
    int m_backgroundIndex = 0;  // Current background index for font color lookup
    const FontLoader* m_font = nullptr;
    QString m_text;

    // Text editing
    int m_cursorPos = 0;
    bool m_cursorVisible = true;
    QTimer m_cursorTimer;

    // Canvas dimensions
    static constexpr int CANVAS_WIDTH = 256;
    static constexpr int CANVAS_HEIGHT = 192;
    static constexpr int SCALE = 2;

    // Letter layout (in original 256x192 coordinates, multiply by 2 for screen pixels)
    // Header: 1 line
    static constexpr int HEADER_LEFT = 48;
    static constexpr int HEADER_TOP = 40;

    // Body: 4 lines
    static constexpr int BODY_LEFT = 48;
    static constexpr int BODY_TOP = 64;
    static constexpr int BODY_LINES = 4;

    // Footer: 1 line (signature, right-aligned)
    static constexpr int FOOTER_RIGHT = 208;  // Right edge for alignment
    static constexpr int FOOTER_TOP = 136;

    // Text rendering
    static constexpr int LINE_HEIGHT = 16;
    static constexpr int GLYPH_SPACING = 1;  // 1px between glyphs
};
