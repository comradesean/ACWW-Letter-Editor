#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QSurfaceFormat>
#include "backend.h"
#include "lettercanvasitem.h"
#include "lettericonprovider.h"

#ifdef Q_OS_WIN
#include "framelesswindowhelper.h"
#endif

// Global pointer to icon provider (set before QML loads, updated when backend loads ROM)
static LetterIconProvider* g_iconProvider = nullptr;

int main(int argc, char* argv[]) {
    // Set surface format with alpha for translucent windows
    QSurfaceFormat format;
    format.setAlphaBufferSize(8);
    QSurfaceFormat::setDefaultFormat(format);

    QGuiApplication app(argc, argv);
    app.setApplicationName("Animal Crossing Wild World Letter Tool");

    // Set Material style
    QQuickStyle::setStyle("Material");

    // Register QML types
    qmlRegisterType<Backend>("LetterPreviewer", 1, 0, "Backend");
    qmlRegisterType<LetterCanvasItem>("LetterPreviewer", 1, 0, "LetterCanvas");

    QQmlApplicationEngine engine;

    // Create and register letter icon image provider
    g_iconProvider = new LetterIconProvider();
    engine.addImageProvider("lettericons", g_iconProvider);

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url, &engine](QObject* obj, const QUrl& objUrl) {
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

            // Find Backend instance and connect its loadedChanged signal
            // to update the icon provider
            Backend* backend = obj->findChild<Backend*>();
            if (backend && g_iconProvider) {
                QObject::connect(backend, &Backend::loadedChanged, [backend]() {
                    if (backend->isLoaded()) {
                        g_iconProvider->setIconLoader(&backend->icons());
                    }
                });
                // If already loaded, set immediately
                if (backend->isLoaded()) {
                    g_iconProvider->setIconLoader(&backend->icons());
                }
            }
        }
    }); // Direct connection ensures this runs before QML bindings evaluate

    engine.load(url);

    return app.exec();
}
