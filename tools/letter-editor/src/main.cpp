#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include "backend.h"
#include "lettercanvasitem.h"

int main(int argc, char* argv[]) {
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
    }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
