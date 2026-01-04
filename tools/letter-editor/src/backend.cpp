#include "backend.h"
#include "acww_encoding.h"
#include "letterconstants.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QPainter>

// CRC32 lookup table (standard polynomial 0xEDB88320)
static const uint32_t crc32_table[256] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
    0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
    0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
    0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
    0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
    0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
    0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
    0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
    0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
    0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
    0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
    0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
    0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
    0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
    0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
    0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
    0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
    0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
    0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
    0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
    0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
    0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

static uint32_t calculateCRC32(const QByteArray& data) {
    uint32_t crc = 0xFFFFFFFF;
    for (int i = 0; i < data.size(); i++) {
        uint8_t byte = static_cast<uint8_t>(data[i]);
        crc = crc32_table[(crc ^ byte) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFF;
}

// LTR file format constants (EUR/USA)
// First 4 bytes are header (zeros), data starts at 0x04
static const int LTR_FILE_SIZE = 0xF4;        // 244 bytes

// Receiver (To) field - 0x04-0x17 (20 bytes)
static const int LTR_TO_TOWN_ID_OFFSET = 0x04;     // 2 bytes (uint16)
static const int LTR_TO_TOWN_OFFSET = 0x06;        // 8 bytes (town name)
static const int LTR_TO_PLAYER_ID_OFFSET = 0x0E;   // 2 bytes (uint16)
static const int LTR_TO_NAME_OFFSET = 0x10;        // 8 bytes (player name)

// Sender (From) field - 0x1C-0x2F (20 bytes)
static const int LTR_FROM_TOWN_ID_OFFSET = 0x1C;   // 2 bytes (uint16)
static const int LTR_FROM_TOWN_OFFSET = 0x1E;      // 8 bytes (town name)
static const int LTR_FROM_PLAYER_ID_OFFSET = 0x26; // 2 bytes (uint16)
static const int LTR_FROM_NAME_OFFSET = 0x28;      // 8 bytes (player name)

// Letter content
static const int LTR_SUBJECT_OFFSET = 0x34;   // Greeting/subject template
static const int LTR_SUBJECT_SIZE = 0x18;     // 24 bytes
static const int LTR_BODY_OFFSET = 0x4C;      // Letter body
static const int LTR_BODY_SIZE = 0x80;        // 128 bytes
static const int LTR_SIGNATURE_OFFSET = 0xCC; // Signature/ending
static const int LTR_SIGNATURE_SIZE = 0x20;   // 32 bytes

// Flags
static const int LTR_RECEIVER_FLAGS_OFFSET = 0x18;  // 4 bytes
static const int LTR_SENDER_FLAGS_OFFSET = 0x30;    // 4 bytes

// Metadata
static const int LTR_NAME_POS_OFFSET = 0xEC;  // 1 byte - position to insert name in greeting
static const int LTR_PAPER_OFFSET = 0xED;     // 1 byte
static const int LTR_ICON_FLAGS_OFFSET = 0xEE;    // 1 byte
static const int LTR_LETTER_SOURCE_OFFSET = 0xEF; // 1 byte
static const int LTR_ITEM_OFFSET = 0xF0;      // 2 bytes (little-endian)

Backend::Backend(QObject* parent)
    : QObject(parent)
{
}

bool Backend::loadRom(const QUrl& fileUrl) {
    QString path = fileUrl.toLocalFile();

    NDSRom rom;
    if (!rom.load(path.toStdString())) {
        return false;
    }

    if (!m_stationery.load(rom)) {
        return false;
    }

    if (!m_font.load(rom)) {
        return false;
    }

    // Load cloth texture (optional - continue even if it fails)
    m_cloth.load(rom);

    // Build paper names list using actual stationery names
    m_paperNames.clear();
    for (int i = 0; i < m_stationery.count(); i++) {
        m_paperNames.append(m_stationery.getName(i));
    }

    m_loaded = true;
    m_currentPaper = 0;

    emit loadedChanged();
    emit currentPaperChanged();
    emit paperChanged();

    return true;
}

void Backend::setCurrentPaper(int index) {
    if (index >= 0 && index < m_stationery.count() && index != m_currentPaper) {
        m_currentPaper = index;
        writeMetadataToData();  // Sync to m_letterData
        emit currentPaperChanged();
        emit paperChanged();
    }
}

void Backend::setLetterText(const QString& text) {
    if (text != m_letterText) {
        m_letterText = text;

        // Parse and sync to m_letterData
        int firstNewline = text.indexOf('\n');
        int lastNewline = text.lastIndexOf('\n');

        QString greeting, body, signature;
        if (firstNewline < 0) {
            greeting = text;
        } else if (firstNewline == lastNewline) {
            greeting = text.left(firstNewline);
            body = text.mid(firstNewline + 1);
        } else {
            greeting = text.left(firstNewline);
            body = text.mid(firstNewline + 1, lastNewline - firstNewline - 1);
            signature = text.mid(lastNewline + 1);
        }

        writeGreetingToData(greeting);
        writeBodyToData(body);
        writeSignatureToData(signature);

        emit letterTextChanged();
    }
}

void Backend::setRecipientNameStart(int pos) {
    if (pos != m_recipientNameStart) {
        m_recipientNameStart = pos;
        emit recipientNamePositionChanged();
    }
}

void Backend::setRecipientNameEnd(int pos) {
    if (pos != m_recipientNameEnd) {
        m_recipientNameEnd = pos;
        emit recipientNamePositionChanged();
    }
}

void Backend::setRecipientName(const QString& name) {
    if (name != m_recipientName) {
        m_recipientName = name;
        writeRecipientToData();  // Sync to m_letterData
        emit recipientInfoChanged();
    }
}

void Backend::setRecipientTown(const QString& town) {
    if (town != m_recipientTown) {
        m_recipientTown = town;
        writeRecipientToData();  // Sync to m_letterData
        emit recipientInfoChanged();
    }
}

void Backend::setRecipientTownId(int id) {
    if (id != m_recipientTownId) {
        m_recipientTownId = id;
        writeRecipientToData();  // Sync to m_letterData
        emit recipientInfoChanged();
    }
}

void Backend::setRecipientPlayerId(int id) {
    if (id != m_recipientPlayerId) {
        m_recipientPlayerId = id;
        writeRecipientToData();  // Sync to m_letterData
        emit recipientInfoChanged();
    }
}

void Backend::setSenderName(const QString& name) {
    if (name != m_senderName) {
        m_senderName = name;
        writeSenderToData();  // Sync to m_letterData
        emit senderInfoChanged();
    }
}

void Backend::setSenderTown(const QString& town) {
    if (town != m_senderTown) {
        m_senderTown = town;
        writeSenderToData();  // Sync to m_letterData
        emit senderInfoChanged();
    }
}

void Backend::setSenderTownId(int id) {
    if (id != m_senderTownId) {
        m_senderTownId = id;
        writeSenderToData();  // Sync to m_letterData
        emit senderInfoChanged();
    }
}

void Backend::setSenderPlayerId(int id) {
    if (id != m_senderPlayerId) {
        m_senderPlayerId = id;
        writeSenderToData();  // Sync to m_letterData
        emit senderInfoChanged();
    }
}

void Backend::setAttachedItem(int item) {
    uint16_t newItem = static_cast<uint16_t>(item & 0xFFFF);
    if (newItem != m_attachedItem) {
        m_attachedItem = newItem;
        writeMetadataToData();  // Sync to m_letterData
        emit attachedItemChanged();
    }
}

void Backend::setReceiverFlags(int flags) {
    uint32_t newFlags = static_cast<uint32_t>(flags);
    if (newFlags != m_receiverFlags) {
        m_receiverFlags = newFlags;
        writeRecipientToData();  // Sync to m_letterData
        emit letterMetadataChanged();
    }
}

void Backend::setSenderFlags(int flags) {
    uint32_t newFlags = static_cast<uint32_t>(flags);
    if (newFlags != m_senderFlags) {
        m_senderFlags = newFlags;
        writeSenderToData();  // Sync to m_letterData
        emit letterMetadataChanged();
    }
}

void Backend::setNamePosition(int pos) {
    uint8_t newPos = static_cast<uint8_t>(pos & 0xFF);
    if (newPos != m_namePosition) {
        m_namePosition = newPos;
        writeMetadataToData();  // Sync to m_letterData
        emit letterMetadataChanged();
    }
}

void Backend::setLetterIconFlags(int flags) {
    uint8_t newFlags = static_cast<uint8_t>(flags & 0xFF);
    if (newFlags != m_letterIconFlags) {
        m_letterIconFlags = newFlags;
        writeMetadataToData();  // Sync to m_letterData
        emit letterMetadataChanged();
    }
}

void Backend::setLetterSource(int source) {
    uint8_t newSource = static_cast<uint8_t>(source & 0xFF);
    if (newSource != m_letterSource) {
        m_letterSource = newSource;
        writeMetadataToData();  // Sync to m_letterData
        emit letterMetadataChanged();
    }
}

QImage Backend::getPaperImage(int index) const {
    if (index >= 0 && index < m_stationery.count()) {
        return m_stationery.getPaper(index);
    }
    return QImage();
}

QString Backend::findLocalRom() const {
    // ACWW game codes in NDS header (offset 0x0C, 4 bytes)
    // ADME = USA, ADMJ = Japan, ADMP = Europe
    const QStringList validGameCodes = {"ADME", "ADMJ", "ADMP"};

    // Known CRC32 values for ACWW ROMs
    const QMap<QString, uint32_t> validCRC32s = {
        {"ADME", 0xF24508AA},  // USA
        {"ADMJ", 0x00000000},  // Japan (unknown, will skip CRC check)
        {"ADMP", 0x00000000}   // Europe (unknown, will skip CRC check)
    };

    // Get executable directory
    QString exeDir = QCoreApplication::applicationDirPath();
    QDir dir(exeDir);

    // Find all .nds files
    QStringList ndsFiles = dir.entryList(QStringList() << "*.nds", QDir::Files);

    for (const QString& filename : ndsFiles) {
        QString filepath = dir.absoluteFilePath(filename);
        QFile file(filepath);

        if (file.open(QIODevice::ReadOnly)) {
            // Read game code from NDS header at offset 0x0C
            file.seek(0x0C);
            QByteArray gameCode = file.read(4);
            QString code = QString::fromLatin1(gameCode);

            if (validGameCodes.contains(code)) {
                // Game code matches, now verify CRC32
                uint32_t expectedCRC = validCRC32s.value(code);

                if (expectedCRC != 0) {
                    // Read entire file for CRC32 check
                    file.seek(0);
                    QByteArray romData = file.readAll();
                    uint32_t actualCRC = calculateCRC32(romData);
                    file.close();

                    qDebug() << "Found ACWW ROM:" << filename
                             << "Game code:" << code
                             << "CRC32:" << QString::number(actualCRC, 16).toUpper();

                    if (actualCRC == expectedCRC) {
                        qDebug() << "CRC32 verified!";
                        return filepath;
                    } else {
                        qDebug() << "CRC32 mismatch, expected:" << QString::number(expectedCRC, 16).toUpper();
                        // Continue checking other files
                    }
                } else {
                    // No known CRC32 for this region, accept based on game code only
                    file.close();
                    qDebug() << "Found ACWW ROM:" << filename << "Game code:" << code << "(CRC32 not verified)";
                    return filepath;
                }
            } else {
                file.close();
            }
        }
    }

    return QString();
}

bool Backend::importLtr(const QUrl& fileUrl) {
    QString path = fileUrl.toLocalFile();
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open LTR file:" << path;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    if (data.size() != LTR_FILE_SIZE) {
        qDebug() << "Invalid LTR file size:" << data.size() << "expected:" << LTR_FILE_SIZE;
        return false;
    }

    // Store raw bytes as single source of truth
    m_letterData = data;

    // Sync UI state from the raw data
    syncUIFromLetterData();

    // Emit signals for UI update
    emit letterTextChanged();
    emit recipientInfoChanged();
    emit senderInfoChanged();
    emit recipientNamePositionChanged();
    emit currentPaperChanged();
    emit paperChanged();
    emit attachedItemChanged();
    emit letterMetadataChanged();

    qDebug() << "Imported LTR file:" << path;
    return true;
}

bool Backend::exportLtr(const QUrl& fileUrl) const {
    QString path = fileUrl.toLocalFile();

    if (m_letterData.size() < LTR_FILE_SIZE) {
        qDebug() << "Cannot export LTR: No letter data";
        return false;
    }

    // Write m_letterData directly - it's the single source of truth
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to create LTR file:" << path;
        return false;
    }

    file.write(m_letterData);
    file.close();

    qDebug() << "Exported LTR file:" << path;
    return true;
}

