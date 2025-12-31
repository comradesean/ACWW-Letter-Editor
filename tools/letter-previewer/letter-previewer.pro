QT += core widgets gui
CONFIG += c++17

TARGET = letter-previewer
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/letter_canvas.cpp \
    src/stationery_loader.cpp \
    src/font_loader.cpp \
    src/nds_rom.cpp \
    src/lz77.cpp

HEADERS += \
    src/mainwindow.h \
    src/letter_canvas.h \
    src/stationery_loader.h \
    src/font_loader.h \
    src/nds_rom.h \
    src/lz77.h

INCLUDEPATH += src
