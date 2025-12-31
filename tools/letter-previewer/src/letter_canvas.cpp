#include "letter_canvas.h"
#include "font_loader.h"
#include <QPainter>
#include <QKeyEvent>
#include <QFocusEvent>

LetterCanvas::LetterCanvas(QWidget* parent)
    : QWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_InputMethodEnabled);

    // Setup cursor blink timer
    connect(&m_cursorTimer, &QTimer::timeout, this, &LetterCanvas::toggleCursor);
    m_cursorTimer.setInterval(500);
}

void LetterCanvas::setBackground(const QImage& background, int backgroundIndex) {
    m_background = background;
    m_backgroundIndex = backgroundIndex;
    update();
}

// TODO: Return the appropriate font color for each background
// This could be extracted from the game data or manually tuned per-background
QColor LetterCanvas::getFontColorForBackground(int index) const {
    // Pseudocode for per-background font colors:
    //
    // Option 1: Hardcoded lookup table
    // static const QColor colors[64] = {
    //     {121, 97, 32},   // Paper 00
    //     {45, 45, 120},   // Paper 01
    //     // ... define all 64 colors
    // };
    // return colors[index % 64];
    //
    // Option 2: Extract from ROM palette data
    // The stationery loader could also extract a "text color" from
    // the palette or a separate metadata file in the ROM
    //
    // Option 3: Compute based on background brightness
    // Analyze the background image and pick a contrasting color
    //
    // For now, return default brown color
    Q_UNUSED(index);
    return QColor(121, 97, 32);
}

void LetterCanvas::setFont(const FontLoader* font) {
    m_font = font;
    update();
}

void LetterCanvas::setText(const QString& text) {
    m_text = text;
    m_cursorPos = m_text.length();
    update();
}

QSize LetterCanvas::sizeHint() const {
    return QSize(CANVAS_WIDTH * SCALE, CANVAS_HEIGHT * SCALE);
}

QSize LetterCanvas::minimumSizeHint() const {
    return QSize(CANVAS_WIDTH * SCALE, CANVAS_HEIGHT * SCALE);
}

void LetterCanvas::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);

    // Fixed 2x scale, centered in widget
    int scaledW = CANVAS_WIDTH * SCALE;
    int scaledH = CANVAS_HEIGHT * SCALE;
    int offsetX = (width() - scaledW) / 2;
    int offsetY = (height() - scaledH) / 2;

    // Draw background at exact 2x scale
    if (!m_background.isNull()) {
        painter.drawImage(QRect(offsetX, offsetY, scaledW, scaledH), m_background);
    } else {
        painter.fillRect(offsetX, offsetY, scaledW, scaledH, Qt::white);
    }

    // Apply fixed 2x scaling for text rendering
    painter.save();
    painter.translate(offsetX, offsetY);
    painter.scale(SCALE, SCALE);

    renderText(painter);

    painter.restore();
}