QVector<VisualGlyph> Backend::buildHeaderVisualGlyphs(const QString& templateText, int startX, int glyphSpacing) const {
    QVector<VisualGlyph> glyphs;

    if (!m_loaded) {
        return glyphs;
    }

    int namePos = static_cast<int>(m_namePosition);
    bool hasName = !m_recipientName.isEmpty() && namePos >= 0 && namePos <= templateText.length();

    int currentX = startX;

    for (int i = 0; i < templateText.length(); i++) {
        // At namePos, insert each name character as individual glyphs
        // All name glyphs map to the same template position (namePos)
        if (hasName && i == namePos) {
            for (const QChar& c : m_recipientName) {
                int glyphStartX = currentX;
                currentX += m_font.charWidth(c) + glyphSpacing;
                glyphs.append({c, glyphStartX, currentX, true, namePos});
            }
        }

        // Template glyph
        int glyphStartX = currentX;
        int charWidth = m_font.charWidth(templateText[i]) + glyphSpacing;
        currentX += charWidth;
        glyphs.append({templateText[i], glyphStartX, currentX, false, i});
    }

    // Handle name at end of template (namePos == templateText.length())
    if (hasName && namePos >= templateText.length()) {
        for (const QChar& c : m_recipientName) {
            int glyphStartX = currentX;
            currentX += m_font.charWidth(c) + glyphSpacing;
            glyphs.append({c, glyphStartX, currentX, true, namePos});
        }
    }

    return glyphs;
}

