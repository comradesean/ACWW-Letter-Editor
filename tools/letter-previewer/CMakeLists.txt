cmake_minimum_required(VERSION 3.16)
project(letter-previewer LANGUAGES CXX)

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
    src/nds_rom.cpp
    src/lz77.cpp
)

set(HEADERS
    src/backend.h
    src/lettercanvasitem.h
    src/stationery_loader.h
    src/font_loader.h
    src/nds_rom.h
    src/lz77.h
)

# Resources
set(RESOURCES
    resources.qrc
)

# Create executable
add_executable(letter-previewer ${SOURCES} ${HEADERS} ${RESOURCES})

# Link Qt libraries
target_link_libraries(letter-previewer
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickControls2
)

# Include directories
target_include_directories(letter-previewer PRIVATE src)
