cmake_minimum_required(VERSION 3.16)
project(letter-editor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt5
find_package(Qt5 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2)

# Enable Qt MOC, UIC, RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Source files
set(SOURCES
    src/main.cpp
    src/backend.cpp
    src/lettercanvasitem.cpp
    src/stationery_loader.cpp
    src/font_loader.cpp
    src/cloth_loader.cpp
    src/icon_loader.cpp
    src/nds_rom.cpp
    src/lz77.cpp
    src/letter.cpp
    src/savefile.cpp
    src/itemdatabase.cpp
)

set(HEADERS
    src/backend.h
    src/lettercanvasitem.h
    src/stationery_loader.h
    src/font_loader.h
    src/cloth_loader.h
    src/icon_loader.h
    src/letter_icons.h
    src/lettericonprovider.h
    src/nds_rom.h
    src/lz77.h
    src/letter.h
    src/savefile.h
    src/itemdatabase.h
)

# Windows-specific frameless window helper
if(WIN32)
    list(APPEND SOURCES src/framelesswindowhelper.cpp)
    list(APPEND HEADERS src/framelesswindowhelper.h)
endif()

# Resources
set(RESOURCES
    resources.qrc
)

# Create executable
add_executable(letter-editor ${SOURCES} ${HEADERS} ${RESOURCES})

# Link Qt libraries
target_link_libraries(letter-editor
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickControls2
)

# Windows-specific libraries for frameless window
if(WIN32)
    target_link_libraries(letter-editor dwmapi)
endif()

# Include directories
target_include_directories(letter-editor PRIVATE src)
