# ACWW Letter Editor

A Windows application for viewing letters from Animal Crossing: Wild World save files.

## Features

- Open and read raw ACWW save files (*.sav)
- View letters from all 4 player slots
- Browse letters stored in:
  - Player inventory (backpack) - 20 slots
  - Mailbox - 20 slots
  - Bank storage - 75 slots
- Visual letter preview with original stationery backgrounds
- Displays decoded letter text (subject, body, signature)
- Shows raw hex data for debugging/analysis

## Requirements

- Windows
- .NET Framework 4.7.2 or later

## Usage

1. Launch the application
2. Go to **File → Open Save**
3. Select your ACWW save file (*.sav)
4. Use the dropdowns to select:
   - Player (1-4)
   - Location (Backpack, Mailbox, or Bank Storage)
   - Letter number
5. View the letter content and stationery preview

## Building from Source

Open `ACWW Letter Editor.sln` in Visual Studio 2019 or later and build the solution.

```bash
msbuild "ACWW Letter Editor.sln" /p:Configuration=Release
```

## Save File Format

ACWW save files contain letter data at specific memory offsets. Each letter is 244 bytes (0xF4) and includes:

| Offset | Size | Description |
|--------|------|-------------|
| 0x30 | 15 bytes | Subject line |
| 0x48 | 99 bytes | Letter body |
| 0xC8 | 25 bytes | Signature |
| 0xE9 | 1 byte | Stationery paper ID |

The application uses ACWW's custom character encoding table to decode the text.

## License

This project is provided for educational and personal use for interacting with your own save files.
