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

// Combined text for backward compatibility
QString LetterCanvasItem::text() const {
    return m_header + "\n" + m_body + "\n" + m_footer;
}

void LetterCanvasItem::setText(const QString& text) {
    int firstNewline = text.indexOf('\n');
    int lastNewline = text.lastIndexOf('\n');

    if (firstNewline < 0) {
        m_header = text;
        m_body = "";
        m_footer = "";
    } else if (firstNewline == lastNewline) {
        m_header = text.left(firstNewline);
        m_body = text.mid(firstNewline + 1);
        m_footer = "";
    } else {
        m_header = text.left(firstNewline);
        m_body = text.mid(firstNewline + 1, lastNewline - firstNewline - 1);
        m_footer = text.mid(lastNewline + 1);
    }

    // Clamp cursor to valid position
    m_cursorPosInSection = qMin(m_cursorPosInSection, sectionText(m_currentSection).length());

    emit textChanged();
    update();
}

// Section text accessor
QString& LetterCanvasItem::sectionText(int section) {
    if (section == 0) return m_header;
    if (section == 1) return m_body;
    return m_footer;
}

const QString& LetterCanvasItem::sectionText(int section) const {
    if (section == 0) return m_header;
    if (section == 1) return m_body;
    return m_footer;
}

// Position conversion helpers
int LetterCanvasItem::sectionPosToGlobal(int section, int posInSection) const {
    if (section == 0) return posInSection;
    if (section == 1) return m_header.length() + 1 + posInSection;
    return m_header.length() + 1 + m_body.length() + 1 + posInSection;
}

void LetterCanvasItem::globalPosToSection(int globalPos, int& outSection, int& outPosInSection) const {
    int headerEnd = m_header.length();
    int bodyStart = headerEnd + 1;
    int bodyEnd = bodyStart + m_body.length();
    int footerStart = bodyEnd + 1;

    if (globalPos <= headerEnd) {
        outSection = 0;
        outPosInSection = globalPos;
    } else if (globalPos < bodyStart) {
        outSection = 0;
        outPosInSection = headerEnd;
    } else if (globalPos <= bodyEnd) {
        outSection = 1;
        outPosInSection = globalPos - bodyStart;
    } else if (globalPos < footerStart) {
        outSection = 1;
        outPosInSection = m_body.length();
    } else {
        outSection = 2;
        outPosInSection = globalPos - footerStart;
    }
}

int LetterCanvasItem::cursorPosition() const {
    return sectionPosToGlobal(m_currentSection, m_cursorPosInSection);
}

void LetterCanvasItem::setCursorPosition(int pos) {
    int newSection, newPosInSection;
    globalPosToSection(pos, newSection, newPosInSection);

    if (m_currentSection != newSection || m_cursorPosInSection != newPosInSection) {
        m_currentSection = newSection;
        m_cursorPosInSection = newPosInSection;
        emit cursorPositionChanged();
        emit currentSectionChanged();
        update();
    }
}

int LetterCanvasItem::selectionStart() const {
    if (m_selectionSection < 0 || m_selectionStartInSection < 0) return -1;
    return sectionPosToGlobal(m_selectionSection, m_selectionStartInSection);
}

int LetterCanvasItem::selectionEnd() const {
    if (m_selectionSection < 0 || m_selectionEndInSection < 0) return -1;
    return sectionPosToGlobal(m_selectionSection, m_selectionEndInSection);
}

bool LetterCanvasItem::hasSelection() const {
    return m_selectionSection >= 0 &&
           m_selectionStartInSection >= 0 &&
           m_selectionEndInSection >= 0 &&
           m_selectionStartInSection != m_selectionEndInSection;
}

int LetterCanvasItem::countLogicalBodyLines() const {
    QString trimmedBody = m_body;
    while (trimmedBody.endsWith('\n')) {
        trimmedBody.chop(1);
    }
    return trimmedBody.isEmpty() ? 0 : trimmedBody.count('\n') + 1;
}

