#include "mainwindow.h"
#include "nds_rom.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUI();
    setWindowTitle("ACWW Letter Previewer");
}

void MainWindow::setupUI() {
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // Background selection row
    QHBoxLayout* topLayout = new QHBoxLayout();
    QLabel* bgLabel = new QLabel("Background:", this);
    m_backgroundCombo = new QComboBox(this);
    m_backgroundCombo->setMinimumWidth(150);
    topLayout->addWidget(bgLabel);
    topLayout->addWidget(m_backgroundCombo);
    topLayout->addStretch();
    mainLayout->addLayout(topLayout);

    // Canvas for letter preview (directly editable)
    m_canvas = new LetterCanvas(this);
    m_canvas->setFixedSize(512, 384);  // Fixed 2x scale (256*2, 192*2)
    mainLayout->addWidget(m_canvas, 0, Qt::AlignCenter);

    // Instructions label
    QLabel* instructionLabel = new QLabel("Click the letter and start typing. Use arrow keys, backspace, and Enter.", this);
    instructionLabel->setAlignment(Qt::AlignCenter);
    instructionLabel->setStyleSheet("color: gray; font-style: italic;");
    mainLayout->addWidget(instructionLabel);

    // Connect signals
    connect(m_backgroundCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onBackgroundChanged);
}

bool MainWindow::loadRom(const QString& romPath) {
    NDSRom rom;
    if (!rom.load(romPath.toStdString())) {
        QMessageBox::critical(this, "Error", "Failed to load ROM file:\n" + romPath);
        return false;
    }

    // Load stationery
    if (!m_stationery.load(rom)) {
        QMessageBox::critical(this, "Error", "Failed to load stationery from ROM");
        return false;
    }

    // Load font
    if (!m_font.load(rom)) {
        QMessageBox::critical(this, "Error", "Failed to load font from ROM");
        return false;
    }

    // Populate background combo
    m_backgroundCombo->clear();
    for (int i = 0; i < m_stationery.count(); i++) {
        m_backgroundCombo->addItem(QString("Paper %1").arg(i, 2, 10, QChar('0')));
    }

    // Set font on canvas
    m_canvas->setFont(&m_font);

    // Select first background
    if (m_stationery.count() > 0) {
        m_backgroundCombo->setCurrentIndex(0);
        m_canvas->setBackground(m_stationery.getPaper(0), 0);
    }

    // Give focus to canvas for immediate typing
    m_canvas->setFocus();

    return true;
}

void MainWindow::onBackgroundChanged(int index) {
    if (index >= 0 && index < m_stationery.count()) {
        m_canvas->setBackground(m_stationery.getPaper(index), index);
    }
}
