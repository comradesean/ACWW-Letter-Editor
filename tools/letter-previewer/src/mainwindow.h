#pragma once
#include <QMainWindow>
#include <QComboBox>
#include "stationery_loader.h"
#include "font_loader.h"
#include "letter_canvas.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

    bool loadRom(const QString& romPath);

private slots:
    void onBackgroundChanged(int index);

private:
    void setupUI();

    QComboBox* m_backgroundCombo = nullptr;
    LetterCanvas* m_canvas = nullptr;

    StationeryLoader m_stationery;
    FontLoader m_font;
};
