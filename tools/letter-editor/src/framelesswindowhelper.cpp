#include "framelesswindowhelper.h"

#ifdef Q_OS_WIN

#include <QGuiApplication>
#include <windows.h>
#include <dwmapi.h>

FramelessWindowHelper::FramelessWindowHelper(QQuickWindow* window)
    : m_window(window)
{
}

void FramelessWindowHelper::apply(QQuickWindow* window) {
    if (!window) return;

    HWND hwnd = reinterpret_cast<HWND>(window->winId());
    if (!hwnd) return;

    // Install native event filter
    auto* helper = new FramelessWindowHelper(window);
    qApp->installNativeEventFilter(helper);

    // Remove the window frame by adjusting styles
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~(WS_CAPTION | WS_THICKFRAME);
    SetWindowLong(hwnd, GWL_STYLE, style);

    // Force Windows to recalculate the window frame
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
}

bool FramelessWindowHelper::nativeEventFilter(const QByteArray& eventType, void* message, long* result) {
    if (eventType != "windows_generic_MSG") {
        return false;
    }

    MSG* msg = static_cast<MSG*>(message);
    if (!m_window || reinterpret_cast<HWND>(m_window->winId()) != msg->hwnd) {
        return false;
    }

    switch (msg->message) {
        case WM_NCCALCSIZE: {
            // Returning 0 tells Windows there's no non-client area
            // This removes the invisible border that causes screenshot issues
            *result = 0;
            return true;
        }
    }

    return false;
}

#endif // Q_OS_WIN
