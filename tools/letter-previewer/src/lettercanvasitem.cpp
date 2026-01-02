#include "lettercanvasitem.h"
#include "backend.h"
#include <QPainter>
#include <QGuiApplication>
#include <QClipboard>

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

        // If there's a selection, delete it first
        if (hasSelection()) {
            deleteSelection();
        }

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

            // Check if current logical line (between newlines) would exceed width
            QString body = getBody();
            int bodyOffset = m_cursorPos - getBodyStartPos();

            // Find the start and end of the current logical line
            int lineStart = bodyOffset;
            while (lineStart > 0 && body[lineStart - 1] != '\n') {
                lineStart--;
            }
            int lineEnd = bodyOffset;
            while (lineEnd < body.length() && body[lineEnd] != '\n') {
                lineEnd++;
            }

            // Extract current line and test with new character
            QString currentLine = body.mid(lineStart, lineEnd - lineStart);
            int insertPosInLine = bodyOffset - lineStart;
            currentLine.insert(insertPosInLine, ch);

            // Calculate width of the line after insertion
            const FontLoader& font = m_backend->font();
            int lineWidth = 0;
            for (const QChar& c : currentLine) {
                lineWidth += font.charWidth(c) + GLYPH_SPACING;
            }

            // If line has explicit newline at end, enforce width limit
            bool hasExplicitNewline = (lineEnd < body.length() && body[lineEnd] == '\n');
            if (hasExplicitNewline && lineWidth > MAX_LINE_WIDTH) {
                return;
            }
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
    // If there's a selection, delete it instead of backspacing
    if (hasSelection()) {
        deleteSelection();
        return;
    }

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
    // If there's a selection, delete it instead
    if (hasSelection()) {
        deleteSelection();
        return;
    }

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
    // Clear selection on cursor movement (unless extending selection via Shift)
    if (hasSelection()) {
        m_cursorPos = qMin(m_selectionStart, m_selectionEnd);
        clearSelection();
        m_cursorVisible = true;
        emit cursorPositionChanged();
        update();
        return;
    }

    // Ensure text has proper structure
    if (m_text.count('\n') < 2) {
        m_text = m_text + "\n\n";
        emit textChanged();
    }

    if (m_cursorPos > 0) {
        int currentSection = getSection(m_cursorPos);

        // At start of body, jump to end of header
        if (currentSection == 1 && m_cursorPos == getBodyStartPos()) {
            m_cursorPos = getBodyStartPos() - 1;  // End of header (before newline)
            // Skip over recipient name if at end
            if (m_backend) {
                int recipientEnd = m_backend->recipientNameEnd();
                if (recipientEnd >= 0 && m_cursorPos == recipientEnd) {
                    m_cursorPos = m_backend->recipientNameStart();
                }
            }
        }
        // At start of footer, jump to end of body
        else if (currentSection == 2 && m_cursorPos == getFooterStartPos()) {
            m_cursorPos = getFooterStartPos() - 1;  // End of body (before newline)
        }
        else {
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
        }

        m_cursorVisible = true;
        emit cursorPositionChanged();
        update();
    }
}

void LetterCanvasItem::moveCursorRight() {
    // Clear selection on cursor movement (unless extending selection via Shift)
    if (hasSelection()) {
        m_cursorPos = qMax(m_selectionStart, m_selectionEnd);
        clearSelection();
        m_cursorVisible = true;
        emit cursorPositionChanged();
        update();
        return;
    }

    // Ensure text has proper structure
    if (m_text.count('\n') < 2) {
        m_text = m_text + "\n\n";
        emit textChanged();
    }

    if (m_cursorPos < m_text.length()) {
        int currentSection = getSection(m_cursorPos);
        QString header = getHeader();
        QString body = getBody();

        // At end of header, jump to start of body
        if (currentSection == 0 && m_cursorPos == header.length()) {
            m_cursorPos = getBodyStartPos();
        }
        // At end of body, jump to start of footer
        else if (currentSection == 1 && m_cursorPos == getFooterStartPos() - 1) {
            m_cursorPos = getFooterStartPos();
        }
        else {
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
        }

        m_cursorVisible = true;
        emit cursorPositionChanged();
        update();
    }
}

