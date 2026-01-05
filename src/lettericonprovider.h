#pragma once
#include <QQuickImageProvider>
#include <QImage>
#include "icon_loader.h"
#include "letter_icons.h"

/**
 * QQuickImageProvider for letter icons
 *
 * Usage in QML:
 *   Image { source: "image://lettericons/<iconFlags>/<hasGift>" }
 *
 * Examples:
 *   "image://lettericons/1/0"  - Letter being written, no gift
 *   "image://lettericons/2/1"  - Unopened received letter with gift
 *   "image://lettericons/5/0"  - Unopened received bottled mail, no gift
 */
class LetterIconProvider : public QQuickImageProvider {
public:
    LetterIconProvider()
        : QQuickImageProvider(QQuickImageProvider::Image)
        , m_iconLoader(nullptr)
    {}

    void setIconLoader(const IconLoader* loader) {
        m_iconLoader = loader;
    }

    QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override {
        if (!m_iconLoader || !m_iconLoader->isLoaded()) {
            // Return empty/placeholder image
            QImage placeholder(16, 16, QImage::Format_ARGB32);
            placeholder.fill(Qt::transparent);
            if (size) *size = placeholder.size();
            return placeholder;
        }

        // Parse id: "iconFlags/hasGift" e.g., "1/0" or "2/1"
        QStringList parts = id.split('/');
        if (parts.size() < 2) {
            QImage placeholder(16, 16, QImage::Format_ARGB32);
            placeholder.fill(Qt::transparent);
            if (size) *size = placeholder.size();
            return placeholder;
        }

        bool ok1, ok2;
        int iconFlags = parts[0].toInt(&ok1);
        int hasGift = parts[1].toInt(&ok2);

        if (!ok1 || !ok2) {
            QImage placeholder(16, 16, QImage::Format_ARGB32);
            placeholder.fill(Qt::transparent);
            if (size) *size = placeholder.size();
            return placeholder;
        }

        // Get icon definition from flags
        LetterIcons::IconDef iconDef = LetterIcons::getIconFromFlags(
            static_cast<uint8_t>(iconFlags),
            hasGift != 0
        );

        if (iconDef.index < 0 || iconDef.palette < 0) {
            QImage placeholder(16, 16, QImage::Format_ARGB32);
            placeholder.fill(Qt::transparent);
            if (size) *size = placeholder.size();
            return placeholder;
        }

        QImage icon = m_iconLoader->getIcon(iconDef.index, iconDef.palette);

        // Scale if requested
        if (requestedSize.isValid() && requestedSize != icon.size()) {
            icon = icon.scaled(requestedSize, Qt::KeepAspectRatio, Qt::FastTransformation);
        }

        if (size) *size = icon.size();
        return icon;
    }

private:
    const IconLoader* m_iconLoader;
};
