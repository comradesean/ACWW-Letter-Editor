#include "backend.h"
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

// ACWW Western character encoding table (256 entries)
// Maps byte values 0x00-0xFF to Unicode characters
static const char* acwwCharTable[256] = {
    "\0", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",        // 0x00-0x0F
    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "a", "b", "c", "d", "e",         // 0x10-0x1F
    "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u",         // 0x20-0x2F
    "v", "w", "x", "y", "z", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "f",         // 0x30-0x3F
    "s", "O", "Z", "s", "o", "z", "Y", "A", "A", "A", "A", "A", "A", "A", "C", "E",         // 0x40-0x4F (accented)
    "E", "E", "E", "I", "I", "I", "I", "D", "N", "O", "O", "O", "O", "O", "O", "U",         // 0x50-0x5F
    "U", "U", "U", "Y", "T", "s", "a", "a", "a", "a", "a", "a", "a", "c", "e", "e",         // 0x60-0x6F
    "e", "e", "i", "i", "i", "i", "d", "n", "o", "o", "o", "o", "o", "o", "u", "u",         // 0x70-0x7F
    "u", "u", "y", "t", "y", " ", "\n", "!", "\"", "#", "$", "%", "&", "'", "(", ")",       // 0x80-0x8F
    "*", "+", ",", "-", ".", "/", ":", ";", "<", "=", ">", "?", "@", "[", "{", "]",         // 0x90-0x9F
    "|", "_", "}", ",", ".", ".", "~", "L", "+", "+", "^", "%", "<", "`", "\"", "*",        // 0xA0-0xAF
    "-", "'", "-", "\"", "T", ">", " ", "~", "Y", "|", "S", "!", "c", "L", " ", "c",        // 0xB0-0xBF
    "a", "<", "-", "-", "R", "o", "+", "2", "3", "-", "s", "P", ">", "1", "o",              // 0xC0-0xCE
    ">", ".", "1", "1", "3", " ", " ", " ", " ", "?", "x", "/", " ", "*", " ", " ",         // 0xCF-0xDF (special chars)
    " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ",         // 0xE0-0xEF
    " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "          // 0xF0-0xFF
};

// LTR file format constants (EUR/USA)
static const int LTR_FILE_SIZE = 0xF4;        // 244 bytes

// Receiver (To) field - 0x00-0x13 (20 bytes)
static const int LTR_TO_TOWN_ID_OFFSET = 0x00;     // 2 bytes (uint16)
static const int LTR_TO_TOWN_OFFSET = 0x02;        // 8 bytes (town name)
static const int LTR_TO_PLAYER_ID_OFFSET = 0x0A;   // 2 bytes (uint16)
static const int LTR_TO_NAME_OFFSET = 0x0C;        // 8 bytes (player name)

// Sender (From) field - 0x18-0x2B (20 bytes)
static const int LTR_FROM_TOWN_ID_OFFSET = 0x18;   // 2 bytes (uint16)
static const int LTR_FROM_TOWN_OFFSET = 0x1A;      // 8 bytes (town name)
static const int LTR_FROM_PLAYER_ID_OFFSET = 0x22; // 2 bytes (uint16)
static const int LTR_FROM_NAME_OFFSET = 0x24;      // 8 bytes (player name)

// Letter content
static const int LTR_SUBJECT_OFFSET = 0x30;   // Greeting/subject template
static const int LTR_SUBJECT_SIZE = 0x10;     // 16 bytes
static const int LTR_BODY_OFFSET = 0x48;      // Letter body
static const int LTR_BODY_SIZE = 0x64;        // 100 bytes
static const int LTR_SIGNATURE_OFFSET = 0xC8; // Signature/ending
static const int LTR_SIGNATURE_SIZE = 0x1A;   // 26 bytes

// Metadata
static const int LTR_NAME_POS_OFFSET = 0xE8;  // 1 byte - position to insert name in greeting
static const int LTR_PAPER_OFFSET = 0xE9;     // 1 byte

// Decode ACWW bytes to QString
static QString decodeAcwwText(const QByteArray& data) {
    QString result;
    for (int i = 0; i < data.size(); i++) {
        uint8_t byte = static_cast<uint8_t>(data[i]);
        if (byte == 0) {
            // Null byte - could be end of string or space
            continue;
        }
        result += QString::fromUtf8(acwwCharTable[byte]);
    }
    return result.trimmed();
}

