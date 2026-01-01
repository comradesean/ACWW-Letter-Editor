#include "lettercanvasitem.h"
#include "backend.h"
#include <QPainter>

LetterCanvasItem::LetterCanvasItem(QQuickItem* parent)
    : QQuickPaintedItem(parent)
{
    setFlag(ItemAcceptsInputMethod, true);
    setAcceptedMouseButtons(Qt::LeftButton);

    connect(&m_cursorTimer, &QTimer::timeout, this, &LetterCanvasItem::toggleCursor);
    m_cursorTimer.setInterval(500);
    m_cursorTimer.start();
}

void LetterCanvasItem::setBackend(Backend* backend) {
    if (m_backend != backend) {
        if (m_backend) {
            disconnect(m_backend, nullptr, this, nullptr);
        }
        m_backend = backend;
        if (m_backend) {
            connect(m_backend, &Backend::paperChanged, this, &LetterCanvasItem::onPaperChanged);
        }
        emit backendChanged();
        update();
    }
}

void LetterCanvasItem::setText(const QString& text) {
    if (m_text != text) {
        m_text = text;
        m_cursorPos = qMin(m_cursorPos, m_text.length());
        emit textChanged();
        update();
    }
}

void LetterCanvasItem::setCursorPosition(int pos) {
    pos = qBound(0, pos, m_text.length());
    if (m_cursorPos != pos) {
        m_cursorPos = pos;
        emit cursorPositionChanged();
        update();
    }
}

void LetterCanvasItem::insertChar(const QString& ch) {
    if (!ch.isEmpty() && ch[0].isPrint() && ch[0].unicode() < 128) {
        m_text.insert(m_cursorPos, ch);
        m_cursorPos++;
        m_cursorVisible = true;
        emit textChanged();
        emit cursorPositionChanged();
        update();
    }
}

void LetterCanvasItem::backspace() {
    if (m_cursorPos > 0) {
        m_text.remove(m_cursorPos - 1, 1);
        m_cursorPos--;
        m_cursorVisible = true;
        emit textChanged();
        emit cursorPositionChanged();
        update();
    }
}

void LetterCanvasItem::deleteChar() {
    if (m_cursorPos < m_text.length()) {
        m_text.remove(m_cursorPos, 1);
        m_cursorVisible = true;
        emit textChanged();
        update();
    }
}

void LetterCanvasItem::moveCursorLeft() {
    if (m_cursorPos > 0) {
        m_cursorPos--;
        m_cursorVisible = true;
        emit cursorPositionChanged();
        update();
    }
}

void LetterCanvasItem::moveCursorRight() {
    if (m_cursorPos < m_text.length()) {
        m_cursorPos++;
        m_cursorVisible = true;
        emit cursorPositionChanged();
        update();
    }
}

void LetterCanvasItem::newLine() {
    m_text.insert(m_cursorPos, '\n');
    m_cursorPos++;
    m_cursorVisible = true;
    emit textChanged();
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::clearText() {
    m_text.clear();
    m_cursorPos = 0;
    m_cursorVisible = true;
    emit textChanged();
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::toggleCursor() {
    m_cursorVisible = !m_cursorVisible;
    emit cursorVisibleChanged();
    update();
}

void LetterCanvasItem::onPaperChanged() {
    update();
}

void LetterCanvasItem::paint(QPainter* painter) {
    painter->setRenderHint(QPainter::SmoothPixmapTransform, false);

    // Calculate scale
    qreal scaleX = width() / 256.0;
    qreal scaleY = height() / 192.0;
    qreal scale = qMin(scaleX, scaleY);

    int scaledW = static_cast<int>(256 * scale);
    int scaledH = static_cast<int>(192 * scale);
    int offsetX = (width() - scaledW) / 2;
    int offsetY = (height() - scaledH) / 2;

    // Draw background
    if (m_backend && m_backend->isLoaded()) {
        QImage bg = m_backend->getPaperImage(m_backend->currentPaper());
        if (!bg.isNull()) {
            painter->drawImage(QRect(offsetX, offsetY, scaledW, scaledH), bg);
        }
    } else {
        painter->fillRect(offsetX, offsetY, scaledW, scaledH, Qt::white);
    }

    // Scale for text
    painter->save();
    painter->translate(offsetX, offsetY);
    painter->scale(scale, scale);

    renderText(painter);

    painter->restore();
}

void LetterCanvasItem::renderText(QPainter* painter) {
    if (!m_backend || !m_backend->isLoaded()) return;

    const FontLoader& font = m_backend->font();

    // Split text into lines
    QStringList lines = m_text.split('\n');
    while (lines.size() < 6) {
        lines.append("");
    }

    // Calculate cursor line and column
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

    // Render line helper
    auto renderLine = [&](const QString& line, int x, int y, int lineIdx, bool rightAlign = false) {
        int drawX = x;

        // Calculate width for right alignment
        if (rightAlign) {
            int totalWidth = 0;
            for (int i = 0; i < line.length(); i++) {
                totalWidth += font.charWidth(line[i]) + GLYPH_SPACING;
            }
            drawX = x - totalWidth;
        }

        int startX = drawX;

        for (int col = 0; col < line.length(); col++) {
            QChar ch = line[col];
            const GlyphInfo* glyph = font.getGlyph(ch);

            if (glyph && !glyph->bitmap.isNull()) {
                painter->drawImage(drawX, y, glyph->bitmap);
                drawX += glyph->displayWidth + GLYPH_SPACING;
            } else {
                drawX += font.charWidth(ch) + GLYPH_SPACING;
            }
        }

        // Draw cursor
        if (m_cursorVisible && lineIdx == cursorLine) {
            int cursorX = startX;
            for (int col = 0; col < cursorCol && col < line.length(); col++) {
                cursorX += font.charWidth(line[col]) + GLYPH_SPACING;
            }
            painter->setPen(QPen(QColor(121, 97, 32), 1));
            painter->drawLine(cursorX, y + 1, cursorX, y + LINE_HEIGHT - 2);
        }
    };

    // Header
    renderLine(lines[0], HEADER_LEFT, HEADER_TOP, 0);

    // Body (4 lines)
    for (int i = 0; i < BODY_LINES && i + 1 < lines.size(); i++) {
        renderLine(lines[i + 1], BODY_LEFT, BODY_TOP + i * LINE_HEIGHT, i + 1);
    }

    // Footer (right-aligned)
    if (lines.size() > 5) {
        renderLine(lines[5], FOOTER_RIGHT, FOOTER_TOP, 5, true);
    }
}
