# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
# Build with CMake
mkdir build && cd build
cmake ..
make

# Or build with qmake
qmake letter-editor.pro
make
```

Output binary is `letter-editor` in the build directory.

## Project Overview

This is a Qt5/QML application (C++17) for editing letters in Animal Crossing: Wild World save files. It extracts stationery and fonts directly from the ROM for authentic preview rendering.

## Architecture

### Entry Point
- `src/main.cpp` - Application entry point, registers QML types, launches QML UI

### Core Classes
- `src/backend.h/cpp` - Main application backend, manages ROM/save file loading, letter data, and QML property bindings
- `src/savefile.h/cpp` - Save file parser, reads/writes letter data at specific memory offsets
- `src/letter.h/cpp` - Letter data structure (subject, body, signature, stationery, attached item)
- `src/lettercanvasitem.h/cpp` - QQuickPaintedItem for rendering letter preview with stationery and font

### ROM Data Extraction
- `src/nds_rom.h/cpp` - NDS ROM parser (NitroFS filesystem)
- `src/lz77.h/cpp` - LZ77 decompression for compressed ROM assets
- `src/stationery_loader.h/cpp` - Extracts 64 letter stationery backgrounds from ROM
- `src/font_loader.h/cpp` - Extracts game font (fontA - 16x16 variable-width glyphs)
- `src/icon_loader.h/cpp` - Extracts inventory item icons from ROM
- `src/cloth_loader.h/cpp` - Extracts clothing/pattern textures

### Data
- `src/acww_encoding.h` - ACWW custom character encoding lookup tables
- `src/itemdatabase.h/cpp` - Item ID to name mapping database
- `src/letter_icons.h` - Letter slot icon definitions
- `src/letterconstants.h` - Letter dimension and layout constants

### UI
- `qml/` - QML UI files
- `src/lettericonprovider.h` - QQuickImageProvider for letter slot icons
- `src/framelesswindowhelper.h/cpp` - Helper for frameless window dragging

## Save File Structure

Letters are stored in three locations per player:
- **Inventory (Backpack)** - 20 letter slots
- **Mailbox** - 20 letter slots
- **Bank Storage** - 75 letter slots

Each letter is 0xF4 (244) bytes:
- Subject at offset 0x30 (15 bytes)
- Body at offset 0x48 (99 bytes)
- Signature at offset 0xC8 (25 bytes)
- Background paper ID at offset 0xE9
- Attached item at offset 0xEA (2 bytes)

## Dependencies

- Qt5 (Core, Widgets, Gui, Qml, Quick)
- C++17 compiler
- CMake 3.16+ or qmake