// Encode QString to ACWW bytes
static QByteArray encodeAcwwText(const QString& text, int maxLen) {
    QByteArray result(maxLen, 0);
    int pos = 0;

    for (int i = 0; i < text.length() && pos < maxLen; i++) {
        QChar ch = text[i];
        uint8_t encoded = 0x85; // Default to space

        // Find the character in the encoding table
        if (ch == '\n') {
            encoded = 0x86; // Newline
        } else if (ch == ' ') {
            encoded = 0x85; // Space
        } else {
            // Search for matching character
            for (int j = 1; j < 256; j++) {
                if (QString::fromUtf8(acwwCharTable[j]) == QString(ch)) {
                    encoded = static_cast<uint8_t>(j);
                    break;
                }
            }
        }

        result[pos++] = static_cast<char>(encoded);
    }

    return result;
}

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
        emit currentPaperChanged();
        emit paperChanged();
    }
}

void Backend::setLetterText(const QString& text) {
    if (text != m_letterText) {
        m_letterText = text;
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
        emit recipientInfoChanged();
    }
}

void Backend::setRecipientTown(const QString& town) {
    if (town != m_recipientTown) {
        m_recipientTown = town;
        emit recipientInfoChanged();
    }
}

void Backend::setRecipientTownId(int id) {
    if (id != m_recipientTownId) {
        m_recipientTownId = id;
        emit recipientInfoChanged();
    }
}

void Backend::setRecipientPlayerId(int id) {
    if (id != m_recipientPlayerId) {
        m_recipientPlayerId = id;
        emit recipientInfoChanged();
    }
}

void Backend::setSenderName(const QString& name) {
    if (name != m_senderName) {
        m_senderName = name;
        emit senderInfoChanged();
    }
}

void Backend::setSenderTown(const QString& town) {
    if (town != m_senderTown) {
        m_senderTown = town;
        emit senderInfoChanged();
    }
}

void Backend::setSenderTownId(int id) {
    if (id != m_senderTownId) {
        m_senderTownId = id;
        emit senderInfoChanged();
    }
}