// Wrap body text into visual lines based on glyph width (word-wrap)
QVector<QPair<QString, int>> LetterCanvasItem::wrapBodyText() const {
    QVector<QPair<QString, int>> visualLines;
    if (!m_backend || !m_backend->isLoaded()) {
        for (int i = 0; i < BODY_LINES; i++) {
            visualLines.append(qMakePair(QString(""), 0));
        }
        return visualLines;
    }

    const FontLoader& font = m_backend->font();
    int pos = 0;

    for (int lineIdx = 0; lineIdx < BODY_LINES && pos <= m_body.length(); lineIdx++) {
        QString lineText;
        int lineStartPos = pos;
        int lineWidth = 0;
        int lastSpacePos = -1;
        int lastSpaceBodyPos = -1;

        while (pos < m_body.length()) {
            QChar ch = m_body[pos];

            if (ch == '\n') {
                pos++;
                break;
            }

            int charWidth = font.charWidth(ch) + GLYPH_SPACING;

            if (lineWidth + charWidth > MAX_LINE_WIDTH && !lineText.isEmpty()) {
                if (lastSpacePos > 0) {
                    lineText = lineText.left(lastSpacePos - 1);
                    pos = lastSpaceBodyPos;
                }
                break;
            }

            lineText += ch;
            lineWidth += charWidth;

            if (ch == ' ') {
                lastSpacePos = lineText.length();
                lastSpaceBodyPos = pos + 1;
            }

            pos++;
        }

        visualLines.append(qMakePair(lineText, lineStartPos));
    }

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

        QString& section = sectionText(m_currentSection);

        if (m_currentSection == 0) {
            // Header validation
            int recipientStart = m_backend->recipientNameStart();
            int recipientEnd = m_backend->recipientNameEnd();
            int recipientLen = (recipientStart >= 0 && recipientEnd >= 0) ? (recipientEnd - recipientStart) : 0;

            int headerLenWithoutName = m_header.length() - recipientLen;
            if (headerLenWithoutName >= MAX_HEADER_CHARS) return;

            // Check pixel width limit
            QString testHeader = m_header;
            testHeader.insert(m_cursorPosInSection, ch);
            int testWidth = 0;
            const FontLoader& font = m_backend->font();
            for (int i = 0; i < testHeader.length(); i++) {
                if (recipientStart >= 0 && recipientEnd >= 0 &&
                    i >= recipientStart && i < recipientEnd + 1) {
                    if (i == recipientStart) testWidth += NAME_TOKEN_WIDTH;
                    continue;
                }
                testWidth += font.charWidth(testHeader[i]) + GLYPH_SPACING;
            }
            if (testWidth > MAX_LINE_WIDTH) return;

            // Protect recipient name
            if (recipientStart >= 0 && recipientEnd >= 0 &&
                m_cursorPosInSection > recipientStart && m_cursorPosInSection < recipientEnd) {
                return;
            }

            int insertPos = m_cursorPosInSection;
            m_header.insert(m_cursorPosInSection, ch);
            m_cursorPosInSection++;

            // Shift recipient name position if inserted before it
            if (recipientStart >= 0 && recipientEnd >= 0 && insertPos <= recipientStart) {
                m_backend->setRecipientNameStart(recipientStart + 1);
                m_backend->setRecipientNameEnd(recipientEnd + 1);
            }
        } else if (m_currentSection == 1) {
            // Body validation
            if (m_body.length() >= MAX_BODY_CHARS) return;

            // Find current logical line bounds
            int lineStart = m_cursorPosInSection;
            while (lineStart > 0 && m_body[lineStart - 1] != '\n') {
                lineStart--;
            }
            int lineEnd = m_cursorPosInSection;
            while (lineEnd < m_body.length() && m_body[lineEnd] != '\n') {
                lineEnd++;
            }

            // Check width for lines with explicit newlines
            QString currentLine = m_body.mid(lineStart, lineEnd - lineStart);
            currentLine.insert(m_cursorPosInSection - lineStart, ch);

            const FontLoader& font = m_backend->font();
            int lineWidth = 0;
            for (const QChar& c : currentLine) {
                lineWidth += font.charWidth(c) + GLYPH_SPACING;
            }

            bool hasExplicitNewline = (lineEnd < m_body.length() && m_body[lineEnd] == '\n');
            if (hasExplicitNewline && lineWidth > MAX_LINE_WIDTH) {
                return;
            }

            m_body.insert(m_cursorPosInSection, ch);
            m_cursorPosInSection++;

            // Check visual line overflow
            auto lines = wrapBodyText();
            int lastLineEnd = lines[BODY_LINES - 1].second + lines[BODY_LINES - 1].first.length();
            if (lastLineEnd < m_body.length()) {
                // Revert
                m_cursorPosInSection--;
                m_body.remove(m_cursorPosInSection, 1);
                emit cursorPositionChanged();
                update();
                return;
            }
        } else {
            // Footer validation
            if (m_footer.length() >= MAX_FOOTER_CHARS) return;

            QString testFooter = m_footer;
            testFooter.insert(m_cursorPosInSection, ch);
            int testWidth = 0;
            const FontLoader& font = m_backend->font();
            for (const QChar& c : testFooter) {
                testWidth += font.charWidth(c) + GLYPH_SPACING;
            }
            if (testWidth > LetterConstants::MAX_FOOTER_WIDTH) return;

            m_footer.insert(m_cursorPosInSection, ch);
            m_cursorPosInSection++;
        }

        m_cursorVisible = true;
        emit textChanged();
        emit cursorPositionChanged();
        update();
    }
}

void LetterCanvasItem::backspace() {
    if (hasSelection()) {
        deleteSelection();
        return;
    }

    if (m_cursorPosInSection > 0) {
        // Header protection for recipient name
        if (m_currentSection == 0 && m_backend) {
            int recipientStart = m_backend->recipientNameStart();
            int recipientEnd = m_backend->recipientNameEnd();
            int deletePos = m_cursorPosInSection - 1;

            if (recipientStart >= 0 && recipientEnd >= 0 &&
                deletePos >= recipientStart && deletePos < recipientEnd) {
                return;
            }

            if (recipientStart >= 0 && deletePos < recipientStart) {
                m_backend->setRecipientNameStart(recipientStart - 1);
                m_backend->setRecipientNameEnd(recipientEnd - 1);
            }
        }

        QString& section = sectionText(m_currentSection);
        section.remove(m_cursorPosInSection - 1, 1);
        m_cursorPosInSection--;

        m_cursorVisible = true;
        emit textChanged();
        emit cursorPositionChanged();
        update();
    }
    // At start of section - don't cross boundaries
}

void LetterCanvasItem::deleteChar() {
    if (hasSelection()) {
        deleteSelection();
        return;
    }

    QString& section = sectionText(m_currentSection);

    if (m_cursorPosInSection < section.length()) {
        // Header protection for recipient name
        if (m_currentSection == 0 && m_backend) {
            int recipientStart = m_backend->recipientNameStart();
            int recipientEnd = m_backend->recipientNameEnd();

            if (recipientStart >= 0 && recipientEnd >= 0 &&
                m_cursorPosInSection >= recipientStart && m_cursorPosInSection < recipientEnd) {
                return;
            }

            if (recipientStart >= 0 && m_cursorPosInSection < recipientStart) {
                m_backend->setRecipientNameStart(recipientStart - 1);
                m_backend->setRecipientNameEnd(recipientEnd - 1);
            }
        }

        section.remove(m_cursorPosInSection, 1);

        m_cursorVisible = true;
        emit textChanged();
        emit cursorPositionChanged();
        update();
    }
    // At end of section - don't cross boundaries
}

