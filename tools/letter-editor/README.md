# ACWW Letter Editor

A Qt5 application for editing letters in Animal Crossing: Wild World save files. Extracts stationery and fonts from the ROM for authentic preview rendering.

## Features

- Extracts all 64 letter stationery backgrounds from the ROM
- Extracts the game font (fontA - 16x16 variable-width glyphs)
- Live preview of letter text on selected stationery
- Direct text editing on the canvas (click and type)
- Supports basic editing: arrow keys, backspace, delete, enter for newlines

## Requirements

- C++17 compiler (g++ or clang++)
- Qt5 (Core, Widgets, Gui)
- CMake 3.16+ or qmake

### Installing Qt5

**Ubuntu/Debian:**
```bash
sudo apt install qtbase5-dev qt5-qmake
```

**Fedora:**
```bash
sudo dnf install qt5-qtbase-devel
```

**Arch Linux:**
```bash
sudo pacman -S qt5-base
```

**macOS (Homebrew):**
```bash
brew install qt@5
```

## Building

### Using CMake:
```bash
mkdir build && cd build
cmake ..
make
```

### Using qmake:
```bash
qmake letter-editor.pro
make
```

## Usage

1. Run the application: `./letter-editor`
2. A file dialog will open - select your ACWW ROM file (*.nds)
3. Select a stationery background from the dropdown
4. Click on the letter preview and start typing
5. Use arrow keys to move cursor, Backspace/Delete to remove characters, Enter for newlines

## File Structure

```
src/
├── main.cpp              # Application entry point with file dialog
├── mainwindow.h/cpp      # Main window with background selector
├── letter_canvas.h/cpp   # Custom widget for letter preview and text editing
├── stationery_loader.h/cpp # Extracts stationery from ROM into QImage
├── font_loader.h/cpp     # Extracts font glyphs from ROM
├── nds_rom.h/cpp         # NDS ROM parser (NitroFS)
└── lz77.h/cpp            # LZ77 decompression
```
