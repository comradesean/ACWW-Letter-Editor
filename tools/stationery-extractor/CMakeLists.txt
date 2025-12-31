cmake_minimum_required(VERSION 3.10)
project(stationery-extractor VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/extract_2d_paper.cpp
    src/nds_rom.cpp
    src/lz77.cpp
)

set(HEADERS
    src/nds_rom.h
    src/lz77.h
)

# Create executable
add_executable(extract_2d_paper ${SOURCES} ${HEADERS})

# Platform-specific settings
if(WIN32)
    target_compile_definitions(extract_2d_paper PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# Install target
install(TARGETS extract_2d_paper DESTINATION bin)
