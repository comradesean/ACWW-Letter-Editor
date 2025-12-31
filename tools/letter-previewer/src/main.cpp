#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include "mainwindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("ACWW Letter Previewer");

    // Get ROM path from file dialog
    QString romPath = QFileDialog::getOpenFileName(
        nullptr,
        "Select Animal Crossing: Wild World ROM",
        QString(),
        "NDS ROM Files (*.nds);;All Files (*)"
    );

    if (romPath.isEmpty()) {
        return 0;  // User cancelled
    }

    MainWindow window;
    if (!window.loadRom(romPath)) {
        return 1;
    }

    window.resize(550, 450);  // Sized to fit 512x384 canvas + controls
    window.show();

    return app.exec();
}
