# ACWW Letter Editor

A Qt5 application for editing letters in Animal Crossing: Wild World save files. Extracts stationery and fonts from the ROM for authentic preview rendering.

## Features

- Open and read ACWW save files (*.sav)
- View and edit letters from all 4 player slots
- Browse letters stored in:
  - Player inventory (backpack) - 20 slots
  - Mailbox - 20 slots
  - Bank storage - 75 slots
- Extracts all 64 letter stationery backgrounds from the ROM
- Extracts the game font (fontA - 16x16 variable-width glyphs)
- Live preview of letter text on selected stationery
- Direct text editing on the canvas (click and type)
- Supports basic editing: arrow keys, backspace, delete, enter for newlines

## Requirements

- C++17 compiler (g++ or clang++)
- Qt5 (Core, Gui, Qml, Quick, QuickControls2)
- CMake 3.16+ or qmake

### Installing Qt5

**Ubuntu/Debian:**
```bash
sudo apt install qtbase5-dev qtdeclarative5-dev qml-module-qtquick-controls2 qml-module-qtquick-dialogs qml-module-qtgraphicaleffects qt5-qmake
```

**Fedora:**
```bash
sudo dnf install qt5-qtbase-devel qt5-qtdeclarative-devel qt5-qtquickcontrols2-devel qt5-qtgraphicaleffects
```

**Arch Linux:**
```bash
sudo pacman -S qt5-base qt5-declarative qt5-quickcontrols2 qt5-graphicaleffects
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
2. Use **File → Open ROM** to load your ACWW ROM file (*.nds) - this extracts stationery and fonts
3. Use **File → Open Save** to load your save file (*.sav)
4. Select a player and letter location (Inventory, Mailbox, or Bank Storage)
5. Select a letter slot to view/edit
6. Click on the letter canvas to edit text directly
7. Use arrow keys to navigate, Backspace/Delete to remove characters, Enter for newlines
8. Change stationery, recipient/sender info, and attached items as needed
9. Save changes back to the save file

## File Structure

```
src/
├── main.cpp                # Application entry point, registers QML types
├── backend.h/cpp           # Main backend - ROM/save loading, letter management
├── lettercanvasitem.h/cpp  # QQuickPaintedItem for letter preview rendering
├── savefile.h/cpp          # Save file parser and writer
├── letter.h/cpp            # Letter data structure
├── stationery_loader.h/cpp # Extracts stationery backgrounds from ROM
├── font_loader.h/cpp       # Extracts font glyphs from ROM
├── icon_loader.h/cpp       # Extracts inventory item icons from ROM
├── nds_rom.h/cpp           # NDS ROM parser (NitroFS)
├── lz77.h/cpp              # LZ77 decompression
├── itemdatabase.h/cpp      # Item ID to name mapping
└── acww_encoding.h         # Character encoding lookup tables

qml/
├── main.qml                # Main application window
├── SaveBrowser.qml         # Save file browser panel
├── components/             # Reusable UI components
└── dialogs/                # Dialog windows
```

## Save File Format

ACWW save files contain letter data at specific memory offsets. Each letter is 244 bytes (0xF4) and includes:

| Offset | Size | Description |
|--------|------|-------------|
| 0x30 | 15 bytes | Subject line |
| 0x48 | 99 bytes | Letter body |
| 0xC8 | 25 bytes | Signature |
| 0xE9 | 1 byte | Stationery paper ID |
| 0xEA | 2 bytes | Attached item ID |

The application uses ACWW's custom character encoding table to decode the text.

## License

This project is provided for educational and personal use for interacting with your own save files.