bool Backend::exportPng(const QUrl& fileUrl, int scale) const {
    QString path = fileUrl.toLocalFile();

    if (!m_loaded) {
        qDebug() << "Cannot export PNG: ROM not loaded";
        return false;
    }

    // Ensure scale is reasonable
    if (scale < 1) scale = 1;
    if (scale > 4) scale = 4;

    // Create image at scaled resolution
    int width = 256 * scale;
    int height = 192 * scale;
    QImage image(width, height, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);

    // Draw background
    QImage bg = m_stationery.getPaper(m_currentPaper);
    if (!bg.isNull()) {
        painter.drawImage(QRect(0, 0, width, height), bg);
    }

    // Get colors for current stationery
    QColor textColor = m_stationery.getTextColor(m_currentPaper);
    QColor recipientColor = m_stationery.getRecipientColor(m_currentPaper);

    // Parse text sections
    int firstNewline = m_letterText.indexOf('\n');
    int lastNewline = m_letterText.lastIndexOf('\n');

    QString header, body, footer;
    if (firstNewline < 0) {
        header = m_letterText;
    } else if (firstNewline == lastNewline) {
        header = m_letterText.left(firstNewline);
        body = m_letterText.mid(firstNewline + 1);
    } else {
        header = m_letterText.left(firstNewline);
        body = m_letterText.mid(firstNewline + 1, lastNewline - firstNewline - 1);
        footer = m_letterText.mid(lastNewline + 1);
    }

    // Layout constants (scaled)
    const int headerLeft = 48 * scale;
    const int headerTop = 40 * scale;
    const int bodyLeft = 48 * scale;
    const int bodyTop = 64 * scale;
    const int footerRight = 208 * scale;
    const int footerTop = 136 * scale;
    const int lineHeight = 16 * scale;
    const int glyphSpacing = 1 * scale;
    const int maxLineWidth = 150 * scale;

    // Helper lambda to draw text
    auto drawText = [&](const QString& text, int x, int y, const QColor& color, bool rightAlign = false) {
        int drawX = x;

        if (rightAlign) {
            int totalWidth = 0;
            for (const QChar& ch : text) {
                totalWidth += (m_font.charWidth(ch) + 1) * scale;
            }
            drawX = x - totalWidth;
        }

        for (int i = 0; i < text.length(); i++) {
            QChar ch = text[i];
            QImage glyph = m_font.getColoredGlyph(ch, color);

            if (!glyph.isNull()) {
                // Scale the glyph
                QImage scaledGlyph = glyph.scaled(16 * scale, 16 * scale, Qt::IgnoreAspectRatio, Qt::FastTransformation);
                painter.drawImage(drawX, y, scaledGlyph);
                drawX += (m_font.charWidth(ch) + 1) * scale;
            } else {
                drawX += (m_font.charWidth(ch) + 1) * scale;
            }
        }
    };

    // Draw header using shared visual glyph builder
    auto headerGlyphs = buildHeaderVisualGlyphs(header, 0, 1);  // Build at x=0 with spacing=1
    for (const auto& g : headerGlyphs) {
        QColor color = g.isName ? recipientColor : textColor;
        QImage glyph = m_font.getColoredGlyph(g.ch, color);
        if (!glyph.isNull()) {
            QImage scaledGlyph = glyph.scaled(16 * scale, 16 * scale, Qt::IgnoreAspectRatio, Qt::FastTransformation);
            // Offset by headerLeft and scale the x position
            painter.drawImage(headerLeft + g.startX * scale, headerTop, scaledGlyph);
        }
    }

    // Draw body with word wrapping
    int bodyLine = 0;
    int pos = 0;
    while (bodyLine < 4 && pos < body.length()) {
        QString lineText;
        int lineWidth = 0;
        int lastSpacePos = -1;
        int lastSpaceBodyPos = -1;

        while (pos < body.length()) {
            QChar ch = body[pos];

            if (ch == '\n') {
                pos++;
                break;
            }

            int charWidth = (m_font.charWidth(ch) + 1) * scale;

            if (lineWidth + charWidth > maxLineWidth && !lineText.isEmpty()) {
                if (lastSpacePos > 0) {
                    lineText = lineText.left(lastSpacePos - 1);
                    pos = lastSpaceBodyPos;
                }
                break;
            }

            lineText += ch;
            lineWidth += charWidth;
            pos++;

            if (ch == ' ') {
                lastSpacePos = lineText.length();
                lastSpaceBodyPos = pos;
            }
        }

        drawText(lineText, bodyLeft, bodyTop + bodyLine * lineHeight, textColor);
        bodyLine++;
    }

    // Draw footer (right-aligned)
    drawText(footer, footerRight, footerTop, textColor, true);

    painter.end();

    // Save as PNG
    if (!image.save(path, "PNG")) {
        qDebug() << "Failed to save PNG:" << path;
        return false;
    }

    qDebug() << "Exported PNG:" << path << "at" << scale << "x scale";
    return true;
}