void LetterCanvas::renderText(QPainter& painter) {
    if (!m_font) return;

    // Split text into lines by newline (max 6 lines: 1 header + 4 body + 1 footer)
    QStringList lines = m_text.split('\n');
    while (lines.size() < 6) {
        lines.append("");
    }

    // Calculate cursor position in terms of line and column
    int cursorLine = 0;
    int cursorCol = 0;
    int charCount = 0;
    for (int i = 0; i < m_text.length() && charCount < m_cursorPos; i++) {
        if (m_text[i] == '\n') {
            cursorLine++;
            cursorCol = 0;
        } else {
            cursorCol++;
        }
        charCount++;
    }

    // TODO: Get font color for current background
    // QColor fontColor = getFontColorForBackground(m_backgroundIndex);
    //
    // To apply per-background colors, either:
    // 1. Store glyphs as white/alpha and tint at render time:
    //    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    //    painter.fillRect(glyphRect, fontColor);
    //
    // 2. Or modify FontLoader to generate glyphs on-demand with a color param
    //
    // 3. Or use a QGraphicsColorizeEffect on the glyph images

    // Helper to render a single line with cursor
    auto renderLine = [&](const QString& line, int x, int y, int lineIdx) {
        int drawX = x;

        for (int col = 0; col < line.length(); col++) {
            QChar ch = line[col];
            const GlyphInfo* glyph = m_font->getGlyph(ch);

            if (glyph && !glyph->bitmap.isNull()) {
                // TODO: Apply fontColor tinting here if needed
                painter.drawImage(drawX, y, glyph->bitmap);
                drawX += glyph->displayWidth + GLYPH_SPACING;
            } else {
                drawX += m_font->charWidth(ch) + GLYPH_SPACING;
            }
        }

        // Draw cursor if on this line and focused
        if (hasFocus() && m_cursorVisible && lineIdx == cursorLine) {
            int cursorX = x;
            for (int col = 0; col < cursorCol && col < line.length(); col++) {
                cursorX += m_font->charWidth(line[col]) + GLYPH_SPACING;
            }
            painter.setPen(QPen(Qt::black, 1));
            painter.drawLine(cursorX, y + 1, cursorX, y + LINE_HEIGHT - 2);
        }
    };

    // Line 0: Header (offset 63px right, 32px down)
    renderLine(lines[0], HEADER_LEFT, HEADER_TOP, 0);

    // Lines 1-4: Body (4 lines)
    for (int i = 0; i < BODY_LINES && i + 1 < lines.size(); i++) {
        int y = BODY_TOP + i * LINE_HEIGHT;
        renderLine(lines[i + 1], BODY_LEFT, y, i + 1);
    }

    // Line 5: Footer (right-aligned)
    if (lines.size() > 5) {
        // Calculate footer width for right alignment
        int footerWidth = 0;
        for (int i = 0; i < lines[5].length(); i++) {
            footerWidth += m_font->charWidth(lines[5][i]) + GLYPH_SPACING;
        }
        int footerX = FOOTER_RIGHT - footerWidth;
        renderLine(lines[5], footerX, FOOTER_TOP, 5);
    }
}

void LetterCanvas::keyPressEvent(QKeyEvent* event) {
    bool modified = false;

    switch (event->key()) {
        case Qt::Key_Backspace:
            if (m_cursorPos > 0) {
                m_text.remove(m_cursorPos - 1, 1);
                m_cursorPos--;
                modified = true;
            }
            break;

        case Qt::Key_Delete:
            if (m_cursorPos < m_text.length()) {
                m_text.remove(m_cursorPos, 1);
                modified = true;
            }
            break;

        case Qt::Key_Left:
            if (m_cursorPos > 0) {
                m_cursorPos--;
                modified = true;
            }
            break;

        case Qt::Key_Right:
            if (m_cursorPos < m_text.length()) {
                m_cursorPos++;
                modified = true;
            }
            break;

        case Qt::Key_Home:
            m_cursorPos = 0;
            modified = true;
            break;

        case Qt::Key_End:
            m_cursorPos = m_text.length();
            modified = true;
            break;

        case Qt::Key_Return:
        case Qt::Key_Enter:
            m_text.insert(m_cursorPos, '\n');
            m_cursorPos++;
            modified = true;
            break;

        default:
            if (!event->text().isEmpty()) {
                QString insertText = event->text();
                // Filter to printable ASCII characters
                QString filtered;
                for (QChar ch : insertText) {
                    if (ch.isPrint() && ch.unicode() < 128) {
                        filtered.append(ch);
                    }
                }
                if (!filtered.isEmpty()) {
                    m_text.insert(m_cursorPos, filtered);
                    m_cursorPos += filtered.length();
                    modified = true;
                }
            }
            break;
    }

    if (modified) {
        m_cursorVisible = true;
        m_cursorTimer.start();
        update();
    }
}

void LetterCanvas::focusInEvent(QFocusEvent*) {
    m_cursorVisible = true;
    m_cursorTimer.start();
    update();
}

void LetterCanvas::focusOutEvent(QFocusEvent*) {
    m_cursorTimer.stop();
    m_cursorVisible = false;
    update();
}

void LetterCanvas::mousePressEvent(QMouseEvent*) {
    setFocus();
}

void LetterCanvas::toggleCursor() {
    m_cursorVisible = !m_cursorVisible;
    update();
}
