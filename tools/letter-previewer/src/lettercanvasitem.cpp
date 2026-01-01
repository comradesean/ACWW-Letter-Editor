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

// Returns 0 for header, 1 for body, 2 for footer
int LetterCanvasItem::getSection(int pos) const {
    int firstNewline = m_text.indexOf('\n');
    if (firstNewline < 0) return 0;  // No newlines, all header
    if (pos <= firstNewline) return 0;  // Header

    int lastNewline = m_text.lastIndexOf('\n');
    if (lastNewline == firstNewline) return 1;  // Only one newline, rest is body
    if (pos <= lastNewline) return 1;  // Body (including body newlines)

    return 2;  // Footer
}

int LetterCanvasItem::getBodyStartPos() const {
    int firstNewline = m_text.indexOf('\n');
    return (firstNewline >= 0) ? firstNewline + 1 : m_text.length();
}

int LetterCanvasItem::getFooterStartPos() const {
    int lastNewline = m_text.lastIndexOf('\n');
    return (lastNewline >= 0) ? lastNewline + 1 : m_text.length();
}

QString LetterCanvasItem::getHeader() const {
    int firstNewline = m_text.indexOf('\n');
    if (firstNewline < 0) return m_text;
    return m_text.left(firstNewline);
}

QString LetterCanvasItem::getBody() const {
    int firstNewline = m_text.indexOf('\n');
    if (firstNewline < 0) return "";
    int lastNewline = m_text.lastIndexOf('\n');
    if (lastNewline == firstNewline) return m_text.mid(firstNewline + 1);
    return m_text.mid(firstNewline + 1, lastNewline - firstNewline - 1);
}

QString LetterCanvasItem::getFooter() const {
    int lastNewline = m_text.lastIndexOf('\n');
    if (lastNewline < 0) return "";
    return m_text.mid(lastNewline + 1);
}

// Wrap body text into visual lines based on glyph width (word-wrap)
// Returns list of (text, startPosInBody) pairs for each visual line
QVector<QPair<QString, int>> LetterCanvasItem::wrapBodyText() const {
    QVector<QPair<QString, int>> visualLines;
    if (!m_backend || !m_backend->isLoaded()) {
        for (int i = 0; i < BODY_LINES; i++) {
            visualLines.append(qMakePair(QString(""), 0));
        }
        return visualLines;
    }

    QString body = getBody();
    const FontLoader& font = m_backend->font();

    int lineIdx = 0;
    int pos = 0;

    while (lineIdx < BODY_LINES && pos < body.length()) {
        QString lineText;
        int lineStartPos = pos;
        int lineWidth = 0;
        int lastSpacePos = -1;      // Position after last space in lineText
        int lastSpaceBodyPos = -1;  // Corresponding position in body
        int lastSpaceWidth = 0;     // Width up to and including last space

        while (pos < body.length()) {
            QChar ch = body[pos];

            // Explicit newline - end this line and move to next
            if (ch == '\n') {
                pos++;  // Skip the newline
                break;
            }

            int charWidth = font.charWidth(ch) + GLYPH_SPACING;

            // Check if this char would exceed line width
            if (lineWidth + charWidth > MAX_LINE_WIDTH && !lineText.isEmpty()) {
                // Line is full - try to wrap at last space
                if (lastSpacePos > 0) {
                    // Wrap at last space - trim trailing space
                    lineText = lineText.left(lastSpacePos - 1);
                    pos = lastSpaceBodyPos;
                }
                // else: no space found, hard break at current position
                break;
            }

            lineText += ch;
            lineWidth += charWidth;
            pos++;

            // Track last space position for word wrapping
            if (ch == ' ') {
                lastSpacePos = lineText.length();
                lastSpaceBodyPos = pos;
                lastSpaceWidth = lineWidth;
            }
        }

        visualLines.append(qMakePair(lineText, lineStartPos));
        lineIdx++;
    }

    // Fill remaining lines with empty
    while (visualLines.size() < BODY_LINES) {
        visualLines.append(qMakePair(QString(""), pos));
    }

    return visualLines;
}

