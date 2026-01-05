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
    Q_PROPERTY(QString header READ header NOTIFY textChanged)
    Q_PROPERTY(QString body READ body NOTIFY textChanged)
    Q_PROPERTY(QString footer READ footer NOTIFY textChanged)
    Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
    Q_PROPERTY(int currentSection READ currentSection NOTIFY currentSectionChanged)
    Q_PROPERTY(bool cursorVisible READ cursorVisible NOTIFY cursorVisibleChanged)
    Q_PROPERTY(int selectionStart READ selectionStart NOTIFY selectionChanged)
    Q_PROPERTY(int selectionEnd READ selectionEnd NOTIFY selectionChanged)
    Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY selectionChanged)

public:
    explicit LetterCanvasItem(QQuickItem* parent = nullptr);

    void paint(QPainter* painter) override;

    Backend* backend() const { return m_backend; }
    void setBackend(Backend* backend);

    // Combined text for backward compatibility
    QString text() const;
    void setText(const QString& text);

    // Separate field accessors
    QString header() const { return m_header; }
    QString body() const { return m_body; }
    QString footer() const { return m_footer; }

    // Cursor position (global, for backward compatibility)
    int cursorPosition() const;
    void setCursorPosition(int pos);

    // Current section (0=header, 1=body, 2=footer)
    int currentSection() const { return m_currentSection; }

    bool cursorVisible() const { return m_cursorVisible; }

    // Selection (global positions for backward compatibility)
    int selectionStart() const;
    int selectionEnd() const;
    bool hasSelection() const;

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
    Q_INVOKABLE void handleDoubleClick(qreal x, qreal y);
    Q_INVOKABLE void setLetterContent(const QString& header, const QString& body, const QString& footer);

signals:
    void backendChanged();
    void textChanged();
    void cursorPositionChanged();
    void currentSectionChanged();
    void cursorVisibleChanged();
    void selectionChanged();
    void recipientNameClicked();

private slots:
    void toggleCursor();
    void onPaperChanged();
    void onLoadedChanged();
    void updateBackgroundOffset();

private:
    void renderText(QPainter* painter);
    void renderLine(QPainter* painter, const QString& text, int x, int y,
                    int cursorCol, const FontLoader& font, bool rightAlign,
                    const QColor& textColor, int selStart = -1, int selEnd = -1,
                    const QColor& selectionColor = QColor());
    void renderLineWithRecipient(QPainter* painter, int y,
                                  int cursorVisualPos, const FontLoader& font,
                                  const QColor& textColor, const QColor& recipientColor,
                                  int selStart = -1, int selEnd = -1,
                                  const QColor& selectionColor = QColor());

    // Section text accessor (returns reference for modification)
    QString& sectionText(int section);
    const QString& sectionText(int section) const;

    // Position conversion helpers
    int sectionPosToGlobal(int section, int posInSection) const;
    void globalPosToSection(int globalPos, int& outSection, int& outPosInSection) const;

    // Body helpers
    int countLogicalBodyLines() const;
    QVector<QPair<QString, int>> wrapBodyText() const;

    // Click handling helper
    int findCharPosAtX(const QString& text, int targetX, int startX, const FontLoader& font) const;

    // Convert widget coordinates to section and position
    // outClickedName is set to true if the click was on the recipient name
    void charPosFromPoint(qreal x, qreal y, int& outSection, int& outPosInSection, bool* outClickedName = nullptr) const;

    // Check if current selection includes the visually rendered recipient name
    bool selectionIncludesName() const;

    // Build unified visual glyph list for header (template + name as individual chars)
    QVector<VisualGlyph> buildHeaderVisualGlyphs() const;

    // Convert header visual cursor position to template position
    int headerVisualToTemplatePos(int visualPos) const;

    // Convert header template position to visual position
    int headerTemplateToVisualPos(int templatePos) const;

    Backend* m_backend = nullptr;

    // Separate text storage
    QString m_header;
    QString m_body;
    QString m_footer;

    // Section-aware cursor
    int m_currentSection = 0;      // 0=header, 1=body, 2=footer
    int m_cursorPosInSection = 0;  // Visual position for header (includes name chars), template position for body/footer

    // Section-aware selection (constrained to single section)
    int m_selectionSection = -1;
    int m_selectionStartInSection = -1;
    int m_selectionEndInSection = -1;
    int m_selectionAnchorInSection = -1;

    bool m_cursorVisible = true;
    QTimer m_cursorTimer;

    // Animated cloth background
    QImage m_clothTexture;
    QTimer m_backgroundTimer;
    qreal m_bgOffsetX = 0;
    qreal m_bgOffsetY = 0;

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