void LetterCanvasItem::moveCursorLeft() {
    if (hasSelection()) {
        int start = qMin(m_selectionStartInSection, m_selectionEndInSection);
        m_cursorPosInSection = start;
        clearSelection();
        m_cursorVisible = true;
        emit cursorPositionChanged();
        update();
        return;
    }

    if (m_cursorPosInSection > 0) {
        m_cursorPosInSection--;

        // Skip recipient name in header
        if (m_currentSection == 0 && m_backend) {
            int recipientStart = m_backend->recipientNameStart();
            int recipientEnd = m_backend->recipientNameEnd();
            if (recipientStart >= 0 && recipientEnd >= 0 &&
                m_cursorPosInSection > recipientStart &&
                m_cursorPosInSection < recipientEnd) {
                m_cursorPosInSection = recipientStart;
            }
        }
    } else if (m_currentSection > 0) {
        // Move to previous section
        m_currentSection--;
        m_cursorPosInSection = sectionText(m_currentSection).length();

        // Skip recipient name at end of header
        if (m_currentSection == 0 && m_backend) {
            int recipientEnd = m_backend->recipientNameEnd();
            int recipientStart = m_backend->recipientNameStart();
            if (recipientEnd >= 0 && recipientStart >= 0 && m_cursorPosInSection == recipientEnd) {
                m_cursorPosInSection = recipientStart;
            }
        }
        emit currentSectionChanged();
    }

    m_cursorVisible = true;
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::moveCursorRight() {
    if (hasSelection()) {
        int end = qMax(m_selectionStartInSection, m_selectionEndInSection);
        m_cursorPosInSection = end;
        clearSelection();
        m_cursorVisible = true;
        emit cursorPositionChanged();
        update();
        return;
    }

    QString& section = sectionText(m_currentSection);

    if (m_cursorPosInSection < section.length()) {
        m_cursorPosInSection++;

        // Skip recipient name in header
        if (m_currentSection == 0 && m_backend) {
            int recipientStart = m_backend->recipientNameStart();
            int recipientEnd = m_backend->recipientNameEnd();
            if (recipientStart >= 0 && recipientEnd >= 0 &&
                m_cursorPosInSection > recipientStart &&
                m_cursorPosInSection < recipientEnd) {
                m_cursorPosInSection = recipientEnd;
            }
        }
    } else if (m_currentSection < 2) {
        // Move to next section
        m_currentSection++;
        m_cursorPosInSection = 0;
        emit currentSectionChanged();
    }

    m_cursorVisible = true;
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::moveCursorUp() {
    if (hasSelection()) {
        clearSelection();
    }

    if (m_currentSection == 0) {
        // In header - nowhere to go
        return;
    } else if (m_currentSection == 2) {
        // In footer - move to last line of body
        auto wrappedLines = wrapBodyText();
        int lastLineIdx = BODY_LINES - 1;
        for (int i = BODY_LINES - 1; i >= 0; i--) {
            if (!wrappedLines[i].first.isEmpty() || i == 0) {
                lastLineIdx = i;
                break;
            }
        }
        m_currentSection = 1;
        int lineStart = wrappedLines[lastLineIdx].second;
        m_cursorPosInSection = lineStart + qMin(m_cursorPosInSection, wrappedLines[lastLineIdx].first.length());
        emit currentSectionChanged();
    } else {
        // In body
        auto wrappedLines = wrapBodyText();
        int currentLine = -1;
        int posInLine = 0;

        for (int i = 0; i < BODY_LINES; i++) {
            int lineStart = wrappedLines[i].second;
            int lineEnd = lineStart + wrappedLines[i].first.length();
            if (m_cursorPosInSection >= lineStart && m_cursorPosInSection <= lineEnd) {
                currentLine = i;
                posInLine = m_cursorPosInSection - lineStart;
                break;
            }
        }

        if (currentLine <= 0) {
            // Move to header
            m_currentSection = 0;
            m_cursorPosInSection = qMin(posInLine, m_header.length());

            // Skip recipient name
            if (m_backend) {
                int recipientStart = m_backend->recipientNameStart();
                int recipientEnd = m_backend->recipientNameEnd();
                if (recipientStart >= 0 && recipientEnd >= 0 &&
                    m_cursorPosInSection > recipientStart &&
                    m_cursorPosInSection < recipientEnd) {
                    m_cursorPosInSection = recipientEnd;
                }
            }
            emit currentSectionChanged();
        } else {
            // Move to previous line in body
            int prevLineStart = wrappedLines[currentLine - 1].second;
            int prevLineLen = wrappedLines[currentLine - 1].first.length();
            m_cursorPosInSection = prevLineStart + qMin(posInLine, prevLineLen);
        }
    }

    m_cursorVisible = true;
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::moveCursorDown() {
    if (hasSelection()) {
        clearSelection();
    }

    if (m_currentSection == 2) {
        // In footer - nowhere to go
        return;
    } else if (m_currentSection == 0) {
        // In header - move to first line of body
        m_currentSection = 1;
        auto wrappedLines = wrapBodyText();
        m_cursorPosInSection = qMin(m_cursorPosInSection, wrappedLines[0].first.length());
        emit currentSectionChanged();
    } else {
        // In body
        auto wrappedLines = wrapBodyText();
        int currentLine = -1;
        int posInLine = 0;

        for (int i = 0; i < BODY_LINES; i++) {
            int lineStart = wrappedLines[i].second;
            int lineEnd = lineStart + wrappedLines[i].first.length();
            if (m_cursorPosInSection >= lineStart && m_cursorPosInSection <= lineEnd) {
                currentLine = i;
                posInLine = m_cursorPosInSection - lineStart;
                break;
            }
        }

        if (currentLine >= BODY_LINES - 1 || wrappedLines[currentLine + 1].first.isEmpty()) {
            // Move to footer
            m_currentSection = 2;
            m_cursorPosInSection = qMin(posInLine, m_footer.length());
            emit currentSectionChanged();
        } else {
            // Move to next line in body
            int nextLineStart = wrappedLines[currentLine + 1].second;
            int nextLineLen = wrappedLines[currentLine + 1].first.length();
            m_cursorPosInSection = nextLineStart + qMin(posInLine, nextLineLen);
        }
    }

    m_cursorVisible = true;
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::moveCursorHome() {
    if (hasSelection()) {
        clearSelection();
    }

    if (m_currentSection == 1) {
        // In body - move to start of current visual line
        auto wrappedLines = wrapBodyText();
        for (int i = 0; i < BODY_LINES; i++) {
            int lineStart = wrappedLines[i].second;
            int lineEnd = lineStart + wrappedLines[i].first.length();
            if (m_cursorPosInSection >= lineStart && m_cursorPosInSection <= lineEnd) {
                m_cursorPosInSection = lineStart;
                break;
            }
        }
    } else {
        // Header or footer - move to start
        m_cursorPosInSection = 0;
    }

    m_cursorVisible = true;
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::moveCursorEnd() {
    if (hasSelection()) {
        clearSelection();
    }

    if (m_currentSection == 1) {
        // In body - move to end of current visual line
        auto wrappedLines = wrapBodyText();
        for (int i = 0; i < BODY_LINES; i++) {
            int lineStart = wrappedLines[i].second;
            int lineEnd = lineStart + wrappedLines[i].first.length();
            if (m_cursorPosInSection >= lineStart && m_cursorPosInSection <= lineEnd) {
                m_cursorPosInSection = lineEnd;
                break;
            }
        }
    } else if (m_currentSection == 0) {
        // Header - move to end, but skip recipient name
        m_cursorPosInSection = m_header.length();
        if (m_backend) {
            int recipientEnd = m_backend->recipientNameEnd();
            int recipientStart = m_backend->recipientNameStart();
            if (recipientEnd >= 0 && recipientStart >= 0 && m_cursorPosInSection == recipientEnd) {
                m_cursorPosInSection = recipientStart;
            }
        }
    } else {
        // Footer - move to end
        m_cursorPosInSection = m_footer.length();
    }

    m_cursorVisible = true;
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::newLine() {
    if (m_currentSection == 0) {
        // In header - move to body
        m_currentSection = 1;
        m_cursorPosInSection = 0;
        m_cursorVisible = true;
        emit currentSectionChanged();
        emit cursorPositionChanged();
        update();
        return;
    }

    if (m_currentSection == 2) {
        // In footer - do nothing
        return;
    }

    // In body
    int logicalLines = countLogicalBodyLines();
    if (logicalLines >= BODY_LINES) {
        // Already at max - move to footer
        m_currentSection = 2;
        m_cursorPosInSection = 0;
        m_cursorVisible = true;
        emit currentSectionChanged();
        emit cursorPositionChanged();
        update();
        return;
    }

    // Check if on last visual line
    auto wrappedLines = wrapBodyText();
    int currentLine = -1;
    for (int i = 0; i < BODY_LINES; i++) {
        int lineStart = wrappedLines[i].second;
        int lineEnd = lineStart + wrappedLines[i].first.length();
        if (m_cursorPosInSection >= lineStart && m_cursorPosInSection <= lineEnd) {
            currentLine = i;
            break;
        }
    }

    if (currentLine >= BODY_LINES - 1) {
        // On last line - move to footer
        m_currentSection = 2;
        m_cursorPosInSection = 0;
        m_cursorVisible = true;
        emit currentSectionChanged();
        emit cursorPositionChanged();
        update();
        return;
    }

    // Check width of current line before adding newline
    if (m_backend && m_backend->isLoaded()) {
        int lineStart = m_cursorPosInSection;
        while (lineStart > 0 && m_body[lineStart - 1] != '\n') {
            lineStart--;
        }
        QString currentLineText = m_body.mid(lineStart, m_cursorPosInSection - lineStart);
        const FontLoader& font = m_backend->font();
        int lineWidth = 0;
        for (const QChar& c : currentLineText) {
            lineWidth += font.charWidth(c) + GLYPH_SPACING;
        }
        if (lineWidth > MAX_LINE_WIDTH) {
            return;
        }
    }

    m_body.insert(m_cursorPosInSection, '\n');
    m_cursorPosInSection++;
    m_cursorVisible = true;
    emit textChanged();
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::clearText() {
    setLetterContent("", "", "");
}

void LetterCanvasItem::setLetterContent(const QString& header, const QString& body, const QString& footer) {
    m_header = header.left(MAX_HEADER_CHARS);
    m_body = body.left(MAX_BODY_CHARS);
    m_footer = footer.left(MAX_FOOTER_CHARS);

    m_currentSection = 0;
    m_cursorPosInSection = 0;
    m_cursorVisible = true;
    clearSelection();

    emit textChanged();
    emit cursorPositionChanged();
    emit currentSectionChanged();
    update();
}

int LetterCanvasItem::calculateHeaderWidth(const QString& newRecipientName) const {
    if (!m_backend || !m_backend->isLoaded()) return 0;

    int recipientStart = m_backend->recipientNameStart();
    int recipientEnd = m_backend->recipientNameEnd();

    int totalWidth = 0;
    const FontLoader& font = m_backend->font();
    bool hasName = (recipientStart >= 0 && recipientEnd >= 0 && recipientEnd <= m_header.length());

    for (int i = 0; i < m_header.length(); i++) {
        if (hasName && i >= recipientStart && i < recipientEnd) {
            if (i == recipientStart) totalWidth += NAME_TOKEN_WIDTH;
            continue;
        }
        totalWidth += font.charWidth(m_header[i]) + GLYPH_SPACING;
    }

    return totalWidth;
}

void LetterCanvasItem::charPosFromPoint(qreal x, qreal y, int& outSection, int& outPosInSection) const {
    if (!m_backend || !m_backend->isLoaded()) {
        outSection = 0;
        outPosInSection = 0;
        return;
    }

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

    // Determine section from Y coordinate
    if (localY >= HEADER_TOP && localY < HEADER_TOP + LINE_HEIGHT) {
        // Header
        outSection = 0;
        outPosInSection = findCharPosAtX(m_header, localX, HEADER_LEFT, font);

        // Skip recipient name
        int recipientStart = m_backend->recipientNameStart();
        int recipientEnd = m_backend->recipientNameEnd();
        if (recipientStart >= 0 && recipientEnd >= 0 &&
            outPosInSection > recipientStart && outPosInSection < recipientEnd) {
            if (outPosInSection - recipientStart < recipientEnd - outPosInSection) {
                outPosInSection = recipientStart;
            } else {
                outPosInSection = recipientEnd;
            }
        }
    } else if (localY >= BODY_TOP && localY < BODY_TOP + BODY_LINES * LINE_HEIGHT) {
        // Body
        outSection = 1;
        int lineIdx = (localY - BODY_TOP) / LINE_HEIGHT;
        lineIdx = qBound(0, lineIdx, BODY_LINES - 1);

        auto wrappedLines = wrapBodyText();
        QString lineText = wrappedLines[lineIdx].first;
        int lineStartInBody = wrappedLines[lineIdx].second;
        int charInLine = findCharPosAtX(lineText, localX, BODY_LEFT, font);
        outPosInSection = lineStartInBody + charInLine;
    } else if (localY >= FOOTER_TOP && localY < FOOTER_TOP + LINE_HEIGHT) {
        // Footer
        outSection = 2;
        int footerWidth = 0;
        for (const QChar& c : m_footer) {
            footerWidth += font.charWidth(c) + GLYPH_SPACING;
        }
        int footerLeft = FOOTER_RIGHT - footerWidth;
        outPosInSection = findCharPosAtX(m_footer, localX, footerLeft, font);
    } else {
        // Default to header
        outSection = 0;
        outPosInSection = 0;
    }
}

void LetterCanvasItem::handleClick(qreal x, qreal y) {
    if (!m_backend || !m_backend->isLoaded()) return;

    int newSection, newPosInSection;
    charPosFromPoint(x, y, newSection, newPosInSection);

    // Check for recipient name click
    if (newSection == 0 && m_backend) {
        int recipientStart = m_backend->recipientNameStart();
        int recipientEnd = m_backend->recipientNameEnd();
        if (recipientStart >= 0 && recipientEnd >= 0) {
            qreal scaleX = width() / 256.0;
            qreal scaleY = height() / 192.0;
            qreal scale = qMin(scaleX, scaleY);
            int scaledW = static_cast<int>(256 * scale);
            int offsetX = (width() - scaledW) / 2;
            int localX = static_cast<int>((x - offsetX) / scale);

            const FontLoader& font = m_backend->font();
            int xPos = HEADER_LEFT;
            for (int i = 0; i < recipientStart && i < m_header.length(); i++) {
                xPos += font.charWidth(m_header[i]) + GLYPH_SPACING;
            }
            int nameEndX = xPos + NAME_TOKEN_WIDTH;

            if (localX >= xPos && localX <= nameEndX) {
                emit recipientNameClicked();
                return;
            }
        }
    }

    clearSelection();
    m_currentSection = newSection;
    m_cursorPosInSection = newPosInSection;
    m_cursorVisible = true;
    emit currentSectionChanged();
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::startSelection(qreal x, qreal y) {
    if (!m_backend || !m_backend->isLoaded()) return;

    int section, posInSection;
    charPosFromPoint(x, y, section, posInSection);

    // Skip recipient name
    if (section == 0 && m_backend) {
        int recipientStart = m_backend->recipientNameStart();
        int recipientEnd = m_backend->recipientNameEnd();
        if (recipientStart >= 0 && recipientEnd >= 0 &&
            posInSection > recipientStart && posInSection < recipientEnd) {
            return;
        }
    }

    m_selectionSection = section;
    m_selectionAnchorInSection = posInSection;
    m_selectionStartInSection = posInSection;
    m_selectionEndInSection = posInSection;
    m_currentSection = section;
    m_cursorPosInSection = posInSection;

    m_cursorVisible = true;
    emit selectionChanged();
    emit cursorPositionChanged();
    emit currentSectionChanged();
    update();
}

void LetterCanvasItem::updateSelection(qreal x, qreal y) {
    if (m_selectionAnchorInSection < 0 || m_selectionSection < 0) return;
    if (!m_backend || !m_backend->isLoaded()) return;

    int section, posInSection;
    charPosFromPoint(x, y, section, posInSection);

    // Constrain to same section as anchor
    if (section != m_selectionSection) {
        const QString& sectionStr = sectionText(m_selectionSection);
        if (section > m_selectionSection) {
            posInSection = sectionStr.length();
        } else {
            posInSection = 0;
        }
    }

    // Skip recipient name in header
    if (m_selectionSection == 0 && m_backend) {
        int recipientStart = m_backend->recipientNameStart();
        int recipientEnd = m_backend->recipientNameEnd();
        if (recipientStart >= 0 && recipientEnd >= 0) {
            if (posInSection > recipientStart && posInSection < recipientEnd) {
                if (m_selectionAnchorInSection <= recipientStart) {
                    posInSection = recipientEnd;
                } else {
                    posInSection = recipientStart;
                }
            }
        }
    }

    // Update selection range
    if (posInSection < m_selectionAnchorInSection) {
        m_selectionStartInSection = posInSection;
        m_selectionEndInSection = m_selectionAnchorInSection;
    } else {
        m_selectionStartInSection = m_selectionAnchorInSection;
        m_selectionEndInSection = posInSection;
    }

    m_cursorPosInSection = posInSection;

    emit selectionChanged();
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::clearSelection() {
    if (m_selectionSection >= 0 || m_selectionStartInSection >= 0) {
        m_selectionSection = -1;
        m_selectionStartInSection = -1;
        m_selectionEndInSection = -1;
        m_selectionAnchorInSection = -1;
        emit selectionChanged();
        update();
    }
}

void LetterCanvasItem::deleteSelection() {
    if (!hasSelection()) return;

    int start = qMin(m_selectionStartInSection, m_selectionEndInSection);
    int end = qMax(m_selectionStartInSection, m_selectionEndInSection);

    // Handle recipient name protection in header
    if (m_selectionSection == 0 && m_backend) {
        int recipientStart = m_backend->recipientNameStart();
        int recipientEnd = m_backend->recipientNameEnd();
        if (recipientStart >= 0 && recipientEnd >= 0) {
            if (start < recipientEnd && end > recipientStart) {
                clearSelection();
                return;
            }
            if (end <= recipientStart) {
                int deleteLen = end - start;
                m_backend->setRecipientNameStart(recipientStart - deleteLen);
                m_backend->setRecipientNameEnd(recipientEnd - deleteLen);
            }
        }
    }

    QString& section = sectionText(m_selectionSection);
    section.remove(start, end - start);

    m_currentSection = m_selectionSection;
    m_cursorPosInSection = start;
    clearSelection();

    m_cursorVisible = true;
    emit textChanged();
    emit cursorPositionChanged();
    emit currentSectionChanged();
    update();
}

void LetterCanvasItem::selectAll() {
    if (!m_backend || !m_backend->isLoaded()) return;

    const QString& section = sectionText(m_currentSection);

    m_selectionSection = m_currentSection;
    m_selectionAnchorInSection = 0;
    m_selectionStartInSection = 0;
    m_selectionEndInSection = section.length();
    m_cursorPosInSection = section.length();

    emit selectionChanged();
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::extendSelectionLeft() {
    if (!m_backend || !m_backend->isLoaded()) return;

    // Initialize selection if needed
    if (m_selectionSection < 0) {
        m_selectionSection = m_currentSection;
        m_selectionAnchorInSection = m_cursorPosInSection;
        m_selectionStartInSection = m_cursorPosInSection;
        m_selectionEndInSection = m_cursorPosInSection;
    }

    if (m_cursorPosInSection > 0) {
        m_cursorPosInSection--;

        // Skip recipient name
        if (m_currentSection == 0 && m_backend) {
            int recipientStart = m_backend->recipientNameStart();
            int recipientEnd = m_backend->recipientNameEnd();
            if (recipientStart >= 0 && recipientEnd >= 0 &&
                m_cursorPosInSection > recipientStart &&
                m_cursorPosInSection < recipientEnd) {
                m_cursorPosInSection = recipientStart;
            }
        }

        // Update selection
        if (m_cursorPosInSection < m_selectionAnchorInSection) {
            m_selectionStartInSection = m_cursorPosInSection;
            m_selectionEndInSection = m_selectionAnchorInSection;
        } else {
            m_selectionStartInSection = m_selectionAnchorInSection;
            m_selectionEndInSection = m_cursorPosInSection;
        }
    }

    emit selectionChanged();
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::extendSelectionRight() {
    if (!m_backend || !m_backend->isLoaded()) return;

    if (m_selectionSection < 0) {
        m_selectionSection = m_currentSection;
        m_selectionAnchorInSection = m_cursorPosInSection;
        m_selectionStartInSection = m_cursorPosInSection;
        m_selectionEndInSection = m_cursorPosInSection;
    }

    const QString& section = sectionText(m_currentSection);

    if (m_cursorPosInSection < section.length()) {
        m_cursorPosInSection++;

        // Skip recipient name
        if (m_currentSection == 0 && m_backend) {
            int recipientStart = m_backend->recipientNameStart();
            int recipientEnd = m_backend->recipientNameEnd();
            if (recipientStart >= 0 && recipientEnd >= 0 &&
                m_cursorPosInSection > recipientStart &&
                m_cursorPosInSection < recipientEnd) {
                m_cursorPosInSection = recipientEnd;
            }
        }

        if (m_cursorPosInSection < m_selectionAnchorInSection) {
            m_selectionStartInSection = m_cursorPosInSection;
            m_selectionEndInSection = m_selectionAnchorInSection;
        } else {
            m_selectionStartInSection = m_selectionAnchorInSection;
            m_selectionEndInSection = m_cursorPosInSection;
        }
    }

    emit selectionChanged();
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::extendSelectionHome() {
    if (!m_backend || !m_backend->isLoaded()) return;

    if (m_selectionSection < 0) {
        m_selectionSection = m_currentSection;
        m_selectionAnchorInSection = m_cursorPosInSection;
        m_selectionStartInSection = m_cursorPosInSection;
        m_selectionEndInSection = m_cursorPosInSection;
    }

    if (m_currentSection == 1) {
        // Body - select to start of visual line
        auto wrappedLines = wrapBodyText();
        for (int i = 0; i < BODY_LINES; i++) {
            int lineStart = wrappedLines[i].second;
            int lineEnd = lineStart + wrappedLines[i].first.length();
            if (m_cursorPosInSection >= lineStart && m_cursorPosInSection <= lineEnd) {
                m_cursorPosInSection = lineStart;
                break;
            }
        }
    } else {
        m_cursorPosInSection = 0;
    }

    if (m_cursorPosInSection < m_selectionAnchorInSection) {
        m_selectionStartInSection = m_cursorPosInSection;
        m_selectionEndInSection = m_selectionAnchorInSection;
    } else {
        m_selectionStartInSection = m_selectionAnchorInSection;
        m_selectionEndInSection = m_cursorPosInSection;
    }

    emit selectionChanged();
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::extendSelectionEnd() {
    if (!m_backend || !m_backend->isLoaded()) return;

    if (m_selectionSection < 0) {
        m_selectionSection = m_currentSection;
        m_selectionAnchorInSection = m_cursorPosInSection;
        m_selectionStartInSection = m_cursorPosInSection;
        m_selectionEndInSection = m_cursorPosInSection;
    }

    if (m_currentSection == 1) {
        // Body - select to end of visual line
        auto wrappedLines = wrapBodyText();
        for (int i = 0; i < BODY_LINES; i++) {
            int lineStart = wrappedLines[i].second;
            int lineEnd = lineStart + wrappedLines[i].first.length();
            if (m_cursorPosInSection >= lineStart && m_cursorPosInSection <= lineEnd) {
                m_cursorPosInSection = lineEnd;
                break;
            }
        }
    } else if (m_currentSection == 0) {
        m_cursorPosInSection = m_header.length();
        if (m_backend) {
            int recipientEnd = m_backend->recipientNameEnd();
            int recipientStart = m_backend->recipientNameStart();
            if (recipientEnd >= 0 && recipientStart >= 0 && m_cursorPosInSection == recipientEnd) {
                m_cursorPosInSection = recipientStart;
            }
        }
    } else {
        m_cursorPosInSection = m_footer.length();
    }

    if (m_cursorPosInSection < m_selectionAnchorInSection) {
        m_selectionStartInSection = m_cursorPosInSection;
        m_selectionEndInSection = m_selectionAnchorInSection;
    } else {
        m_selectionStartInSection = m_selectionAnchorInSection;
        m_selectionEndInSection = m_cursorPosInSection;
    }

    emit selectionChanged();
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::extendSelectionUp() {
    if (!m_backend || !m_backend->isLoaded()) return;

    if (m_selectionSection < 0) {
        m_selectionSection = m_currentSection;
        m_selectionAnchorInSection = m_cursorPosInSection;
        m_selectionStartInSection = m_cursorPosInSection;
        m_selectionEndInSection = m_cursorPosInSection;
    }

    // For header/footer, treat like Home
    if (m_currentSection != 1) {
        m_cursorPosInSection = 0;
    } else {
        // In body - stay within body
        auto wrappedLines = wrapBodyText();
        int currentLine = -1;
        int posInLine = 0;

        for (int i = 0; i < BODY_LINES; i++) {
            int lineStart = wrappedLines[i].second;
            int lineEnd = lineStart + wrappedLines[i].first.length();
            if (m_cursorPosInSection >= lineStart && m_cursorPosInSection <= lineEnd) {
                currentLine = i;
                posInLine = m_cursorPosInSection - lineStart;
                break;
            }
        }

        if (currentLine <= 0) {
            m_cursorPosInSection = 0;
        } else {
            int prevLineStart = wrappedLines[currentLine - 1].second;
            int prevLineLen = wrappedLines[currentLine - 1].first.length();
            m_cursorPosInSection = prevLineStart + qMin(posInLine, prevLineLen);
        }
    }

    if (m_cursorPosInSection < m_selectionAnchorInSection) {
        m_selectionStartInSection = m_cursorPosInSection;
        m_selectionEndInSection = m_selectionAnchorInSection;
    } else {
        m_selectionStartInSection = m_selectionAnchorInSection;
        m_selectionEndInSection = m_cursorPosInSection;
    }

    emit selectionChanged();
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::extendSelectionDown() {
    if (!m_backend || !m_backend->isLoaded()) return;

    if (m_selectionSection < 0) {
        m_selectionSection = m_currentSection;
        m_selectionAnchorInSection = m_cursorPosInSection;
        m_selectionStartInSection = m_cursorPosInSection;
        m_selectionEndInSection = m_cursorPosInSection;
    }

    // For header/footer, treat like End
    if (m_currentSection != 1) {
        const QString& section = sectionText(m_currentSection);
        m_cursorPosInSection = section.length();
    } else {
        // In body - stay within body
        auto wrappedLines = wrapBodyText();
        int currentLine = -1;
        int posInLine = 0;

        for (int i = 0; i < BODY_LINES; i++) {
            int lineStart = wrappedLines[i].second;
            int lineEnd = lineStart + wrappedLines[i].first.length();
            if (m_cursorPosInSection >= lineStart && m_cursorPosInSection <= lineEnd) {
                currentLine = i;
                posInLine = m_cursorPosInSection - lineStart;
                break;
            }
        }

        if (currentLine >= BODY_LINES - 1 || wrappedLines[currentLine + 1].first.isEmpty()) {
            m_cursorPosInSection = m_body.length();
        } else {
            int nextLineStart = wrappedLines[currentLine + 1].second;
            int nextLineLen = wrappedLines[currentLine + 1].first.length();
            m_cursorPosInSection = nextLineStart + qMin(posInLine, nextLineLen);
        }
    }

    if (m_cursorPosInSection < m_selectionAnchorInSection) {
        m_selectionStartInSection = m_cursorPosInSection;
        m_selectionEndInSection = m_selectionAnchorInSection;
    } else {
        m_selectionStartInSection = m_selectionAnchorInSection;
        m_selectionEndInSection = m_cursorPosInSection;
    }

    emit selectionChanged();
    emit cursorPositionChanged();
    update();
}

void LetterCanvasItem::copySelection() {
    if (!hasSelection()) return;

    int start = qMin(m_selectionStartInSection, m_selectionEndInSection);
    int end = qMax(m_selectionStartInSection, m_selectionEndInSection);
    QString selectedText = sectionText(m_selectionSection).mid(start, end - start);

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

    if (hasSelection()) {
        deleteSelection();
    }

    int startSection = m_currentSection;

    for (const QChar& ch : clipboardText) {
        // Stop if section changed
        if (m_currentSection != startSection) {
            break;
        }

        if (ch == '\n') {
            // Only allow newlines in body
            if (startSection == 1) {
                if (countLogicalBodyLines() >= BODY_LINES) {
                    break;
                }
                newLine();
            }
        } else if (ch.isPrint() && ch.unicode() < 128) {
            insertChar(QString(ch));
        }
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

    qreal scaleX = width() / 256.0;
    qreal scaleY = height() / 192.0;
    qreal scale = qMin(scaleX, scaleY);

    int scaledW = static_cast<int>(256 * scale);
    int scaledH = static_cast<int>(192 * scale);
    int offsetX = (width() - scaledW) / 2;
    int offsetY = (height() - scaledH) / 2;

    painter->translate(offsetX, offsetY);
    painter->scale(scale, scale);

    if (m_backend && m_backend->isLoaded()) {
        QImage paperImage = m_backend->getPaperImage(m_backend->currentPaper());
        if (!paperImage.isNull()) {
            painter->drawImage(0, 0, paperImage);
        }
    }

    renderText(painter);
}

void LetterCanvasItem::renderText(QPainter* painter) {
    if (!m_backend || !m_backend->isLoaded()) return;

    const FontLoader& font = m_backend->font();
    int paperIdx = m_backend->currentPaper();
    QColor textColor = m_backend->stationery().getTextColor(paperIdx);
    QColor recipientColor = m_backend->stationery().getRecipientColor(paperIdx);
    QColor selectionColor(56, 189, 248, 100);

    // Header
    int headerCursorCol = (m_currentSection == 0 && m_cursorVisible) ? m_cursorPosInSection : -1;
    int headerSelStart = -1, headerSelEnd = -1;
    if (m_selectionSection == 0 && hasSelection()) {
        headerSelStart = m_selectionStartInSection;
        headerSelEnd = m_selectionEndInSection;
    }
    renderLineWithRecipient(painter, m_header, HEADER_LEFT, HEADER_TOP,
                           headerCursorCol, font, textColor, recipientColor,
                           m_backend->recipientNameStart(),
                           m_backend->recipientNameEnd(),
                           headerSelStart, headerSelEnd, selectionColor);

    // Body
    auto wrappedLines = wrapBodyText();
    bool cursorDrawn = false;

    for (int i = 0; i < BODY_LINES && i < wrappedLines.size(); i++) {
        QString lineText = wrappedLines[i].first;
        int lineStartInBody = wrappedLines[i].second;
        int lineEndInBody = lineStartInBody + lineText.length();

        int lineCursorCol = -1;
        if (!cursorDrawn && m_currentSection == 1 && m_cursorVisible &&
            m_cursorPosInSection >= lineStartInBody &&
            m_cursorPosInSection <= lineEndInBody) {
            lineCursorCol = m_cursorPosInSection - lineStartInBody;
            cursorDrawn = true;
        }

        int lineSelStart = -1, lineSelEnd = -1;
        if (m_selectionSection == 1 && hasSelection()) {
            if (m_selectionEndInSection > lineStartInBody &&
                m_selectionStartInSection < lineEndInBody) {
                lineSelStart = qMax(0, m_selectionStartInSection - lineStartInBody);
                lineSelEnd = qMin(lineText.length(), m_selectionEndInSection - lineStartInBody);
            }
        }

        renderLine(painter, lineText, BODY_LEFT, BODY_TOP + i * LINE_HEIGHT,
                  lineCursorCol, font, false, textColor,
                  lineSelStart, lineSelEnd, selectionColor);
    }

    // Footer
    int footerCursorCol = (m_currentSection == 2 && m_cursorVisible) ? m_cursorPosInSection : -1;
    int footerSelStart = -1, footerSelEnd = -1;
    if (m_selectionSection == 2 && hasSelection()) {
        footerSelStart = m_selectionStartInSection;
        footerSelEnd = m_selectionEndInSection;
    }
    renderLine(painter, m_footer, FOOTER_RIGHT, FOOTER_TOP,
               footerCursorCol, font, true, textColor,
               footerSelStart, footerSelEnd, selectionColor);
}

void LetterCanvasItem::renderLine(QPainter* painter, const QString& text, int x, int y,
                                   int cursorCol, const FontLoader& font, bool rightAlign,
                                   const QColor& textColor, int selStart, int selEnd,
                                   const QColor& selectionColor) {
    int textWidth = 0;
    for (const QChar& c : text) {
        textWidth += font.charWidth(c) + GLYPH_SPACING;
    }

    int startX = rightAlign ? (x - textWidth) : x;
    int currentX = startX;

    // Draw selection background
    if (selStart >= 0 && selEnd > selStart) {
        int selStartX = startX;
        for (int i = 0; i < selStart && i < text.length(); i++) {
            selStartX += font.charWidth(text[i]) + GLYPH_SPACING;
        }
        int selEndX = selStartX;
        for (int i = selStart; i < selEnd && i < text.length(); i++) {
            selEndX += font.charWidth(text[i]) + GLYPH_SPACING;
        }
        painter->fillRect(selStartX, y, selEndX - selStartX, LINE_HEIGHT, selectionColor);
    }

    // Draw text
    for (int i = 0; i < text.length(); i++) {
        QImage glyph = font.getColoredGlyph(text[i], textColor);
        if (!glyph.isNull()) {
            painter->drawImage(currentX, y, glyph);
        }
        currentX += font.charWidth(text[i]) + GLYPH_SPACING;
    }

    // Draw cursor
    if (cursorCol >= 0 && cursorCol <= text.length()) {
        int cursorX = startX;
        for (int i = 0; i < cursorCol && i < text.length(); i++) {
            cursorX += font.charWidth(text[i]) + GLYPH_SPACING;
        }
        painter->fillRect(cursorX, y, 1, LINE_HEIGHT, textColor);
    }
}

void LetterCanvasItem::renderLineWithRecipient(QPainter* painter, const QString& text, int x, int y,
                                                int cursorCol, const FontLoader& font,
                                                const QColor& textColor, const QColor& recipientColor,
                                                int recipientStart, int recipientEnd,
                                                int selStart, int selEnd,
                                                const QColor& selectionColor) {
    int currentX = x;
    bool hasRecipient = (recipientStart >= 0 && recipientEnd >= 0 && recipientEnd <= text.length());

    // Calculate selection positions
    int selStartX = -1, selEndX = -1;
    if (selStart >= 0 && selEnd > selStart) {
        selStartX = x;
        for (int i = 0; i < selStart && i < text.length(); i++) {
            if (hasRecipient && i >= recipientStart && i < recipientEnd) {
                if (i == recipientStart) selStartX += NAME_TOKEN_WIDTH;
            } else {
                selStartX += font.charWidth(text[i]) + GLYPH_SPACING;
            }
        }
        selEndX = selStartX;
        for (int i = selStart; i < selEnd && i < text.length(); i++) {
            if (hasRecipient && i >= recipientStart && i < recipientEnd) {
                if (i == selStart || i == recipientStart) selEndX += NAME_TOKEN_WIDTH;
            } else {
                selEndX += font.charWidth(text[i]) + GLYPH_SPACING;
            }
        }
        painter->fillRect(selStartX, y, selEndX - selStartX, LINE_HEIGHT, selectionColor);
    }

    // Draw text with recipient name in different color
    for (int i = 0; i < text.length(); i++) {
        QColor color = textColor;
        if (hasRecipient && i >= recipientStart && i < recipientEnd) {
            color = recipientColor;
        }
        QImage glyph = font.getColoredGlyph(text[i], color);
        if (!glyph.isNull()) {
            painter->drawImage(currentX, y, glyph);
        }
        currentX += font.charWidth(text[i]) + GLYPH_SPACING;
    }

    // Draw cursor
    if (cursorCol >= 0 && cursorCol <= text.length()) {
        int cursorX = x;
        for (int i = 0; i < cursorCol && i < text.length(); i++) {
            if (hasRecipient && i >= recipientStart && i < recipientEnd) {
                if (i == recipientStart) cursorX += NAME_TOKEN_WIDTH;
            } else {
                cursorX += font.charWidth(text[i]) + GLYPH_SPACING;
            }
        }
        painter->fillRect(cursorX, y, 1, LINE_HEIGHT, textColor);
    }
}
