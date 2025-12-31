# ACWW Stationery Extractor

A C++ tool to extract letter stationery textures from Animal Crossing: Wild World NDS ROM files.

## Features

- Parses NDS ROM file structure (NitroFS)
- Extracts high-quality 2D stationery from `/menu/paper/` (256x192 native resolution)
- Handles LZ77 decompression
- Exports textures as PNG files with transparency

## Building

```bash
g++ -o extract_2d_paper src/extract_2d_paper.cpp src/nds_rom.cpp src/lz77.cpp -I src -std=c++17
```

## Usage

```bash
./extract_2d_paper [output_dir]
```

### Arguments

- `output_dir` - Output directory for extracted textures (default: `extracted_2d_paper`)

The ROM path is hardcoded to `../../references/Welcome to Animal Crossing - Wild World (USA).nds`

## Output

Extracts all 64 stationery papers (000-063) as 256x192 PNG files with transparency.

Output files: `paper_000.png` through `paper_063.png`

## Stationery Data Locations

The high-quality 2D stationery used in the letter writing UI is stored in:

```
/menu/paper/
├── chr/     # Tile graphics (000.bch - 063.bch)
├── plt/     # Palettes (000.bpl - 063.bpl)
└── scr/     # Screen maps (000.bsc - 063.bsc)
```

## Technical Details

### File Formats

- **BCH**: 4bpp tile graphics (8x8 pixels per tile, 32 bytes each)
- **BPL**: 3 sub-palettes of 16 RGB555 colors (96 bytes total)
- **BSC**: Screen map entries (2 bytes each: tile number + flip + palette)

### Palette Mapping

| Screen Palette | BPL Offset |
|----------------|------------|
| 0 | Transparent |
| 13 | 32 (sub-palette 1) |
| 14 | 64 (sub-palette 2) |
| 15 | 0 (sub-palette 0) |

See `docs/NDS_FORMAT.md` for complete technical documentation.

## License

Part of the ACWW Letter Editor project.