void Backend::setSenderPlayerId(int id) {
    if (id != m_senderPlayerId) {
        m_senderPlayerId = id;
        emit senderInfoChanged();
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

    // Extract recipient name, town, and IDs from To field
    uint16_t recipientTownId = static_cast<uint8_t>(data[LTR_TO_TOWN_ID_OFFSET]) |
                               (static_cast<uint8_t>(data[LTR_TO_TOWN_ID_OFFSET + 1]) << 8);
    QByteArray toTownBytes = data.mid(LTR_TO_TOWN_OFFSET, 8);
    QString recipientTown = decodeAcwwText(toTownBytes);
    uint16_t recipientPlayerId = static_cast<uint8_t>(data[LTR_TO_PLAYER_ID_OFFSET]) |
                                 (static_cast<uint8_t>(data[LTR_TO_PLAYER_ID_OFFSET + 1]) << 8);
    QByteArray toNameBytes = data.mid(LTR_TO_NAME_OFFSET, 8);
    QString recipientName = decodeAcwwText(toNameBytes);

    // Extract sender name, town, and IDs from From field
    uint16_t senderTownId = static_cast<uint8_t>(data[LTR_FROM_TOWN_ID_OFFSET]) |
                            (static_cast<uint8_t>(data[LTR_FROM_TOWN_ID_OFFSET + 1]) << 8);
    QByteArray fromTownBytes = data.mid(LTR_FROM_TOWN_OFFSET, 8);
    QString senderTown = decodeAcwwText(fromTownBytes);
    uint16_t senderPlayerId = static_cast<uint8_t>(data[LTR_FROM_PLAYER_ID_OFFSET]) |
                              (static_cast<uint8_t>(data[LTR_FROM_PLAYER_ID_OFFSET + 1]) << 8);
    QByteArray fromNameBytes = data.mid(LTR_FROM_NAME_OFFSET, 8);
    QString senderName = decodeAcwwText(fromNameBytes);

    // Extract subject/greeting template
    QByteArray subjectBytes = data.mid(LTR_SUBJECT_OFFSET, LTR_SUBJECT_SIZE);
    QString subjectTemplate = decodeAcwwText(subjectBytes);

    // Get name insertion position
    int namePos = static_cast<uint8_t>(data[LTR_NAME_POS_OFFSET]);

    // Insert recipient name into greeting at the specified position
    QString subject;
    int recipientStart = -1;
    int recipientEnd = -1;
    if (namePos >= 0 && namePos <= subjectTemplate.length() && !recipientName.isEmpty()) {
        subject = subjectTemplate.left(namePos) + recipientName + subjectTemplate.mid(namePos);
        recipientStart = namePos;
        recipientEnd = namePos + recipientName.length();
    } else {
        subject = subjectTemplate;
    }

    // Extract body bytes (99 bytes total)
    QByteArray bodyBytes = data.mid(LTR_BODY_OFFSET, LTR_BODY_SIZE);

    // Decode body - preserve exactly as stored (including spacing and newlines)
    QString body = decodeAcwwText(bodyBytes);

    // Extract signature
    QByteArray sigBytes = data.mid(LTR_SIGNATURE_OFFSET, LTR_SIGNATURE_SIZE);
    QString signature = decodeAcwwText(sigBytes);

    // Extract paper ID
    int paperId = static_cast<uint8_t>(data[LTR_PAPER_OFFSET]);
    if (paperId < 0 || paperId > 63) {
        paperId = 0;
    }

    // Store the separate sections for use by canvas.setLetterContent()
    m_letterHeader = subject;
    m_letterBody = body;
    m_letterFooter = signature;

    // Build the combined letter text
    QString letterText = subject + "\n" + body + "\n" + signature;

    // Update state
    m_recipientName = recipientName;
    m_recipientTown = recipientTown;
    m_recipientTownId = recipientTownId;
    m_recipientPlayerId = recipientPlayerId;
    m_senderName = senderName;
    m_senderTown = senderTown;
    m_senderTownId = senderTownId;
    m_senderPlayerId = senderPlayerId;
    m_recipientNameStart = recipientStart;
    m_recipientNameEnd = recipientEnd;
    setLetterText(letterText);
    setCurrentPaper(paperId);

    qDebug() << "Imported LTR file:" << path;
    qDebug() << "Paper ID:" << paperId;
    qDebug() << "Recipient:" << recipientName << "from" << recipientTown;
    qDebug() << "Sender:" << senderName << "from" << senderTown;
    qDebug() << "Subject template:" << subjectTemplate;
    qDebug() << "Subject (with name):" << subject;
    qDebug() << "Body:" << body;
    qDebug() << "Signature:" << signature;

    return true;
}

bool Backend::exportLtr(const QUrl& fileUrl) const {
    QString path = fileUrl.toLocalFile();

    // Parse new format: header\nbody\nfooter
    int firstNewline = m_letterText.indexOf('\n');
    int lastNewline = m_letterText.lastIndexOf('\n');

    QString subject, body, signature;
    if (firstNewline < 0) {
        // No newlines - all header
        subject = m_letterText;
    } else if (firstNewline == lastNewline) {
        // One newline - header and body only
        subject = m_letterText.left(firstNewline);
        body = m_letterText.mid(firstNewline + 1);
    } else {
        // Two+ newlines - header, body, footer
        subject = m_letterText.left(firstNewline);
        body = m_letterText.mid(firstNewline + 1, lastNewline - firstNewline - 1);
        signature = m_letterText.mid(lastNewline + 1);
    }

    // Remove any newlines from body for export (they become 0x86 via encoding)
    // Body newlines are preserved as they map to 0x86 in encodeAcwwText

    // Create LTR file buffer (244 bytes, all zeros initially)
    QByteArray data(LTR_FILE_SIZE, 0);

    // Encode and write subject
    QByteArray subjectEncoded = encodeAcwwText(subject, LTR_SUBJECT_SIZE);
    for (int i = 0; i < LTR_SUBJECT_SIZE; i++) {
        data[LTR_SUBJECT_OFFSET + i] = subjectEncoded[i];
    }

    // Encode and write body
    QByteArray bodyEncoded = encodeAcwwText(body, LTR_BODY_SIZE);
    for (int i = 0; i < LTR_BODY_SIZE; i++) {
        data[LTR_BODY_OFFSET + i] = bodyEncoded[i];
    }

    // Encode and write signature
    QByteArray sigEncoded = encodeAcwwText(signature, LTR_SIGNATURE_SIZE);
    for (int i = 0; i < LTR_SIGNATURE_SIZE; i++) {
        data[LTR_SIGNATURE_OFFSET + i] = sigEncoded[i];
    }

    // Write paper ID
    data[LTR_PAPER_OFFSET] = static_cast<char>(m_currentPaper & 0x3F);

    // Write recipient info
    data[LTR_TO_TOWN_ID_OFFSET] = static_cast<char>(m_recipientTownId & 0xFF);
    data[LTR_TO_TOWN_ID_OFFSET + 1] = static_cast<char>((m_recipientTownId >> 8) & 0xFF);
    QByteArray recipientTownEncoded = encodeAcwwText(m_recipientTown, 8);
    for (int i = 0; i < 8; i++) {
        data[LTR_TO_TOWN_OFFSET + i] = recipientTownEncoded[i];
    }
    data[LTR_TO_PLAYER_ID_OFFSET] = static_cast<char>(m_recipientPlayerId & 0xFF);
    data[LTR_TO_PLAYER_ID_OFFSET + 1] = static_cast<char>((m_recipientPlayerId >> 8) & 0xFF);
    QByteArray recipientNameEncoded = encodeAcwwText(m_recipientName, 8);
    for (int i = 0; i < 8; i++) {
        data[LTR_TO_NAME_OFFSET + i] = recipientNameEncoded[i];
    }

    // Write sender info
    data[LTR_FROM_TOWN_ID_OFFSET] = static_cast<char>(m_senderTownId & 0xFF);
    data[LTR_FROM_TOWN_ID_OFFSET + 1] = static_cast<char>((m_senderTownId >> 8) & 0xFF);
    QByteArray senderTownEncoded = encodeAcwwText(m_senderTown, 8);
    for (int i = 0; i < 8; i++) {
        data[LTR_FROM_TOWN_OFFSET + i] = senderTownEncoded[i];
    }
    data[LTR_FROM_PLAYER_ID_OFFSET] = static_cast<char>(m_senderPlayerId & 0xFF);
    data[LTR_FROM_PLAYER_ID_OFFSET + 1] = static_cast<char>((m_senderPlayerId >> 8) & 0xFF);
    QByteArray senderNameEncoded = encodeAcwwText(m_senderName, 8);
    for (int i = 0; i < 8; i++) {
        data[LTR_FROM_NAME_OFFSET + i] = senderNameEncoded[i];
    }

    // Write file
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to create LTR file:" << path;
        return false;
    }

    file.write(data);
    file.close();

    qDebug() << "Exported LTR file:" << path;
    return true;
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

    // Helper lambda to draw header with recipient coloring
    auto drawHeader = [&](const QString& text, int x, int y) {
        int drawX = x;

        for (int i = 0; i < text.length(); i++) {
            QChar ch = text[i];

            // Determine color - use recipient color if within recipient name range
            QColor color = textColor;
            if (m_recipientNameStart >= 0 && m_recipientNameEnd >= 0 &&
                i >= m_recipientNameStart && i < m_recipientNameEnd) {
                color = recipientColor;
            }

            QImage glyph = m_font.getColoredGlyph(ch, color);

            if (!glyph.isNull()) {
                QImage scaledGlyph = glyph.scaled(16 * scale, 16 * scale, Qt::IgnoreAspectRatio, Qt::FastTransformation);
                painter.drawImage(drawX, y, scaledGlyph);
                drawX += (m_font.charWidth(ch) + 1) * scale;
            } else {
                drawX += (m_font.charWidth(ch) + 1) * scale;
            }
        }
    };

    // Draw header with recipient coloring
    drawHeader(header, headerLeft, headerTop);

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

void Backend::loadCurrentSlot() {
    if (!m_saveFile.isLoaded()) {
        qDebug() << "Backend::loadCurrentSlot: No save file loaded";
        return;
    }

    Letter letter = m_saveFile.getLetter(m_currentPlayer, m_currentStorageType, m_currentSlot);

    // Update backend state from letter
    m_recipientName = letter.toPlayerName;
    m_recipientTown = letter.toTownName;
    m_recipientTownId = letter.toTownId;
    m_recipientPlayerId = letter.toPlayerId;
    m_senderName = letter.fromPlayerName;
    m_senderTown = letter.fromTownName;
    m_senderTownId = letter.fromTownId;
    m_senderPlayerId = letter.fromPlayerId;

    // Get greeting with name inserted
    QString greetingWithName = letter.getGreetingWithName();
    m_recipientNameStart = letter.namePosition;
    m_recipientNameEnd = letter.namePosition + letter.toPlayerName.length();

    // Store sections
    m_letterHeader = greetingWithName;
    m_letterBody = letter.body;
    m_letterFooter = letter.signature;

    // Build combined text
    m_letterText = greetingWithName + "\n" + letter.body + "\n" + letter.signature;

    // Set paper
    if (letter.stationeryType >= 0 && letter.stationeryType < 64) {
        m_currentPaper = letter.stationeryType;
    }

    emit letterTextChanged();
    emit recipientInfoChanged();
    emit senderInfoChanged();
    emit recipientNamePositionChanged();
    emit currentPaperChanged();
    emit paperChanged();

    qDebug() << "Backend::loadCurrentSlot: Loaded player" << m_currentPlayer
             << "storage" << m_currentStorageType << "slot" << m_currentSlot;
}

void Backend::saveCurrentSlot() {
    if (!m_saveFile.isLoaded()) {
        qDebug() << "Backend::saveCurrentSlot: No save file loaded";
        return;
    }

    // Parse sections from letter text
    int firstNewline = m_letterText.indexOf('\n');
    int lastNewline = m_letterText.lastIndexOf('\n');

    QString header, body, signature;
    if (firstNewline < 0) {
        header = m_letterText;
    } else if (firstNewline == lastNewline) {
        header = m_letterText.left(firstNewline);
        body = m_letterText.mid(firstNewline + 1);
    } else {
        header = m_letterText.left(firstNewline);
        body = m_letterText.mid(firstNewline + 1, lastNewline - firstNewline - 1);
        signature = m_letterText.mid(lastNewline + 1);
    }

    // Build letter structure
    Letter letter;
    letter.toTownId = m_recipientTownId;
    letter.toTownName = m_recipientTown;
    letter.toPlayerId = m_recipientPlayerId;
    letter.toPlayerName = m_recipientName;
    letter.fromTownId = m_senderTownId;
    letter.fromTownName = m_senderTown;
    letter.fromPlayerId = m_senderPlayerId;
    letter.fromPlayerName = m_senderName;

    // Remove recipient name from header to get template
    if (m_recipientNameStart >= 0 && m_recipientNameEnd > m_recipientNameStart) {
        letter.greeting = header.left(m_recipientNameStart) + header.mid(m_recipientNameEnd);
        letter.namePosition = m_recipientNameStart;
    } else {
        letter.greeting = header;
        letter.namePosition = 0;
    }

    letter.body = body;
    letter.signature = signature;
    letter.stationeryType = m_currentPaper;
    letter.status = LetterStatus::OPENED;  // Mark as opened when edited

    // Save to file
    m_saveFile.setLetter(m_currentPlayer, m_currentStorageType, m_currentSlot, letter);

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

    emit letterTextChanged();
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
    m_recipientName = m_saveFile.getPlayerName(m_currentPlayer);
    m_recipientTown = m_saveFile.getPlayerTown(m_currentPlayer);
    m_recipientPlayerId = m_saveFile.getPlayerId(m_currentPlayer);
    m_recipientTownId = m_saveFile.getTownId(m_currentPlayer);

    emit recipientInfoChanged();

    qDebug() << "Backend::importAddresseeFromSave: Imported"
             << m_recipientName << "from" << m_recipientTown
             << "(Player ID:" << m_recipientPlayerId << ", Town ID:" << m_recipientTownId << ")";
}
