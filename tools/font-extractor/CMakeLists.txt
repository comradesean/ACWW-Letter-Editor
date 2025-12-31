cmake_minimum_required(VERSION 3.10)
project(font-extractor VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/extract_font.cpp
    src/nds_rom.cpp
    src/lz77.cpp
)

set(HEADERS
    src/nds_rom.h
    src/lz77.h
)

# Create executable
add_executable(extract_font ${SOURCES} ${HEADERS})

# Platform-specific settings
if(WIN32)
    target_compile_definitions(extract_font PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# Install target
install(TARGETS extract_font DESTINATION bin)
