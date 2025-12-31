# ACWW Font Extractor

A C++ tool to extract bitmap fonts from Animal Crossing: Wild World NDS ROM files as spritesheets.

## Features

- Extracts all 5 game fonts (fontA, fontASub, fontB, fontC, fontD)
- Outputs PNG spritesheets with transparent backgrounds
- Generates character map files with character codes and widths

## Building

```bash
g++ -o extract_font src/extract_font.cpp src/nds_rom.cpp src/lz77.cpp -I src -std=c++17
```

## Usage

```bash
./extract_font [output_dir]
```

### Arguments

- `output_dir` - Output directory for extracted fonts (default: `extracted_fonts`)

The ROM path is hardcoded to `../../references/Welcome to Animal Crossing - Wild World (USA).nds`

## Output

For each font, two files are generated:
- `fontX.png` - Spritesheet with glyphs arranged in a 16-column grid
- `fontX_map.txt` - Character map with index, code, width, and character

## Font Details

| Font | Glyphs | Size | Description |
|------|--------|------|-------------|
| fontA | 224 | 16x16 | Main game font (ASCII + extended) |
| fontASub | 8 | 16x16 | Supplementary characters |
| fontB | 224 | 8x8 | Small font variant |
| fontC | 20 | 16x16 | Special characters |
| fontD | 15 | 8x8 | Numeric/symbol font |

## Technical Details

### Font File Format

Each font consists of 3 files in `/font/`:
- `fontX_head.bin` - 8-byte header (char count, glyph dimensions)
- `fontX_attr.bin` - Character attributes (4 bytes per char: code + width)
- `fontX_img.bin` - Glyph image data (2bpp, 4 shades of gray)

### Header Format (8 bytes)

| Offset | Size | Description |
|--------|------|-------------|
| 0 | 2 | Number of characters |
| 2 | 2 | Reserved |
| 4 | 2 | Glyph width |
| 6 | 2 | Glyph height |

### Attribute Format (4 bytes per character)

| Offset | Size | Description |
|--------|------|-------------|
| 0 | 2 | Character code |
| 2 | 2 | Character width (pixels) |

### Image Format

- 2 bits per pixel (4 colors)
- Palette: transparent, dark gray, light gray, white
- Glyphs stored sequentially, row-major order

## License

Part of the ACWW Letter Editor project.