void LetterCanvasItem::insertChar(const QString& ch) {
    if (!ch.isEmpty() && ch[0].isPrint() && ch[0].unicode() < 128) {
        if (!m_backend || !m_backend->isLoaded()) return;

        int section = getSection(m_cursorPos);
        int insertPos = m_cursorPos;  // Save position before insertion

        // Check section character limits
        if (section == 0) {
            int recipientStart = m_backend->recipientNameStart();
            int recipientEnd = m_backend->recipientNameEnd();
            int recipientLen = (recipientStart >= 0 && recipientEnd >= 0) ? (recipientEnd - recipientStart) : 0;

            // Check char limit excluding recipient name (24 chars total, name doesn't count)
            int headerLenWithoutName = getHeader().length() - recipientLen;
            if (headerLenWithoutName >= MAX_HEADER_CHARS) return;

            // Check pixel width limit (name token = fixed 54px)
            QString testHeader = getHeader();
            int headerOffset = m_cursorPos;
            testHeader.insert(headerOffset, ch);
            int testWidth = 0;
            const FontLoader& font = m_backend->font();
            for (int i = 0; i < testHeader.length(); i++) {
                // Skip recipient name chars - use fixed token width instead
                if (recipientStart >= 0 && recipientEnd >= 0 &&
                    i >= recipientStart && i < recipientEnd + 1) {  // +1 for inserted char shift
                    if (i == recipientStart) testWidth += NAME_TOKEN_WIDTH;
                    continue;
                }
                testWidth += font.charWidth(testHeader[i]) + GLYPH_SPACING;
            }
            if (testWidth > MAX_LINE_WIDTH) return;

            // Protect recipient name - don't allow inserting within it
            if (recipientStart >= 0 && recipientEnd >= 0 &&
                m_cursorPos > recipientStart && m_cursorPos < recipientEnd) {
                return;
            }
        } else if (section == 1) {
            if (getBody().length() >= MAX_BODY_CHARS) return;
            // Check if body would overflow 4 visual lines
            QString testBody = getBody();
            int bodyOffset = m_cursorPos - getBodyStartPos();
            testBody.insert(bodyOffset, ch);
            // Temporarily check if this would fit
            QString origBody = getBody();
            // We'll check after insertion if it overflows
        } else {
            if (getFooter().length() >= MAX_FOOTER_CHARS) return;

            // Check pixel width limit
            QString testFooter = getFooter();
            int footerOffset = m_cursorPos - getFooterStartPos();
            testFooter.insert(footerOffset, ch);
            int testWidth = 0;
            const FontLoader& font = m_backend->font();
            for (const QChar& c : testFooter) {
                testWidth += font.charWidth(c) + GLYPH_SPACING;
            }
            if (testWidth > LetterConstants::MAX_FOOTER_WIDTH) return;
        }

        m_text.insert(m_cursorPos, ch);
        m_cursorPos++;

        // For body, check if we overflowed visual lines
        if (section == 1) {
            auto lines = wrapBodyText();
            QString body = getBody();
            int lastLineEnd = lines[BODY_LINES - 1].second + lines[BODY_LINES - 1].first.length();
            if (lastLineEnd < body.length() && body[lastLineEnd] != '\n') {
                // Text overflowed beyond 4 lines - revert
                m_cursorPos--;
                m_text.remove(m_cursorPos, 1);
                return;
            }
        }

        // For header, shift recipient name position if we inserted before it
        if (section == 0) {
            int recipientStart = m_backend->recipientNameStart();
            int recipientEnd = m_backend->recipientNameEnd();
            if (recipientStart >= 0 && recipientEnd >= 0 && insertPos <= recipientStart) {
                m_backend->setRecipientNameStart(recipientStart + 1);
                m_backend->setRecipientNameEnd(recipientEnd + 1);
            }
        }

        m_cursorVisible = true;
        emit textChanged();
        emit cursorPositionChanged();
        update();
    }
}