// Save file methods

void Backend::setCurrentPlayer(int player) {
    if (player != m_currentPlayer && player >= 0 && player < SaveFile::PLAYER_COUNT) {
        m_currentPlayer = player;
        m_currentSlot = 0;  // Reset slot when player changes
        emit currentPlayerChanged();
        emit currentSlotChanged();
    }
}

void Backend::setCurrentStorageType(int type) {
    if (type != m_currentStorageType && type >= 0 && type <= 2) {
        m_currentStorageType = type;
        m_currentSlot = 0;  // Reset slot when storage type changes
        emit currentStorageTypeChanged();
        emit currentSlotChanged();
    }
}

void Backend::setCurrentSlot(int slot) {
    int maxSlots = m_saveFile.getSlotCount(m_currentStorageType);
    if (slot != m_currentSlot && slot >= 0 && slot < maxSlots) {
        m_currentSlot = slot;
        emit currentSlotChanged();
    }
}

bool Backend::loadSave(const QUrl& fileUrl) {
    QString path = fileUrl.toLocalFile();

    if (!m_saveFile.load(path)) {
        qDebug() << "Backend::loadSave: Failed to load save file:" << path;
        return false;
    }

    // Reset selection state
    m_currentPlayer = 0;
    m_currentStorageType = 0;
    m_currentSlot = 0;

    // Connect save file signals
    connect(&m_saveFile, &SaveFile::modifiedChanged, this, &Backend::saveModifiedChanged);

    // Load slot 1 (index 0) by default BEFORE emitting signals
    // so QML handlers see the loaded letter data
    loadCurrentSlot();

    emit saveLoadedChanged();
    emit currentPlayerChanged();
    emit currentStorageTypeChanged();
    emit currentSlotChanged();

    qDebug() << "Backend::loadSave: Loaded save file:" << path;
    return true;
}

bool Backend::saveSave(const QUrl& fileUrl) {
    QString path = fileUrl.isValid() ? fileUrl.toLocalFile() : QString();

    if (!m_saveFile.save(path)) {
        qDebug() << "Backend::saveSave: Failed to save:" << path;
        return false;
    }

    emit saveModifiedChanged();
    qDebug() << "Backend::saveSave: Saved to:" << (path.isEmpty() ? m_saveFile.filePath() : path);
    return true;
}

void Backend::closeSave() {
    if (!m_saveFile.isLoaded()) {
        return;
    }

    m_saveFile.close();
    m_currentPlayer = 0;
    m_currentStorageType = 0;
    m_currentSlot = 0;

    // Clear letter info
    clearLetter();

    emit saveLoadedChanged();
    emit saveModifiedChanged();
    emit currentPlayerChanged();
    emit currentStorageTypeChanged();
    emit currentSlotChanged();

    qDebug() << "Backend::closeSave: Save file closed";
}

