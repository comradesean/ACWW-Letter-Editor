QT += core gui qml quick quickcontrols2
CONFIG += c++17

TARGET = letter-previewer
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/backend.cpp \
    src/lettercanvasitem.cpp \
    src/stationery_loader.cpp \
    src/font_loader.cpp \
    src/nds_rom.cpp \
    src/lz77.cpp

HEADERS += \
    src/backend.h \
    src/lettercanvasitem.h \
    src/stationery_loader.h \
    src/font_loader.h \
    src/nds_rom.h \
    src/lz77.h

RESOURCES += resources.qrc

INCLUDEPATH += src