void LetterCanvasItem::moveCursorUp() {
    // Clear selection on cursor movement
    if (hasSelection()) {
        m_cursorPos = qMin(m_selectionStart, m_selectionEnd);
        clearSelection();
        m_cursorVisible = true;
        emit cursorPositionChanged();
        update();
        return;
    }

    // Ensure text has proper structure
    if (m_text.count('\n') < 2) {
        m_text = m_text + "\n\n";
        emit textChanged();
    }

    int currentSection = getSection(m_cursorPos);

    if (currentSection == 0) {
        // In header - nowhere to go up
        return;
    } else if (currentSection == 2) {
        // In footer - move to last line of body
        auto wrappedLines = wrapBodyText();
        // Find the last non-empty line
        int lastLineIdx = BODY_LINES - 1;
        for (int i = BODY_LINES - 1; i >= 0; i--) {
            if (!wrappedLines[i].first.isEmpty() || i == 0) {
                lastLineIdx = i;
                break;
            }
        }
        // Position at end of last body line
        int bodyStart = getBodyStartPos();
        int lineStartInBody = wrappedLines[lastLineIdx].second;
        m_cursorPos = bodyStart + lineStartInBody + wrappedLines[lastLineIdx].first.length();
    } else {
        // In body - find which visual line we're on
        auto wrappedLines = wrapBodyText();
        int bodyStart = getBodyStartPos();
        int cursorInBody = m_cursorPos - bodyStart;

        int currentLine = -1;
        int posInLine = 0;
        for (int i = 0; i < BODY_LINES; i++) {
            int lineStart = wrappedLines[i].second;
            int lineEnd = lineStart + wrappedLines[i].first.length();
            if (cursorInBody >= lineStart && cursorInBody <= lineEnd) {
                currentLine = i;
                posInLine = cursorInBody - lineStart;
                break;
            }
        }

        if (currentLine <= 0) {
            // On first line of body - move to header
            QString header = getHeader();
            m_cursorPos = qMin(posInLine, header.length());
            // Skip recipient name if we landed in it
            if (m_backend) {
                int recipientStart = m_backend->recipientNameStart();
                int recipientEnd = m_backend->recipientNameEnd();
                if (recipientStart >= 0 && recipientEnd >= 0 &&
                    m_cursorPos > recipientStart && m_cursorPos < recipientEnd) {
                    m_cursorPos = recipientEnd;
                }
            }
        } else {
            // Move to previous line, same approximate position
            int prevLineStart = wrappedLines[currentLine - 1].second;
            int prevLineLen = wrappedLines[currentLine - 1].first.length();
            m_cursorPos = bodyStart + prevLineStart + qMin(posInLine, prevLineLen);
        }
    }

    m_cursorVisible = true;
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::moveCursorDown() {
    // Clear selection on cursor movement
    if (hasSelection()) {
        m_cursorPos = qMax(m_selectionStart, m_selectionEnd);
        clearSelection();
        m_cursorVisible = true;
        emit cursorPositionChanged();
        update();
        return;
    }

    // Ensure text has proper structure
    if (m_text.count('\n') < 2) {
        m_text = m_text + "\n\n";
        emit textChanged();
    }

    int currentSection = getSection(m_cursorPos);

    if (currentSection == 2) {
        // In footer - nowhere to go down
        return;
    } else if (currentSection == 0) {
        // In header - move to first line of body
        QString header = getHeader();
        int posInHeader = m_cursorPos;
        // Skip recipient name width if cursor is after it
        if (m_backend) {
            int recipientStart = m_backend->recipientNameStart();
            int recipientEnd = m_backend->recipientNameEnd();
            if (recipientStart >= 0 && recipientEnd >= 0 && m_cursorPos >= recipientEnd) {
                // Adjust for the fact that recipient name is a single "token"
                posInHeader = m_cursorPos;
            }
        }
        auto wrappedLines = wrapBodyText();
        int bodyStart = getBodyStartPos();
        int firstLineLen = wrappedLines[0].first.length();
        m_cursorPos = bodyStart + qMin(posInHeader, firstLineLen);
    } else {
        // In body - find which visual line we're on
        auto wrappedLines = wrapBodyText();
        int bodyStart = getBodyStartPos();
        int cursorInBody = m_cursorPos - bodyStart;

        int currentLine = -1;
        int posInLine = 0;
        for (int i = 0; i < BODY_LINES; i++) {
            int lineStart = wrappedLines[i].second;
            int lineEnd = lineStart + wrappedLines[i].first.length();
            if (cursorInBody >= lineStart && cursorInBody <= lineEnd) {
                currentLine = i;
                posInLine = cursorInBody - lineStart;
                break;
            }
        }

        // Check if we're on the last used line
        bool onLastLine = (currentLine == BODY_LINES - 1);
        if (!onLastLine && currentLine >= 0) {
            // Check if next line is empty (meaning current is effectively last)
            if (wrappedLines[currentLine + 1].first.isEmpty()) {
                onLastLine = true;
            }
        }

        if (onLastLine || currentLine < 0) {
            // On last line of body - move to footer
            QString footer = getFooter();
            m_cursorPos = getFooterStartPos() + qMin(posInLine, footer.length());
        } else {
            // Move to next line, same approximate position
            int nextLineStart = wrappedLines[currentLine + 1].second;
            int nextLineLen = wrappedLines[currentLine + 1].first.length();
            m_cursorPos = bodyStart + nextLineStart + qMin(posInLine, nextLineLen);
        }
    }

    m_cursorVisible = true;
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::moveCursorHome() {
    // Clear selection on cursor movement
    if (hasSelection()) {
        clearSelection();
    }

    // Ensure text has proper structure
    if (m_text.count('\n') < 2) {
        m_text = m_text + "\n\n";
        emit textChanged();
    }

    int currentSection = getSection(m_cursorPos);

    if (currentSection == 0) {
        // Header - move to start (position 0)
        m_cursorPos = 0;
    } else if (currentSection == 1) {
        // Body - move to start of current visual line
        auto wrappedLines = wrapBodyText();
        int bodyStart = getBodyStartPos();
        int cursorInBody = m_cursorPos - bodyStart;

        for (int i = 0; i < BODY_LINES; i++) {
            int lineStart = wrappedLines[i].second;
            int lineEnd = lineStart + wrappedLines[i].first.length();
            if (cursorInBody >= lineStart && cursorInBody <= lineEnd) {
                m_cursorPos = bodyStart + lineStart;
                break;
            }
        }
    } else {
        // Footer - move to start of footer
        m_cursorPos = getFooterStartPos();
    }

    m_cursorVisible = true;
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::moveCursorEnd() {
    // Clear selection on cursor movement
    if (hasSelection()) {
        clearSelection();
    }

    // Ensure text has proper structure
    if (m_text.count('\n') < 2) {
        m_text = m_text + "\n\n";
        emit textChanged();
    }

    int currentSection = getSection(m_cursorPos);

    if (currentSection == 0) {
        // Header - move to end (before the newline)
        QString header = getHeader();
        m_cursorPos = header.length();
    } else if (currentSection == 1) {
        // Body - move to end of current visual line
        auto wrappedLines = wrapBodyText();
        int bodyStart = getBodyStartPos();
        int cursorInBody = m_cursorPos - bodyStart;

        for (int i = 0; i < BODY_LINES; i++) {
            int lineStart = wrappedLines[i].second;
            int lineEnd = lineStart + wrappedLines[i].first.length();
            if (cursorInBody >= lineStart && cursorInBody <= lineEnd) {
                m_cursorPos = bodyStart + lineEnd;
                break;
            }
        }
    } else {
        // Footer - move to end of footer
        m_cursorPos = m_text.length();
    }

    m_cursorVisible = true;
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::newLine() {
    int section = getSection(m_cursorPos);

    // In header, Enter acts like Down arrow (move to body)
    if (section == 0) {
        moveCursorDown();
        return;
    }

    // In footer, Enter does nothing (nowhere to go)
    if (section == 2) {
        return;
    }

    // In body: check if we're on the last line or if adding would overflow
    auto wrappedLines = wrapBodyText();
    int bodyStart = getBodyStartPos();
    int cursorInBody = m_cursorPos - bodyStart;

    // Find which visual line we're on
    int currentLine = -1;
    for (int i = 0; i < BODY_LINES; i++) {
        int lineStart = wrappedLines[i].second;
        int lineEnd = lineStart + wrappedLines[i].first.length();
        if (cursorInBody >= lineStart && cursorInBody <= lineEnd) {
            currentLine = i;
            break;
        }
    }

    // Check if we're on line 4 or if body is already full
    bool onLastLine = (currentLine == BODY_LINES - 1);
    if (!onLastLine && currentLine >= 0 && currentLine < BODY_LINES - 1) {
        // Check if next line is empty (meaning current is effectively last with content)
        if (wrappedLines[currentLine + 1].first.isEmpty()) {
            // Check if we already have 3 newlines (4 lines max)
            QString body = getBody();
            if (body.count('\n') >= 3) {
                onLastLine = true;
            }
        }
    }

    // On last line, Enter moves to footer
    if (onLastLine) {
        moveCursorDown();
        return;
    }

    // Check if adding a newline would overflow
    QString body = getBody();
    if (body.count('\n') >= 3) {
        moveCursorDown();  // Move to footer instead
        return;
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
        // Check if clicked on recipient name using pixel bounds
        int recipientStart = m_backend->recipientNameStart();
        int recipientEnd = m_backend->recipientNameEnd();
        if (recipientStart >= 0 && recipientEnd >= 0) {
            // Calculate pixel bounds of recipient name
            int nameStartX = HEADER_LEFT;
            for (int i = 0; i < recipientStart && i < header.length(); i++) {
                nameStartX += font.charWidth(header[i]) + GLYPH_SPACING;
            }
            int nameEndX = nameStartX;
            for (int i = recipientStart; i < recipientEnd && i < header.length(); i++) {
                nameEndX += font.charWidth(header[i]) + GLYPH_SPACING;
            }

            // Only trigger popup if click is within the name's pixel bounds
            if (localX >= nameStartX && localX < nameEndX) {
                emit recipientNameClicked();
                return;
            }
        }

        int charPos = findCharPosAtX(header, localX, HEADER_LEFT, font);
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

int LetterCanvasItem::charPosFromPoint(qreal x, qreal y) const {
    if (!m_backend || !m_backend->isLoaded()) return 0;

    // Convert from widget coordinates to 1x coordinates
    qreal scaleX = width() / 256.0;
    qreal scaleY = height() / 192.0;
    qreal scale = qMin(scaleX, scaleY);

    int scaledW = static_cast<int>(256 * scale);
    int scaledH = static_cast<int>(192 * scale);
    int offsetX = (width() - scaledW) / 2;
    int offsetY = (height() - scaledH) / 2;

    int localX = static_cast<int>((x - offsetX) / scale);
    int localY = static_cast<int>((y - offsetY) / scale);

    const FontLoader& font = m_backend->font();

    // Determine which region was clicked and return absolute position
    if (localY >= HEADER_TOP && localY < HEADER_TOP + LINE_HEIGHT) {
        // Header
        QString header = getHeader();
        int charPos = findCharPosAtX(header, localX, HEADER_LEFT, font);
        return charPos;
    } else if (localY >= BODY_TOP && localY < BODY_TOP + BODY_LINES * LINE_HEIGHT) {
        // Body - determine which visual line
        int visualLine = (localY - BODY_TOP) / LINE_HEIGHT;
        auto wrappedLines = wrapBodyText();

        if (visualLine < wrappedLines.size()) {
            QString lineText = wrappedLines[visualLine].first;
            int lineStartInBody = wrappedLines[visualLine].second;
            int charPos = findCharPosAtX(lineText, localX, BODY_LEFT, font);
            return getBodyStartPos() + lineStartInBody + charPos;
        } else {
            return getFooterStartPos() - 1;  // End of body
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
        return getFooterStartPos() + charPos;
    } else {
        // Clicked outside regions - find nearest
        if (localY < BODY_TOP) {
            return getHeader().length();  // End of header
        } else if (localY < FOOTER_TOP) {
            return getFooterStartPos() - 1;  // End of body
        } else {
            return m_text.length();  // End of footer
        }
    }
}

void LetterCanvasItem::startSelection(qreal x, qreal y) {
    if (!m_backend || !m_backend->isLoaded()) return;

    // Ensure text has proper structure
    if (m_text.count('\n') < 2) {
        m_text = "\n\n";
        emit textChanged();
    }

    int pos = charPosFromPoint(x, y);

    // Skip over recipient name if clicked on it
    int section = getSection(pos);
    if (section == 0 && m_backend) {
        int recipientStart = m_backend->recipientNameStart();
        int recipientEnd = m_backend->recipientNameEnd();
        if (recipientStart >= 0 && recipientEnd >= 0 &&
            pos > recipientStart && pos < recipientEnd) {
            // Don't start selection within recipient name
            return;
        }
    }

    m_selectionAnchor = pos;
    m_selectionStart = pos;
    m_selectionEnd = pos;
    m_cursorPos = pos;

    m_cursorVisible = true;
    emit selectionChanged();
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::updateSelection(qreal x, qreal y) {
    if (m_selectionAnchor < 0) return;
    if (!m_backend || !m_backend->isLoaded()) return;

    int pos = charPosFromPoint(x, y);

    // Clamp selection to same section as anchor
    int anchorSection = getSection(m_selectionAnchor);
    int posSection = getSection(pos);

    if (posSection != anchorSection) {
        // Clamp to section boundary
        if (anchorSection == 0) {
            // Header - clamp to first newline
            int firstNewline = m_text.indexOf('\n');
            pos = (posSection > anchorSection) ? firstNewline : 0;
        } else if (anchorSection == 1) {
            // Body - clamp to body boundaries
            int bodyStart = getBodyStartPos();
            int footerStart = getFooterStartPos();
            pos = (posSection > anchorSection) ? footerStart - 1 : bodyStart;
        } else {
            // Footer - clamp to footer boundaries
            int footerStart = getFooterStartPos();
            pos = (posSection < anchorSection) ? footerStart : m_text.length();
        }
    }

    // Skip over recipient name in header
    if (anchorSection == 0 && m_backend) {
        int recipientStart = m_backend->recipientNameStart();
        int recipientEnd = m_backend->recipientNameEnd();
        if (recipientStart >= 0 && recipientEnd >= 0) {
            // If dragging over recipient name, skip it
            if (pos > recipientStart && pos < recipientEnd) {
                if (m_selectionAnchor <= recipientStart) {
                    pos = recipientEnd;  // Selecting forward - skip to end
                } else {
                    pos = recipientStart;  // Selecting backward - skip to start
                }
            }
        }
    }

    // Update selection range
    if (pos < m_selectionAnchor) {
        m_selectionStart = pos;
        m_selectionEnd = m_selectionAnchor;
    } else {
        m_selectionStart = m_selectionAnchor;
        m_selectionEnd = pos;
    }

    m_cursorPos = pos;

    emit selectionChanged();
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::clearSelection() {
    if (m_selectionStart >= 0 || m_selectionEnd >= 0 || m_selectionAnchor >= 0) {
        m_selectionStart = -1;
        m_selectionEnd = -1;
        m_selectionAnchor = -1;
        emit selectionChanged();
        update();
    }
}

void LetterCanvasItem::deleteSelection() {
    if (!hasSelection()) return;
    if (!m_backend) return;

    int start = qMin(m_selectionStart, m_selectionEnd);
    int end = qMax(m_selectionStart, m_selectionEnd);

    // Protect recipient name in header - don't delete any part of it
    int section = getSection(start);
    if (section == 0) {
        int recipientStart = m_backend->recipientNameStart();
        int recipientEnd = m_backend->recipientNameEnd();
        if (recipientStart >= 0 && recipientEnd >= 0) {
            // If selection includes any part of recipient name, abort
            if ((start < recipientEnd && end > recipientStart)) {
                clearSelection();
                return;
            }
            // If deleting before recipient name, adjust positions
            if (end <= recipientStart) {
                int deleteLen = end - start;
                m_backend->setRecipientNameStart(recipientStart - deleteLen);
                m_backend->setRecipientNameEnd(recipientEnd - deleteLen);
            }
        }
    }

    // Don't delete across section boundaries (newlines)
    int startSection = getSection(start);
    int endSection = getSection(end);
    if (startSection != endSection) {
        clearSelection();
        return;
    }

    // Delete the selected text
    m_text.remove(start, end - start);
    m_cursorPos = start;

    clearSelection();
    emit textChanged();
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::selectAll() {
    if (!m_backend || !m_backend->isLoaded()) return;

    // Select all text in current section
    int section = getSection(m_cursorPos);

    if (section == 0) {
        // Header
        m_selectionStart = 0;
        m_selectionEnd = m_text.indexOf('\n');
        if (m_selectionEnd < 0) m_selectionEnd = m_text.length();
    } else if (section == 1) {
        // Body
        m_selectionStart = getBodyStartPos();
        m_selectionEnd = getFooterStartPos() - 1;  // Exclude the newline
    } else {
        // Footer
        m_selectionStart = getFooterStartPos();
        m_selectionEnd = m_text.length();
    }

    m_selectionAnchor = m_selectionStart;
    m_cursorPos = m_selectionEnd;

    emit selectionChanged();
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::extendSelectionLeft() {
    if (!m_backend || !m_backend->isLoaded()) return;

    // If no selection, start one at current cursor position
    if (!hasSelection()) {
        m_selectionAnchor = m_cursorPos;
        m_selectionStart = m_cursorPos;
        m_selectionEnd = m_cursorPos;
    }

    // Move cursor left
    if (m_cursorPos > 0) {
        int newPos = m_cursorPos - 1;
        int cursorSection = getSection(m_cursorPos);
        int newSection = getSection(newPos);

        // Don't cross section boundaries
        if (newSection != cursorSection) {
            return;
        }

        // Skip over recipient name in header
        if (cursorSection == 0 && m_backend) {
            int recipientStart = m_backend->recipientNameStart();
            int recipientEnd = m_backend->recipientNameEnd();
            if (recipientStart >= 0 && recipientEnd >= 0 &&
                newPos >= recipientStart && newPos < recipientEnd) {
                newPos = recipientStart;
            }
        }

        m_cursorPos = newPos;

        // Update selection range based on anchor
        if (m_cursorPos < m_selectionAnchor) {
            m_selectionStart = m_cursorPos;
            m_selectionEnd = m_selectionAnchor;
        } else {
            m_selectionStart = m_selectionAnchor;
            m_selectionEnd = m_cursorPos;
        }

        m_cursorVisible = true;
        emit selectionChanged();
        emit cursorPositionChanged();
        update();
    }
}

void LetterCanvasItem::extendSelectionRight() {
    if (!m_backend || !m_backend->isLoaded()) return;

    // If no selection, start one at current cursor position
    if (!hasSelection()) {
        m_selectionAnchor = m_cursorPos;
        m_selectionStart = m_cursorPos;
        m_selectionEnd = m_cursorPos;
    }

    // Move cursor right
    if (m_cursorPos < m_text.length()) {
        int newPos = m_cursorPos + 1;
        int cursorSection = getSection(m_cursorPos);
        int newSection = getSection(newPos);

        // Don't cross section boundaries
        if (newSection != cursorSection) {
            return;
        }

        // Skip over recipient name in header
        if (cursorSection == 0 && m_backend) {
            int recipientStart = m_backend->recipientNameStart();
            int recipientEnd = m_backend->recipientNameEnd();
            if (recipientStart >= 0 && recipientEnd >= 0 &&
                newPos > recipientStart && newPos < recipientEnd) {
                newPos = recipientEnd;
            }
        }

        m_cursorPos = newPos;

        // Update selection range based on anchor
        if (m_cursorPos < m_selectionAnchor) {
            m_selectionStart = m_cursorPos;
            m_selectionEnd = m_selectionAnchor;
        } else {
            m_selectionStart = m_selectionAnchor;
            m_selectionEnd = m_cursorPos;
        }

        m_cursorVisible = true;
        emit selectionChanged();
        emit cursorPositionChanged();
        update();
    }
}

void LetterCanvasItem::extendSelectionHome() {
    if (!m_backend || !m_backend->isLoaded()) return;

    // Ensure text has proper structure
    if (m_text.count('\n') < 2) {
        m_text = m_text + "\n\n";
        emit textChanged();
    }

    // If no selection, start one at current cursor position
    if (!hasSelection()) {
        m_selectionAnchor = m_cursorPos;
        m_selectionStart = m_cursorPos;
        m_selectionEnd = m_cursorPos;
    }

    int currentSection = getSection(m_cursorPos);
    int newPos = m_cursorPos;

    if (currentSection == 0) {
        // Header - move to start
        newPos = 0;
    } else if (currentSection == 1) {
        // Body - move to start of current visual line
        auto wrappedLines = wrapBodyText();
        int bodyStart = getBodyStartPos();
        int cursorInBody = m_cursorPos - bodyStart;

        for (int i = 0; i < BODY_LINES; i++) {
            int lineStart = wrappedLines[i].second;
            int lineEnd = lineStart + wrappedLines[i].first.length();
            if (cursorInBody >= lineStart && cursorInBody <= lineEnd) {
                newPos = bodyStart + lineStart;
                break;
            }
        }
    } else {
        // Footer - move to start of footer
        newPos = getFooterStartPos();
    }

    m_cursorPos = newPos;

    // Update selection range based on anchor
    if (m_cursorPos < m_selectionAnchor) {
        m_selectionStart = m_cursorPos;
        m_selectionEnd = m_selectionAnchor;
    } else {
        m_selectionStart = m_selectionAnchor;
        m_selectionEnd = m_cursorPos;
    }

    m_cursorVisible = true;
    emit selectionChanged();
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::extendSelectionEnd() {
    if (!m_backend || !m_backend->isLoaded()) return;

    // Ensure text has proper structure
    if (m_text.count('\n') < 2) {
        m_text = m_text + "\n\n";
        emit textChanged();
    }

    // If no selection, start one at current cursor position
    if (!hasSelection()) {
        m_selectionAnchor = m_cursorPos;
        m_selectionStart = m_cursorPos;
        m_selectionEnd = m_cursorPos;
    }

    int currentSection = getSection(m_cursorPos);
    int newPos = m_cursorPos;

    if (currentSection == 0) {
        // Header - move to end
        QString header = getHeader();
        newPos = header.length();
    } else if (currentSection == 1) {
        // Body - move to end of current visual line
        auto wrappedLines = wrapBodyText();
        int bodyStart = getBodyStartPos();
        int cursorInBody = m_cursorPos - bodyStart;

        for (int i = 0; i < BODY_LINES; i++) {
            int lineStart = wrappedLines[i].second;
            int lineEnd = lineStart + wrappedLines[i].first.length();
            if (cursorInBody >= lineStart && cursorInBody <= lineEnd) {
                newPos = bodyStart + lineEnd;
                break;
            }
        }
    } else {
        // Footer - move to end of footer
        newPos = m_text.length();
    }

    m_cursorPos = newPos;

    // Update selection range based on anchor
    if (m_cursorPos < m_selectionAnchor) {
        m_selectionStart = m_cursorPos;
        m_selectionEnd = m_selectionAnchor;
    } else {
        m_selectionStart = m_selectionAnchor;
        m_selectionEnd = m_cursorPos;
    }

    m_cursorVisible = true;
    emit selectionChanged();
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::extendSelectionUp() {
    if (!m_backend || !m_backend->isLoaded()) return;

    // Ensure text has proper structure
    if (m_text.count('\n') < 2) {
        m_text = m_text + "\n\n";
        emit textChanged();
    }

    // If no selection, start one at current cursor position
    if (!hasSelection()) {
        m_selectionAnchor = m_cursorPos;
        m_selectionStart = m_cursorPos;
        m_selectionEnd = m_cursorPos;
    }

    int currentSection = getSection(m_cursorPos);

    if (currentSection == 0) {
        // In header - treat like Shift+Home (select to start)
        m_cursorPos = 0;
    } else if (currentSection == 2) {
        // In footer - treat like Shift+Home (select to start of footer)
        m_cursorPos = getFooterStartPos();
    } else {
        // In body - move up within body only
        auto wrappedLines = wrapBodyText();
        int bodyStart = getBodyStartPos();
        int cursorInBody = m_cursorPos - bodyStart;

        int currentLine = -1;
        int posInLine = 0;
        for (int i = 0; i < BODY_LINES; i++) {
            int lineStart = wrappedLines[i].second;
            int lineEnd = lineStart + wrappedLines[i].first.length();
            if (cursorInBody >= lineStart && cursorInBody <= lineEnd) {
                currentLine = i;
                posInLine = cursorInBody - lineStart;
                break;
            }
        }

        if (currentLine <= 0) {
            // On first line of body - select to start of body (don't leave body)
            m_cursorPos = bodyStart;
        } else {
            // Move to previous line, same approximate position
            int prevLineStart = wrappedLines[currentLine - 1].second;
            int prevLineLen = wrappedLines[currentLine - 1].first.length();
            m_cursorPos = bodyStart + prevLineStart + qMin(posInLine, prevLineLen);
        }
    }

    // Update selection range based on anchor
    if (m_cursorPos < m_selectionAnchor) {
        m_selectionStart = m_cursorPos;
        m_selectionEnd = m_selectionAnchor;
    } else {
        m_selectionStart = m_selectionAnchor;
        m_selectionEnd = m_cursorPos;
    }

    m_cursorVisible = true;
    emit selectionChanged();
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::extendSelectionDown() {
    if (!m_backend || !m_backend->isLoaded()) return;

    // Ensure text has proper structure
    if (m_text.count('\n') < 2) {
        m_text = m_text + "\n\n";
        emit textChanged();
    }

    // If no selection, start one at current cursor position
    if (!hasSelection()) {
        m_selectionAnchor = m_cursorPos;
        m_selectionStart = m_cursorPos;
        m_selectionEnd = m_cursorPos;
    }

    int currentSection = getSection(m_cursorPos);

    if (currentSection == 0) {
        // In header - treat like Shift+End (select to end of header)
        QString header = getHeader();
        m_cursorPos = header.length();
    } else if (currentSection == 2) {
        // In footer - treat like Shift+End (select to end)
        m_cursorPos = m_text.length();
    } else {
        // In body - move down within body only
        auto wrappedLines = wrapBodyText();
        int bodyStart = getBodyStartPos();
        int cursorInBody = m_cursorPos - bodyStart;

        int currentLine = -1;
        int posInLine = 0;
        for (int i = 0; i < BODY_LINES; i++) {
            int lineStart = wrappedLines[i].second;
            int lineEnd = lineStart + wrappedLines[i].first.length();
            if (cursorInBody >= lineStart && cursorInBody <= lineEnd) {
                currentLine = i;
                posInLine = cursorInBody - lineStart;
                break;
            }
        }

        // Check if we're on the last used line
        bool onLastLine = (currentLine == BODY_LINES - 1);
        if (!onLastLine && currentLine >= 0 && currentLine < BODY_LINES - 1) {
            if (wrappedLines[currentLine + 1].first.isEmpty()) {
                onLastLine = true;
            }
        }

        if (onLastLine || currentLine < 0) {
            // On last line of body - select to end of body (don't leave body)
            m_cursorPos = getFooterStartPos() - 1;
        } else {
            // Move to next line, same approximate position
            int nextLineStart = wrappedLines[currentLine + 1].second;
            int nextLineLen = wrappedLines[currentLine + 1].first.length();
            m_cursorPos = bodyStart + nextLineStart + qMin(posInLine, nextLineLen);
        }
    }

    // Update selection range based on anchor
    if (m_cursorPos < m_selectionAnchor) {
        m_selectionStart = m_cursorPos;
        m_selectionEnd = m_selectionAnchor;
    } else {
        m_selectionStart = m_selectionAnchor;
        m_selectionEnd = m_cursorPos;
    }

    m_cursorVisible = true;
    emit selectionChanged();
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::copySelection() {
    if (!hasSelection()) return;

    int start = qMin(m_selectionStart, m_selectionEnd);
    int end = qMax(m_selectionStart, m_selectionEnd);
    QString selectedText = m_text.mid(start, end - start);

    QGuiApplication::clipboard()->setText(selectedText);
}

void LetterCanvasItem::cutSelection() {
    if (!hasSelection()) return;

    copySelection();
    deleteSelection();
}

void LetterCanvasItem::paste() {
    QString clipboardText = QGuiApplication::clipboard()->text();
    if (clipboardText.isEmpty()) return;
    if (!m_backend || !m_backend->isLoaded()) return;

    // Delete selection first if exists
    if (hasSelection()) {
        deleteSelection();
    }

    // Insert each character one at a time (reuses existing validation)
    for (const QChar& ch : clipboardText) {
        if (ch == '\n') {
            // Only allow newlines in body section
            if (getSection(m_cursorPos) == 1) {
                newLine();
            }
        } else if (ch.isPrint() && ch.unicode() < 128) {
            insertChar(QString(ch));
        }
        // Skip non-printable and non-ASCII characters
    }
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
    QColor selectionColor(56, 189, 248, 100);  // Semi-transparent blue

    // Get cursor position info
    int cursorSection = getSection(m_cursorPos);

    // Get selection range for rendering
    int selStart = (hasSelection()) ? qMin(m_selectionStart, m_selectionEnd) : -1;
    int selEnd = (hasSelection()) ? qMax(m_selectionStart, m_selectionEnd) : -1;

    // Render header with recipient name in different color
    QString header = getHeader();
    int headerCursorCol = (cursorSection == 0) ? m_cursorPos : -1;
    int recipientStart = m_backend->recipientNameStart();
    int recipientEnd = m_backend->recipientNameEnd();

    // Calculate header selection (relative to header start which is 0)
    int headerSelStart = -1, headerSelEnd = -1;
    if (selStart >= 0 && getSection(selStart) == 0) {
        headerSelStart = qMax(0, selStart);
        headerSelEnd = qMin(header.length(), selEnd);
    }

    renderLineWithRecipient(painter, header, HEADER_LEFT, HEADER_TOP, headerCursorCol, font,
                            textColor, recipientColor, recipientStart, recipientEnd,
                            headerSelStart, headerSelEnd, selectionColor);

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

        // Calculate selection for this body line (relative to line start)
        int lineSelStart = -1, lineSelEnd = -1;
        if (selStart >= 0 && getSection(selStart) == 1) {
            int lineAbsStart = bodyStart + lineStartInBody;
            int lineAbsEnd = bodyStart + lineEndInBody;
            if (selEnd > lineAbsStart && selStart < lineAbsEnd) {
                lineSelStart = qMax(0, selStart - lineAbsStart);
                lineSelEnd = qMin(lineText.length(), selEnd - lineAbsStart);
            }
        }

        renderLine(painter, lineText, BODY_LEFT, BODY_TOP + i * LINE_HEIGHT, lineCursorCol, font,
                   false, textColor, lineSelStart, lineSelEnd, selectionColor);
    }

    // Render footer (right-aligned)
    QString footer = getFooter();
    int footerStart = getFooterStartPos();
    int footerCursorCol = (cursorSection == 2) ? m_cursorPos - footerStart : -1;

    // Calculate footer selection (relative to footer start)
    int footerSelStart = -1, footerSelEnd = -1;
    if (selStart >= 0 && getSection(selStart) == 2) {
        footerSelStart = qMax(0, selStart - footerStart);
        footerSelEnd = qMin(footer.length(), selEnd - footerStart);
    }

    renderLine(painter, footer, FOOTER_RIGHT, FOOTER_TOP, footerCursorCol, font, true, textColor,
               footerSelStart, footerSelEnd, selectionColor);
}

void LetterCanvasItem::renderLine(QPainter* painter, const QString& text, int x, int y,
                                   int cursorCol, const FontLoader& font, bool rightAlign,
                                   const QColor& textColor, int selStart, int selEnd,
                                   const QColor& selectionColor) {
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

    // Draw selection background first (if any)
    if (selStart >= 0 && selEnd > selStart) {
        int selStartX = startX;
        for (int col = 0; col < selStart && col < text.length(); col++) {
            selStartX += font.charWidth(text[col]) + GLYPH_SPACING;
        }
        int selEndX = selStartX;
        for (int col = selStart; col < selEnd && col < text.length(); col++) {
            selEndX += font.charWidth(text[col]) + GLYPH_SPACING;
        }
        painter->fillRect(selStartX, y, selEndX - selStartX, LINE_HEIGHT, selectionColor);
    }

    // Draw text
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
                                                int recipientStart, int recipientEnd,
                                                int selStart, int selEnd, const QColor& selectionColor) {
    int drawX = x;
    int startX = drawX;

    // Draw selection background first (if any)
    if (selStart >= 0 && selEnd > selStart) {
        int selStartX = startX;
        for (int col = 0; col < selStart && col < text.length(); col++) {
            selStartX += font.charWidth(text[col]) + GLYPH_SPACING;
        }
        int selEndX = selStartX;
        for (int col = selStart; col < selEnd && col < text.length(); col++) {
            selEndX += font.charWidth(text[col]) + GLYPH_SPACING;
        }
        painter->fillRect(selStartX, y, selEndX - selStartX, LINE_HEIGHT, selectionColor);
    }

    // Draw text
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