void Backend::loadCurrentSlot() {
    if (!m_saveFile.isLoaded()) {
        qDebug() << "Backend::loadCurrentSlot: No save file loaded";
        return;
    }

    // Get raw letter bytes as single source of truth
    m_letterData = m_saveFile.getRawLetterBytes(m_currentPlayer, m_currentStorageType, m_currentSlot);

    // Sync UI state from the raw data
    syncUIFromLetterData();

    emit letterTextChanged();
    emit recipientInfoChanged();
    emit senderInfoChanged();
    emit recipientNamePositionChanged();
    emit currentPaperChanged();
    emit paperChanged();
    emit attachedItemChanged();

    qDebug() << "Backend::loadCurrentSlot: Loaded player" << m_currentPlayer
             << "storage" << m_currentStorageType << "slot" << m_currentSlot;
}

void Backend::saveCurrentSlot() {
    if (!m_saveFile.isLoaded()) {
        qDebug() << "Backend::saveCurrentSlot: No save file loaded";
        return;
    }

    if (m_letterData.size() < LTR_FILE_SIZE) {
        qDebug() << "Backend::saveCurrentSlot: No letter data to save";
        return;
    }

    // Save m_letterData directly - it's the single source of truth
    m_saveFile.setRawLetterBytes(m_currentPlayer, m_currentStorageType, m_currentSlot, m_letterData);

    qDebug() << "Backend::saveCurrentSlot: Saved to player" << m_currentPlayer
             << "storage" << m_currentStorageType << "slot" << m_currentSlot;
}

QVariantList Backend::getSlotSummaries() const {
    if (!m_saveFile.isLoaded()) {
        return QVariantList();
    }
    return m_saveFile.getSlotSummaries(m_currentPlayer, m_currentStorageType);
}

void Backend::clearLetter() {
    // Reset canonical letter data to empty buffer
    initLetterData();

    // Reset UI state
    m_letterText.clear();
    m_letterHeader.clear();
    m_letterBody.clear();
    m_letterFooter.clear();
    m_recipientName.clear();
    m_recipientTown.clear();
    m_recipientTownId = 0;
    m_recipientPlayerId = 0;
    m_senderName.clear();
    m_senderTown.clear();
    m_senderTownId = 0;
    m_senderPlayerId = 0;
    m_recipientNameStart = -1;
    m_recipientNameEnd = -1;
    m_currentPaper = 0;

    // Reset metadata fields to defaults
    m_receiverFlags = 0;
    m_senderFlags = 0;
    m_namePosition = 0;
    m_letterIconFlags = 0;
    m_letterSource = 0;
    m_attachedItem = 0xFFF1;  // "No item" value

    // Update m_letterData with default metadata (attached item)
    writeMetadataToData();

    emit letterTextChanged();
    emit attachedItemChanged();
    emit recipientInfoChanged();
    emit senderInfoChanged();
    emit recipientNamePositionChanged();
    emit currentPaperChanged();
    emit paperChanged();
}

void Backend::importAddresseeFromSave() {
    if (!m_saveFile.isLoaded()) {
        qDebug() << "Backend::importAddresseeFromSave: No save file loaded";
        return;
    }

    // Get player info from current player in save file
    QString newName = m_saveFile.getPlayerName(m_currentPlayer);
    m_recipientTown = m_saveFile.getPlayerTown(m_currentPlayer);
    m_recipientPlayerId = m_saveFile.getPlayerId(m_currentPlayer);
    m_recipientTownId = m_saveFile.getTownId(m_currentPlayer);

    // Update recipient name - the name is visual only, not stored in header text
    // The canvas renders the name at namePosition during paint
    m_recipientName = newName;

    // Update the name in the raw letter data (recipient field)
    writeRecipientToData();

    // Update recipientNameStart/End for visual rendering purposes
    // These indicate where the name appears visually (at namePosition)
    if (!newName.isEmpty()) {
        m_recipientNameStart = m_namePosition;
        m_recipientNameEnd = m_namePosition + newName.length();
    } else {
        m_recipientNameStart = -1;
        m_recipientNameEnd = -1;
    }

    emit recipientInfoChanged();
    emit recipientNamePositionChanged();

    qDebug() << "Backend::importAddresseeFromSave: Imported"
             << m_recipientName << "from" << m_recipientTown
             << "(Player ID:" << m_recipientPlayerId << ", Town ID:" << m_recipientTownId << ")";
}

bool Backend::playerExists(int player) const {
    if (!m_saveFile.isLoaded()) {
        return false;
    }
    return m_saveFile.playerExists(player);
}

QString Backend::getLetterHex() const {
    // Use m_letterData directly - it's the single source of truth
    const QByteArray& data = m_letterData;

    if (data.size() < LTR_FILE_SIZE) {
        return QStringLiteral("No letter data loaded");
    }

    // Format as hex dump
    QString result;
    result += "Offset    00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F   ASCII\n";
    result += "--------  ------------------------------------------------  ----------------\n";

    for (int i = 0; i < data.size(); i += 16) {
        // Offset
        result += QString("%1  ").arg(i, 8, 16, QChar('0')).toUpper();

        // Hex bytes
        QString hexPart;
        QString asciiPart;
        for (int j = 0; j < 16; j++) {
            if (i + j < data.size()) {
                uint8_t byte = static_cast<uint8_t>(data[i + j]);
                hexPart += QString("%1 ").arg(byte, 2, 16, QChar('0')).toUpper();
                asciiPart += (byte >= 32 && byte < 127) ? QChar(byte) : QChar('.');
            } else {
                hexPart += "   ";
            }
            if (j == 7) hexPart += " ";
        }

        result += hexPart + " " + asciiPart + "\n";
    }

    return result;
}

// Item database access methods