void LetterCanvasItem::backspace() {
    if (m_cursorPos > 0) {
        int section = getSection(m_cursorPos);
        int prevSection = getSection(m_cursorPos - 1);

        // Don't allow backspace across section boundaries (deleting the section-separating newline)
        if (section != prevSection) {
            return;
        }

        // In header section, protect the recipient name
        if (section == 0 && m_backend) {
            int recipientStart = m_backend->recipientNameStart();
            int recipientEnd = m_backend->recipientNameEnd();
            int deletePos = m_cursorPos - 1;

            // Don't allow deleting characters within the recipient name
            if (recipientStart >= 0 && recipientEnd >= 0 &&
                deletePos >= recipientStart && deletePos < recipientEnd) {
                return;
            }

            // If deleting before the recipient name, shift the name positions
            if (recipientStart >= 0 && recipientEnd >= 0 && deletePos < recipientStart) {
                m_backend->setRecipientNameStart(recipientStart - 1);
                m_backend->setRecipientNameEnd(recipientEnd - 1);
            }
        }

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
        int section = getSection(m_cursorPos);
        int nextSection = getSection(m_cursorPos + 1);

        // Don't allow delete across section boundaries
        if (section != nextSection && m_text[m_cursorPos] == '\n') {
            return;
        }

        // In header section, protect the recipient name
        if (section == 0 && m_backend) {
            int recipientStart = m_backend->recipientNameStart();
            int recipientEnd = m_backend->recipientNameEnd();
            int deletePos = m_cursorPos;

            // Don't allow deleting characters within the recipient name
            if (recipientStart >= 0 && recipientEnd >= 0 &&
                deletePos >= recipientStart && deletePos < recipientEnd) {
                return;
            }

            // If deleting before the recipient name, shift the name positions
            if (recipientStart >= 0 && recipientEnd >= 0 && deletePos < recipientStart) {
                m_backend->setRecipientNameStart(recipientStart - 1);
                m_backend->setRecipientNameEnd(recipientEnd - 1);
            }
        }

        m_text.remove(m_cursorPos, 1);
        m_cursorVisible = true;
        emit textChanged();
        update();
    }
}

void LetterCanvasItem::moveCursorLeft() {
    if (m_cursorPos > 0) {
        m_cursorPos--;

        // Skip over recipient name in header - jump to start of name
        if (m_backend && getSection(m_cursorPos) == 0) {
            int recipientStart = m_backend->recipientNameStart();
            int recipientEnd = m_backend->recipientNameEnd();
            if (recipientStart >= 0 && recipientEnd >= 0 &&
                m_cursorPos > recipientStart && m_cursorPos < recipientEnd) {
                m_cursorPos = recipientStart;
            }
        }

        m_cursorVisible = true;
        emit cursorPositionChanged();
        update();
    }
}

void LetterCanvasItem::moveCursorRight() {
    if (m_cursorPos < m_text.length()) {
        m_cursorPos++;

        // Skip over recipient name in header - jump to end of name
        if (m_backend && getSection(m_cursorPos) == 0) {
            int recipientStart = m_backend->recipientNameStart();
            int recipientEnd = m_backend->recipientNameEnd();
            if (recipientStart >= 0 && recipientEnd >= 0 &&
                m_cursorPos > recipientStart && m_cursorPos < recipientEnd) {
                m_cursorPos = recipientEnd;
            }
        }

        m_cursorVisible = true;
        emit cursorPositionChanged();
        update();
    }
}

