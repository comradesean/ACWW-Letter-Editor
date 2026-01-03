#pragma once

#ifdef Q_OS_WIN

#include <QAbstractNativeEventFilter>
#include <QQuickWindow>

class FramelessWindowHelper : public QAbstractNativeEventFilter {
public:
    static void apply(QQuickWindow* window);

    bool nativeEventFilter(const QByteArray& eventType, void* message, long* result) override;

private:
    explicit FramelessWindowHelper(QQuickWindow* window);
    QQuickWindow* m_window = nullptr;
};

#endif // Q_OS_WIN