QString Backend::attachedItemName() const {
    if (ItemDatabase::isNoItem(m_attachedItem)) {
        return QStringLiteral("(No item)");
    }
    QString name = ItemDatabase::getItemName(m_attachedItem);
    if (name.isEmpty()) {
        return QString("Unknown (0x%1)").arg(m_attachedItem, 4, 16, QChar('0')).toUpper();
    }
    return name;
}

QString Backend::getItemName(int hexCode) const {
    if (ItemDatabase::isNoItem(static_cast<uint16_t>(hexCode))) {
        return QStringLiteral("(No item)");
    }
    QString name = ItemDatabase::getItemName(static_cast<uint16_t>(hexCode));
    if (name.isEmpty()) {
        return QString("Unknown (0x%1)").arg(hexCode, 4, 16, QChar('0')).toUpper();
    }
    return name;
}

QVariantList Backend::getItemList() const {
    QVariantList result;

    // Add "No item" entry first
    QVariantMap noItem;
    noItem["hex"] = static_cast<int>(ItemDatabase::NO_ITEM);
    noItem["name"] = QStringLiteral("(No item)");
    noItem["display"] = QStringLiteral("FFF1 - (No item)");
    result.append(noItem);

    // Get all items from database
    QList<QPair<uint16_t, QString>> items = ItemDatabase::getAllItems();
    for (const auto& item : items) {
        QVariantMap entry;
        entry["hex"] = static_cast<int>(item.first);
        entry["name"] = item.second;
        QString hexStr = QString("%1").arg(item.first, 4, 16, QChar('0')).toUpper();
        entry["display"] = hexStr + " - " + item.second;
        result.append(entry);
    }

    return result;
}

QStringList Backend::getItemCategories() const {
    return QStringList{
        QStringLiteral("All"),
        QStringLiteral("Stationery"),
        QStringLiteral("Wallpaper"),
        QStringLiteral("Carpet/Flooring"),
        QStringLiteral("Clothing"),
        QStringLiteral("Insects"),
        QStringLiteral("Fish"),
        QStringLiteral("Trash"),
        QStringLiteral("K.K. Songs"),
        QStringLiteral("Tools"),
        QStringLiteral("Umbrellas"),
        QStringLiteral("Hats"),
        QStringLiteral("Hair Accessories"),
        QStringLiteral("Flowers"),
        QStringLiteral("Glasses"),
        QStringLiteral("Facial Accessories"),
        QStringLiteral("Bags of Bells"),
        QStringLiteral("Furniture"),
        QStringLiteral("Paintings"),
        QStringLiteral("Museum Displays"),
        QStringLiteral("Villager Pictures")
    };
}

QVariantList Backend::getItemsByCategory(const QString& categoryName) const {
    QVariantList result;

    // Map category name to enum
    ItemDatabase::Category category = ItemDatabase::Category::Unknown;
    if (categoryName == "All") {
        return getItemList();  // Return all items
    } else if (categoryName == "Stationery") {
        category = ItemDatabase::Category::Stationery;
    } else if (categoryName == "Wallpaper") {
        category = ItemDatabase::Category::Wallpaper;
    } else if (categoryName == "Carpet/Flooring") {
        category = ItemDatabase::Category::Carpet;
    } else if (categoryName == "Clothing") {
        category = ItemDatabase::Category::Clothing;
    } else if (categoryName == "Insects") {
        category = ItemDatabase::Category::Insects;
    } else if (categoryName == "Fish") {
        category = ItemDatabase::Category::Fish;
    } else if (categoryName == "Trash") {
        category = ItemDatabase::Category::Trash;
    } else if (categoryName == "K.K. Songs") {
        category = ItemDatabase::Category::Songs;
    } else if (categoryName == "Tools") {
        category = ItemDatabase::Category::Tools;
    } else if (categoryName == "Umbrellas") {
        category = ItemDatabase::Category::Umbrellas;
    } else if (categoryName == "Hats") {
        category = ItemDatabase::Category::Hats;
    } else if (categoryName == "Hair Accessories") {
        category = ItemDatabase::Category::HairAccessories;
    } else if (categoryName == "Flowers") {
        category = ItemDatabase::Category::Flowers;
    } else if (categoryName == "Glasses") {
        category = ItemDatabase::Category::Glasses;
    } else if (categoryName == "Facial Accessories") {
        category = ItemDatabase::Category::FacialAccessories;
    } else if (categoryName == "Bags of Bells") {
        category = ItemDatabase::Category::Bells;
    } else if (categoryName == "Furniture") {
        category = ItemDatabase::Category::Furniture;
    } else if (categoryName == "Paintings") {
        category = ItemDatabase::Category::Paintings;
    } else if (categoryName == "Museum Displays") {
        category = ItemDatabase::Category::MuseumDisplays;
    } else if (categoryName == "Villager Pictures") {
        category = ItemDatabase::Category::VillagerPictures;
    }

    // Get items for this category
    QList<QPair<uint16_t, QString>> items = ItemDatabase::getItemsByCategory(category);
    for (const auto& item : items) {
        QVariantMap entry;
        entry["hex"] = static_cast<int>(item.first);
        entry["name"] = item.second;
        QString hexStr = QString("%1").arg(item.first, 4, 16, QChar('0')).toUpper();
        entry["display"] = hexStr + " - " + item.second;
        result.append(entry);
    }

    return result;
}

// ========================================
// Letter Data Helpers
// ========================================

void Backend::initLetterData() {
    m_letterData = QByteArray(LTR_FILE_SIZE, 0);
}

void Backend::writeGreetingToData(const QString& greeting) {
    if (m_letterData.size() < LTR_FILE_SIZE) initLetterData();
    QByteArray encoded = encodeAcwwText(greeting, LTR_SUBJECT_SIZE);
    for (int i = 0; i < LTR_SUBJECT_SIZE; i++) {
        m_letterData[LTR_SUBJECT_OFFSET + i] = encoded[i];
    }
}