void LetterCanvasItem::newLine() {
    int section = getSection(m_cursorPos);

    // Only allow newline within body section
    if (section != 1) {
        return;
    }

    // Check if adding a newline would overflow
    QString body = getBody();
    if (body.count('\n') >= 3) {
        return;  // Already have 3 explicit newlines in body (4 logical lines max)
    }

    m_text.insert(m_cursorPos, '\n');
    m_cursorPos++;
    m_cursorVisible = true;
    emit textChanged();
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::clearText() {
    setLetterContent("", "", "");
}

void LetterCanvasItem::setLetterContent(const QString& header, const QString& body, const QString& footer) {
    // Truncate to limits
    QString h = header.left(MAX_HEADER_CHARS);
    QString b = body.left(MAX_BODY_CHARS);
    QString f = footer.left(MAX_FOOTER_CHARS);

    // Build text in format: header\nbody\nfooter
    m_text = h + "\n" + b + "\n" + f;
    m_cursorPos = 0;
    m_cursorVisible = true;
    emit textChanged();
    emit cursorPositionChanged();
    update();
}

int LetterCanvasItem::calculateHeaderWidth(const QString& newRecipientName) const {
    if (!m_backend || !m_backend->isLoaded()) return 0;

    QString header = getHeader();
    int recipientStart = m_backend->recipientNameStart();
    int recipientEnd = m_backend->recipientNameEnd();

    // Calculate width of non-name characters + fixed 54px for name token
    int totalWidth = 0;
    const FontLoader& font = m_backend->font();
    bool hasName = (recipientStart >= 0 && recipientEnd >= 0 && recipientEnd <= header.length());

    for (int i = 0; i < header.length(); i++) {
        // Skip recipient name chars - use fixed token width instead
        if (hasName && i >= recipientStart && i < recipientEnd) {
            if (i == recipientStart) totalWidth += NAME_TOKEN_WIDTH;
            continue;
        }
        totalWidth += font.charWidth(header[i]) + GLYPH_SPACING;
    }

    return totalWidth;
}

void LetterCanvasItem::handleClick(qreal x, qreal y) {
    if (!m_backend || !m_backend->isLoaded()) return;

    // Ensure text has proper structure
    if (m_text.count('\n') < 2) {
        m_text = "\n\n";
        emit textChanged();
    }

    // Convert from widget coordinates to 1x coordinates
    qreal scaleX = width() / 256.0;
    qreal scaleY = height() / 192.0;
    qreal scale = qMin(scaleX, scaleY);

    int scaledW = static_cast<int>(256 * scale);
    int scaledH = static_cast<int>(192 * scale);
    int offsetX = (width() - scaledW) / 2;
    int offsetY = (height() - scaledH) / 2;

    // Convert to 1x coordinates
    int localX = static_cast<int>((x - offsetX) / scale);
    int localY = static_cast<int>((y - offsetY) / scale);

    const FontLoader& font = m_backend->font();
    int newCursorPos = 0;

    // Determine which region was clicked
    if (localY >= HEADER_TOP && localY < HEADER_TOP + LINE_HEIGHT) {
        // Header
        QString header = getHeader();
        int charPos = findCharPosAtX(header, localX, HEADER_LEFT, font);

        // Check if clicked on recipient name - if so, open dialog instead of positioning cursor
        int recipientStart = m_backend->recipientNameStart();
        int recipientEnd = m_backend->recipientNameEnd();
        if (recipientStart >= 0 && recipientEnd >= 0 &&
            charPos >= recipientStart && charPos < recipientEnd) {
            emit recipientNameClicked();
            return;
        }

        newCursorPos = charPos;
    } else if (localY >= BODY_TOP && localY < BODY_TOP + BODY_LINES * LINE_HEIGHT) {
        // Body - determine which visual line
        int visualLine = (localY - BODY_TOP) / LINE_HEIGHT;
        auto wrappedLines = wrapBodyText();

        if (visualLine < wrappedLines.size()) {
            QString lineText = wrappedLines[visualLine].first;
            int lineStartInBody = wrappedLines[visualLine].second;
            int charPos = findCharPosAtX(lineText, localX, BODY_LEFT, font);
            newCursorPos = getBodyStartPos() + lineStartInBody + charPos;
        } else {
            newCursorPos = getFooterStartPos() - 1;  // End of body
        }
    } else if (localY >= FOOTER_TOP && localY < FOOTER_TOP + LINE_HEIGHT) {
        // Footer (right-aligned)
        QString footer = getFooter();
        int totalWidth = 0;
        for (const QChar& ch : footer) {
            totalWidth += font.charWidth(ch) + GLYPH_SPACING;
        }
        int footerStartX = FOOTER_RIGHT - totalWidth;
        int charPos = findCharPosAtX(footer, localX, footerStartX, font);
        newCursorPos = getFooterStartPos() + charPos;
    } else {
        // Clicked outside regions - find nearest
        if (localY < BODY_TOP) {
            newCursorPos = getHeader().length();  // End of header
        } else if (localY < FOOTER_TOP) {
            newCursorPos = getFooterStartPos() - 1;  // End of body
        } else {
            newCursorPos = m_text.length();  // End of footer
        }
    }

    setCursorPosition(newCursorPos);
    m_cursorVisible = true;
    update();
}

int LetterCanvasItem::findCharPosAtX(const QString& text, int targetX, int startX, const FontLoader& font) const {
    int charPos = 0;
    int currentX = startX;

    for (int i = 0; i < text.length(); i++) {
        int charWidth = font.charWidth(text[i]) + GLYPH_SPACING;
        if (targetX < currentX + charWidth / 2) {
            break;
        }
        currentX += charWidth;
        charPos++;
    }

    return qMin(charPos, text.length());
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
    int paperIdx = m_backend->currentPaper();
    QColor textColor = m_backend->stationery().getTextColor(paperIdx);
    QColor recipientColor = m_backend->stationery().getRecipientColor(paperIdx);

    // Get cursor position info
    int cursorSection = getSection(m_cursorPos);

    // Render header with recipient name in different color
    QString header = getHeader();
    int headerCursorCol = (cursorSection == 0) ? m_cursorPos : -1;
    int recipientStart = m_backend->recipientNameStart();
    int recipientEnd = m_backend->recipientNameEnd();
    renderLineWithRecipient(painter, header, HEADER_LEFT, HEADER_TOP, headerCursorCol, font,
                            textColor, recipientColor, recipientStart, recipientEnd);

    // Render body with wrapping
    auto wrappedLines = wrapBodyText();
    int bodyStart = getBodyStartPos();
    int cursorInBody = (cursorSection == 1) ? m_cursorPos - bodyStart : -1;

    bool cursorDrawn = false;
    for (int i = 0; i < BODY_LINES && i < wrappedLines.size(); i++) {
        QString lineText = wrappedLines[i].first;
        int lineStartInBody = wrappedLines[i].second;
        int lineEndInBody = lineStartInBody + lineText.length();

        int lineCursorCol = -1;
        if (!cursorDrawn && cursorInBody >= lineStartInBody && cursorInBody <= lineEndInBody) {
            lineCursorCol = cursorInBody - lineStartInBody;
            cursorDrawn = true;
        }

        renderLine(painter, lineText, BODY_LEFT, BODY_TOP + i * LINE_HEIGHT, lineCursorCol, font, false, textColor);
    }

    // Render footer (right-aligned)
    QString footer = getFooter();
    int footerStart = getFooterStartPos();
    int footerCursorCol = (cursorSection == 2) ? m_cursorPos - footerStart : -1;
    renderLine(painter, footer, FOOTER_RIGHT, FOOTER_TOP, footerCursorCol, font, true, textColor);
}

void LetterCanvasItem::renderLine(QPainter* painter, const QString& text, int x, int y,
                                   int cursorCol, const FontLoader& font, bool rightAlign,
                                   const QColor& textColor) {
    int drawX = x;

    // Calculate width for right alignment
    if (rightAlign) {
        int totalWidth = 0;
        for (const QChar& ch : text) {
            totalWidth += font.charWidth(ch) + GLYPH_SPACING;
        }
        drawX = x - totalWidth;
    }

    int startX = drawX;

    for (int col = 0; col < text.length(); col++) {
        QChar ch = text[col];
        QImage glyph = font.getColoredGlyph(ch, textColor);

        if (!glyph.isNull()) {
            painter->drawImage(drawX, y, glyph);
            drawX += font.charWidth(ch) + GLYPH_SPACING;
        } else {
            drawX += font.charWidth(ch) + GLYPH_SPACING;
        }
    }

    // Draw cursor
    if (m_cursorVisible && cursorCol >= 0) {
        int cursorX = startX;
        for (int col = 0; col < cursorCol && col < text.length(); col++) {
            cursorX += font.charWidth(text[col]) + GLYPH_SPACING;
        }
        painter->setPen(QPen(textColor, 1));
        painter->drawLine(cursorX, y + 1, cursorX, y + LINE_HEIGHT - 2);
    }
}

void LetterCanvasItem::renderLineWithRecipient(QPainter* painter, const QString& text, int x, int y,
                                                int cursorCol, const FontLoader& font,
                                                const QColor& textColor, const QColor& recipientColor,
                                                int recipientStart, int recipientEnd) {
    int drawX = x;
    int startX = drawX;

    for (int col = 0; col < text.length(); col++) {
        QChar ch = text[col];

        // Determine color based on whether this character is part of the recipient name
        QColor charColor = textColor;
        if (recipientStart >= 0 && recipientEnd >= 0 &&
            col >= recipientStart && col < recipientEnd) {
            charColor = recipientColor;
        }

        QImage glyph = font.getColoredGlyph(ch, charColor);

        if (!glyph.isNull()) {
            painter->drawImage(drawX, y, glyph);
            drawX += font.charWidth(ch) + GLYPH_SPACING;
        } else {
            drawX += font.charWidth(ch) + GLYPH_SPACING;
        }
    }

    // Draw cursor
    if (m_cursorVisible && cursorCol >= 0) {
        int cursorX = startX;
        for (int col = 0; col < cursorCol && col < text.length(); col++) {
            cursorX += font.charWidth(text[col]) + GLYPH_SPACING;
        }
        painter->setPen(QPen(textColor, 1));
        painter->drawLine(cursorX, y + 1, cursorX, y + LINE_HEIGHT - 2);
    }
}
