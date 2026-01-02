#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QSurfaceFormat>
#include "backend.h"
#include "lettercanvasitem.h"

#ifdef Q_OS_WIN
#include "framelesswindowhelper.h"
#endif

int main(int argc, char* argv[]) {
    // Set surface format with alpha for translucent windows
    QSurfaceFormat format;
    format.setAlphaBufferSize(8);
    QSurfaceFormat::setDefaultFormat(format);

    QGuiApplication app(argc, argv);
    app.setApplicationName("ACWW Letter Previewer");

    // Set Material style
    QQuickStyle::setStyle("Material");

    // Register QML types
    qmlRegisterType<Backend>("LetterPreviewer", 1, 0, "Backend");
    qmlRegisterType<LetterCanvasItem>("LetterPreviewer", 1, 0, "LetterCanvas");

    QQmlApplicationEngine engine;

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject* obj, const QUrl& objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
        // Set translucent background and apply frameless helper on Windows
        if (obj) {
            QQuickWindow* window = qobject_cast<QQuickWindow*>(obj);
            if (window) {
                window->setColor(Qt::transparent);
#ifdef Q_OS_WIN
                FramelessWindowHelper::apply(window);
#endif
            }
        }
    }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
