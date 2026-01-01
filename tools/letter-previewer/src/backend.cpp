#include "backend.h"

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

    // Build paper names list
    m_paperNames.clear();
    for (int i = 0; i < m_stationery.count(); i++) {
        m_paperNames.append(QString("Paper %1").arg(i, 2, 10, QChar('0')));
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

QImage Backend::getPaperImage(int index) const {
    if (index >= 0 && index < m_stationery.count()) {
        return m_stationery.getPaper(index);
    }
    return QImage();
}