void Backend::writeBodyToData(const QString& body) {
    if (m_letterData.size() < LTR_FILE_SIZE) initLetterData();
    QByteArray encoded = encodeAcwwText(body, LTR_BODY_SIZE);
    for (int i = 0; i < LTR_BODY_SIZE; i++) {
        m_letterData[LTR_BODY_OFFSET + i] = encoded[i];
    }
}

void Backend::writeSignatureToData(const QString& signature) {
    if (m_letterData.size() < LTR_FILE_SIZE) initLetterData();
    QByteArray encoded = encodeAcwwText(signature, LTR_SIGNATURE_SIZE);
    for (int i = 0; i < LTR_SIGNATURE_SIZE; i++) {
        m_letterData[LTR_SIGNATURE_OFFSET + i] = encoded[i];
    }
}

void Backend::writeRecipientToData() {
    if (m_letterData.size() < LTR_FILE_SIZE) initLetterData();

    // Town ID
    m_letterData[LTR_TO_TOWN_ID_OFFSET] = static_cast<char>(m_recipientTownId & 0xFF);
    m_letterData[LTR_TO_TOWN_ID_OFFSET + 1] = static_cast<char>((m_recipientTownId >> 8) & 0xFF);

    // Town name
    QByteArray townEncoded = encodeAcwwText(m_recipientTown, 8);
    for (int i = 0; i < 8; i++) {
        m_letterData[LTR_TO_TOWN_OFFSET + i] = townEncoded[i];
    }

    // Player ID
    m_letterData[LTR_TO_PLAYER_ID_OFFSET] = static_cast<char>(m_recipientPlayerId & 0xFF);
    m_letterData[LTR_TO_PLAYER_ID_OFFSET + 1] = static_cast<char>((m_recipientPlayerId >> 8) & 0xFF);

    // Player name
    QByteArray nameEncoded = encodeAcwwText(m_recipientName, 8);
    for (int i = 0; i < 8; i++) {
        m_letterData[LTR_TO_NAME_OFFSET + i] = nameEncoded[i];
    }

    // Receiver flags
    m_letterData[LTR_RECEIVER_FLAGS_OFFSET] = static_cast<char>(m_receiverFlags & 0xFF);
    m_letterData[LTR_RECEIVER_FLAGS_OFFSET + 1] = static_cast<char>((m_receiverFlags >> 8) & 0xFF);
    m_letterData[LTR_RECEIVER_FLAGS_OFFSET + 2] = static_cast<char>((m_receiverFlags >> 16) & 0xFF);
    m_letterData[LTR_RECEIVER_FLAGS_OFFSET + 3] = static_cast<char>((m_receiverFlags >> 24) & 0xFF);
}

void Backend::writeSenderToData() {
    if (m_letterData.size() < LTR_FILE_SIZE) initLetterData();

    // Town ID
    m_letterData[LTR_FROM_TOWN_ID_OFFSET] = static_cast<char>(m_senderTownId & 0xFF);
    m_letterData[LTR_FROM_TOWN_ID_OFFSET + 1] = static_cast<char>((m_senderTownId >> 8) & 0xFF);

    // Town name
    QByteArray townEncoded = encodeAcwwText(m_senderTown, 8);
    for (int i = 0; i < 8; i++) {
        m_letterData[LTR_FROM_TOWN_OFFSET + i] = townEncoded[i];
    }

    // Player ID
    m_letterData[LTR_FROM_PLAYER_ID_OFFSET] = static_cast<char>(m_senderPlayerId & 0xFF);
    m_letterData[LTR_FROM_PLAYER_ID_OFFSET + 1] = static_cast<char>((m_senderPlayerId >> 8) & 0xFF);

    // Player name
    QByteArray nameEncoded = encodeAcwwText(m_senderName, 8);
    for (int i = 0; i < 8; i++) {
        m_letterData[LTR_FROM_NAME_OFFSET + i] = nameEncoded[i];
    }

    // Sender flags
    m_letterData[LTR_SENDER_FLAGS_OFFSET] = static_cast<char>(m_senderFlags & 0xFF);
    m_letterData[LTR_SENDER_FLAGS_OFFSET + 1] = static_cast<char>((m_senderFlags >> 8) & 0xFF);
    m_letterData[LTR_SENDER_FLAGS_OFFSET + 2] = static_cast<char>((m_senderFlags >> 16) & 0xFF);
    m_letterData[LTR_SENDER_FLAGS_OFFSET + 3] = static_cast<char>((m_senderFlags >> 24) & 0xFF);
}

void Backend::writeMetadataToData() {
    if (m_letterData.size() < LTR_FILE_SIZE) initLetterData();

    m_letterData[LTR_NAME_POS_OFFSET] = static_cast<char>(m_namePosition);
    m_letterData[LTR_PAPER_OFFSET] = static_cast<char>(m_currentPaper & 0x3F);
    m_letterData[LTR_ICON_FLAGS_OFFSET] = static_cast<char>(m_letterIconFlags);
    m_letterData[LTR_LETTER_SOURCE_OFFSET] = static_cast<char>(m_letterSource);
    m_letterData[LTR_ITEM_OFFSET] = static_cast<char>(m_attachedItem & 0xFF);
    m_letterData[LTR_ITEM_OFFSET + 1] = static_cast<char>((m_attachedItem >> 8) & 0xFF);
}

void Backend::syncLetterDataFromUI() {
    initLetterData();

    // Parse letter text into sections
    int firstNewline = m_letterText.indexOf('\n');
    int lastNewline = m_letterText.lastIndexOf('\n');

    QString greeting, body, signature;
    if (firstNewline < 0) {
        greeting = m_letterText;
    } else if (firstNewline == lastNewline) {
        greeting = m_letterText.left(firstNewline);
        body = m_letterText.mid(firstNewline + 1);
    } else {
        greeting = m_letterText.left(firstNewline);
        body = m_letterText.mid(firstNewline + 1, lastNewline - firstNewline - 1);
        signature = m_letterText.mid(lastNewline + 1);
    }

    writeGreetingToData(greeting);
    writeBodyToData(body);
    writeSignatureToData(signature);
    writeRecipientToData();
    writeSenderToData();
    writeMetadataToData();
}

void Backend::syncUIFromLetterData() {
    if (m_letterData.size() < LTR_FILE_SIZE) return;

    // Decode recipient info
    m_recipientTownId = static_cast<uint8_t>(m_letterData[LTR_TO_TOWN_ID_OFFSET]) |
                        (static_cast<uint8_t>(m_letterData[LTR_TO_TOWN_ID_OFFSET + 1]) << 8);
    m_recipientTown = decodeAcwwText(m_letterData.mid(LTR_TO_TOWN_OFFSET, 8));
    m_recipientPlayerId = static_cast<uint8_t>(m_letterData[LTR_TO_PLAYER_ID_OFFSET]) |
                          (static_cast<uint8_t>(m_letterData[LTR_TO_PLAYER_ID_OFFSET + 1]) << 8);
    m_recipientName = decodeAcwwText(m_letterData.mid(LTR_TO_NAME_OFFSET, 8));

    // Decode sender info
    m_senderTownId = static_cast<uint8_t>(m_letterData[LTR_FROM_TOWN_ID_OFFSET]) |
                     (static_cast<uint8_t>(m_letterData[LTR_FROM_TOWN_ID_OFFSET + 1]) << 8);
    m_senderTown = decodeAcwwText(m_letterData.mid(LTR_FROM_TOWN_OFFSET, 8));
    m_senderPlayerId = static_cast<uint8_t>(m_letterData[LTR_FROM_PLAYER_ID_OFFSET]) |
                       (static_cast<uint8_t>(m_letterData[LTR_FROM_PLAYER_ID_OFFSET + 1]) << 8);
    m_senderName = decodeAcwwText(m_letterData.mid(LTR_FROM_NAME_OFFSET, 8));

    // Decode flags
    m_receiverFlags = static_cast<uint8_t>(m_letterData[LTR_RECEIVER_FLAGS_OFFSET]) |
                      (static_cast<uint8_t>(m_letterData[LTR_RECEIVER_FLAGS_OFFSET + 1]) << 8) |
                      (static_cast<uint8_t>(m_letterData[LTR_RECEIVER_FLAGS_OFFSET + 2]) << 16) |
                      (static_cast<uint8_t>(m_letterData[LTR_RECEIVER_FLAGS_OFFSET + 3]) << 24);
    m_senderFlags = static_cast<uint8_t>(m_letterData[LTR_SENDER_FLAGS_OFFSET]) |
                    (static_cast<uint8_t>(m_letterData[LTR_SENDER_FLAGS_OFFSET + 1]) << 8) |
                    (static_cast<uint8_t>(m_letterData[LTR_SENDER_FLAGS_OFFSET + 2]) << 16) |
                    (static_cast<uint8_t>(m_letterData[LTR_SENDER_FLAGS_OFFSET + 3]) << 24);

    // Decode letter content - greeting is the raw template WITHOUT name inserted
    QString greeting = decodeAcwwText(m_letterData.mid(LTR_SUBJECT_OFFSET, LTR_SUBJECT_SIZE));
    QString body = decodeAcwwText(m_letterData.mid(LTR_BODY_OFFSET, LTR_BODY_SIZE));
    QString signature = decodeAcwwText(m_letterData.mid(LTR_SIGNATURE_OFFSET, LTR_SIGNATURE_SIZE));

    // Decode metadata
    m_namePosition = static_cast<uint8_t>(m_letterData[LTR_NAME_POS_OFFSET]);
    m_currentPaper = static_cast<uint8_t>(m_letterData[LTR_PAPER_OFFSET]) & 0x3F;
    m_letterIconFlags = static_cast<uint8_t>(m_letterData[LTR_ICON_FLAGS_OFFSET]);
    m_letterSource = static_cast<uint8_t>(m_letterData[LTR_LETTER_SOURCE_OFFSET]);
    m_attachedItem = static_cast<uint8_t>(m_letterData[LTR_ITEM_OFFSET]) |
                     (static_cast<uint8_t>(m_letterData[LTR_ITEM_OFFSET + 1]) << 8);

    // The name is NOT inserted into the greeting text - it's rendered visually only
    // m_namePosition indicates where the name should appear during rendering
    // Clamp namePosition to valid range
    if (m_namePosition > greeting.length()) {
        m_namePosition = static_cast<uint8_t>(greeting.length());
    }

    // Store sections - greeting is the raw template (no name)
    m_letterHeader = greeting;
    m_letterBody = body;
    m_letterFooter = signature;

    // Build combined text - also without name (it's visual only)
    m_letterText = greeting + "\n" + body + "\n" + signature;

    // recipientNameStart/End are now just used for visual rendering, not text editing
    // They indicate where the name appears visually in the rendered output
    m_recipientNameStart = m_namePosition;
    m_recipientNameEnd = m_namePosition + m_recipientName.length();
}
