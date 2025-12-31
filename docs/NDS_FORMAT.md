# NDS ROM and Texture Format Technical Reference

This document provides a comprehensive technical reference for the Nintendo DS ROM format and texture structures used in extracting stationery textures from Animal Crossing: Wild World.

---

## Table of Contents

1. [NDS ROM Structure](#1-nds-rom-structure)
2. [LZ77 Compression](#2-lz77-compression)
3. [BTX0/NSBTX Format](#3-btx0nsbtx-format)
4. [Texture Formats](#4-texture-formats)
5. [Stationery File Locations](#5-stationery-file-locations)
6. [Font Format](#6-font-format)

---

## 1. NDS ROM Structure

### 1.1 ROM Header (0x0000 - 0x017F)

The NDS ROM header occupies the first 0x4000 bytes, but only the first 0x180 bytes contain essential information.

| Offset | Size | Field | Description |
|--------|------|-------|-------------|
| 0x000 | 12 | `gameTitle` | ASCII game title (null-padded) |
| 0x00C | 4 | `gameCode` | 4-character game code (e.g., "ADME" for ACWW USA) |
| 0x010 | 2 | `makerCode` | 2-character maker code |
| 0x012 | 1 | `unitCode` | Unit code (0x00 = NDS) |
| 0x013 | 1 | `encryptionSeed` | Encryption seed index |
| 0x014 | 1 | `deviceCapacity` | Device capacity (128KB << n) |
| 0x015 | 7 | `reserved1` | Reserved (zero-filled) |
| 0x01C | 1 | `reserved2` | Reserved |
| 0x01D | 1 | `ndsRegion` | NDS region code |
| 0x01E | 1 | `romVersion` | ROM version |
| 0x01F | 1 | `autostart` | Autostart flag |
| 0x020 | 4 | `arm9RomOffset` | ARM9 binary ROM offset |
| 0x024 | 4 | `arm9EntryAddress` | ARM9 entry address |
| 0x028 | 4 | `arm9RamAddress` | ARM9 RAM load address |
| 0x02C | 4 | `arm9Size` | ARM9 binary size |
| 0x030 | 4 | `arm7RomOffset` | ARM7 binary ROM offset |
| 0x034 | 4 | `arm7EntryAddress` | ARM7 entry address |
| 0x038 | 4 | `arm7RamAddress` | ARM7 RAM load address |
| 0x03C | 4 | `arm7Size` | ARM7 binary size |
| **0x040** | **4** | **`fntOffset`** | **File Name Table offset** |
| **0x044** | **4** | **`fntSize`** | **File Name Table size** |
| **0x048** | **4** | **`fatOffset`** | **File Allocation Table offset** |
| **0x04C** | **4** | **`fatSize`** | **File Allocation Table size** |
| 0x050 | 4 | `arm9OverlayOffset` | ARM9 overlay table offset |
| 0x054 | 4 | `arm9OverlaySize` | ARM9 overlay table size |
| 0x058 | 4 | `arm7OverlayOffset` | ARM7 overlay table offset |
| 0x05C | 4 | `arm7OverlaySize` | ARM7 overlay table size |
| 0x060 | 4 | `normalCardControl` | Normal card control settings |
| 0x064 | 4 | `secureCardControl` | Secure card control settings |
| 0x068 | 4 | `iconBannerOffset` | Icon/banner data offset |
| 0x06C | 2 | `secureAreaCRC` | Secure area CRC16 |
| 0x06E | 2 | `secureTransferTimeout` | Secure transfer timeout |
| 0x070 | 4 | `arm9Autoload` | ARM9 autoload list RAM address |
| 0x074 | 4 | `arm7Autoload` | ARM7 autoload list RAM address |
| 0x078 | 8 | `secureDisable` | Secure area disable magic |
| 0x080 | 4 | `romSize` | Total ROM size (excluding DSi area) |
| 0x084 | 4 | `headerSize` | ROM header size (typically 0x4000) |

### 1.2 File Allocation Table (FAT)

The FAT stores the start and end offsets for each file in the ROM. Each entry is 8 bytes.

```
FAT Entry Structure (8 bytes):
+--------+--------+
| Offset | Size   | Description
+--------+--------+
| 0x00   | 4      | Start offset in ROM
| 0x04   | 4      | End offset in ROM (exclusive)
+--------+--------+
```

**Total entries**: `fatSize / 8`

**File size calculation**: `endOffset - startOffset`

### 1.3 File Name Table (FNT)

The FNT consists of two parts:
1. **Main Table**: Directory entry records
2. **Sub-Tables**: File/subdirectory name entries for each directory

#### Main Table Entry (8 bytes per directory)

| Offset | Size | Description |
|--------|------|-------------|
| 0x00 | 4 | Sub-table offset (relative to FNT start) |
| 0x04 | 2 | First file ID for this directory |
| 0x06 | 2 | Parent directory ID (0xF000-0xFFFF for directories, total dir count for root) |

**Note**: Directory IDs are `0xF000 + index`. The root directory is ID `0xF000`.

#### Sub-Table Entry Format

Sub-tables contain sequential entries describing files and subdirectories:

```
Type/Length Byte:
  Bits 0-6: Name length (1-127 characters)
  Bit 7:    0 = File, 1 = Directory

If File:
  - Read `length` bytes for filename
  - Increment file ID counter

If Directory:
  - Read `length` bytes for directory name
  - Read 2 bytes for directory ID (0xF000-0xFFFF)

Terminator: 0x00 byte ends the sub-table
```

### 1.4 NitroFS Navigation Algorithm

```
1. Read FNT main table entry at index 0 (root directory)
2. Get sub-table offset and first file ID
3. Seek to (fntOffset + subTableOffset)
4. Loop:
   a. Read type/length byte
   b. If 0x00: end of directory
   c. If bit 7 set (directory):
      - Read name (length & 0x7F bytes)
      - Read directory ID (2 bytes)
      - Recursively parse using main table entry at (dirId & 0xFFF)
   d. If bit 7 clear (file):
      - Read name (length bytes)
      - Look up FAT entry at current file ID
      - Increment file ID
```

---

## 2. LZ77 Compression

ACWW uses a variant of Nintendo's LZ77 compression for texture data.

### 2.1 Header Formats

#### Standard NDS LZ77 Header (4 bytes)

```
+--------+--------+
| Offset | Size   | Description
+--------+--------+
| 0x00   | 1      | Compression type (0x10 = LZ77)
| 0x01   | 3      | Decompressed size (little-endian, 24-bit)
+--------+--------+
```

#### ACWW Custom Header (8 bytes)

```
+--------+--------+
| Offset | Size   | Description
+--------+--------+
| 0x00   | 4      | Magic "LZ77" (ASCII)
| 0x04   | 1      | Compression type (0x10 = LZ77)
| 0x05   | 3      | Decompressed size (little-endian, 24-bit)
+--------+--------+
```

### 2.2 Detection

Data is LZ77 compressed if:
- First 4 bytes are ASCII "LZ77", OR
- First byte is 0x10

### 2.3 Decompression Algorithm

```
Input:  Compressed data stream (after header)
Output: Decompressed buffer

while output_position < decompressed_size:
    flags = read_byte()  // 8-bit flag byte

    for i = 0 to 7:
        if output_position >= decompressed_size:
            break

        if flags & 0x80:  // High bit set = backreference
            byte1 = read_byte()
            byte2 = read_byte()

            // Length: upper 4 bits + 3 (range: 3-18)
            length = ((byte1 >> 4) & 0x0F) + 3

            // Offset: lower 4 bits << 8 | byte2 + 1 (range: 1-4096)
            offset = ((byte1 & 0x0F) << 8) | byte2 + 1

            // Copy from already-decompressed data
            copy_position = output_position - offset
            for j = 0 to length-1:
                output[output_position++] = output[copy_position + j]

        else:  // High bit clear = literal byte
            output[output_position++] = read_byte()

        flags <<= 1  // Shift to next flag bit
```

### 2.4 Backreference Encoding

```
Backreference (2 bytes):
+----------------+----------------+
| Byte 1         | Byte 2         |
+----------------+----------------+
| LLLL OOOO      | OOOO OOOO      |
+----------------+----------------+

L = Length - 3 (4 bits, range 0-15 -> length 3-18)
O = Offset - 1 (12 bits, range 0-4095 -> offset 1-4096)
```

---

## 3. BTX0/NSBTX Format

BTX0 (also known as NSBTX) is Nintendo's texture container format for DS 3D graphics.

### 3.1 BTX0 Header (16 bytes + section offsets)

| Offset | Size | Description |
|--------|------|-------------|
| 0x00 | 4 | Magic "BTX0" (ASCII) |
| 0x04 | 2 | Byte order mark (0xFFFE = little-endian) |
| 0x06 | 2 | Version (typically 0x0001) |
| 0x08 | 4 | Total file size |
| 0x0C | 2 | Header size (typically 0x10) |
| 0x0E | 2 | Number of sections |
| 0x10+ | 4*n | Section offset array |

**Note**: BMD0 (3D model files) may also contain embedded TEX0 sections.

### 3.2 TEX0 Section Structure

The TEX0 section contains texture and palette data.

#### TEX0 Header (0x28 bytes)

| Offset | Size | Description |
|--------|------|-------------|
| 0x00 | 4 | Magic "TEX0" (ASCII) |
| 0x04 | 4 | Section size |
| 0x08 | 4 | Padding/Reserved |
| 0x0C | 2 | Texture data size >> 3 |
| 0x0E | 2 | Texture info offset (relative to TEX0) |
| 0x10 | 4 | Padding/Reserved |
| 0x14 | 4 | Texture data offset (relative to TEX0) |
| 0x18 | 4 | Padding/Reserved |
| 0x1C | 2 | Palette data size >> 3 |
| 0x1E | 2 | Padding/Reserved |
| 0x20 | 4 | Palette info offset (relative to TEX0) |
| 0x24 | 4 | Palette data offset (relative to TEX0) |

### 3.3 3D Info / Dictionary Structure

Both texture and palette info sections use the same dictionary structure.

#### Info Section Header (4 bytes)

| Offset | Size | Description |
|--------|------|-------------|
| 0x00 | 1 | Dummy (always 0) |
| 0x01 | 1 | Number of entries |
| 0x02 | 2 | Info section size |

#### Dictionary Header (4 bytes, at offset +4)

| Offset | Size | Description |
|--------|------|-------------|
| 0x00 | 1 | Dummy (always 0) |
| 0x01 | 1 | Number of entries (same as above) |
| 0x02 | 2 | Dictionary size |

The dictionary contains a Patricia tree for fast name lookups, followed by data entries.

### 3.4 Texture Data Entry (8 bytes per texture)

Data entries start at: `dictOffset + dictSize`

| Offset | Size | Description |
|--------|------|-------------|
| 0x00 | 2 | Texture image offset >> 3 |
| 0x02 | 2 | Parameters (see below) |
| 0x04 | 4 | Additional parameters |

#### Parameters Field (16-bit)

```
Bits 0-3:   Repeat mode
Bits 4-6:   Width exponent (width = 8 << value)
Bits 7-9:   Height exponent (height = 8 << value)
Bits 10-12: Texture format (see Section 4)
Bit 13:     Color 0 transparent flag
Bits 14-15: Coordinate transform mode
```

**Width/Height Lookup Table**:

| Value | Dimension |
|-------|-----------|
| 0 | 8 |
| 1 | 16 |
| 2 | 32 |
| 3 | 64 |
| 4 | 128 |
| 5 | 256 |
| 6 | 512 |
| 7 | 1024 |

### 3.5 Name Table

Located at: `texInfoOffset + texInfoSectionSize`

Each name entry is 16 bytes (null-padded ASCII string).

### 3.6 Palette Info

Palette info follows the same dictionary structure as texture info.

#### Palette Data Entry (4 bytes per palette)

| Offset | Size | Description |
|--------|------|-------------|
| 0x00 | 2 | Palette offset >> 3 |
| 0x02 | 2 | Reserved |

---

## 4. Texture Formats

### 4.1 Format Types

| Value | Name | Description | Bits/Pixel |
|-------|------|-------------|------------|
| 0 | None | No texture | - |
| 1 | A3I5 | 3-bit alpha, 5-bit color index | 8 |
| 2 | Palette4 | 4-color palette | 2 |
| 3 | Palette16 | 16-color palette | 4 |
| 4 | Palette256 | 256-color palette | 8 |
| 5 | Compressed | 4x4 block compressed | 2+ |
| 6 | A5I3 | 5-bit alpha, 3-bit color index | 8 |
| 7 | Direct | 16-bit direct color (ABGR1555) | 16 |

### 4.2 Texture Data Size Calculation

| Format | Size Formula |
|--------|--------------|
| A3I5, A5I3, Palette256 | `width * height` |
| Palette4 | `width * height / 4` |
| Palette16 | `width * height / 2` |
| Direct | `width * height * 2` |

### 4.3 Palette Size per Format

| Format | Palette Entries |
|--------|-----------------|
| A3I5 | 32 |
| Palette4 | 4 |
| Palette16 | 16 |
| Palette256 | 256 |
| A5I3 | 8 |
| Direct | N/A (no palette) |

### 4.4 RGB555 Color Format

All palette entries and Direct format pixels use RGB555:

```
15-bit color (stored in 16-bit word):
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
|A/X| B4| B3| B2| B1| B0| G4| G3| G2| G1| G0| R4| R3| R2| R1| R0|
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 15   14  13  12  11  10   9   8   7   6   5   4   3   2   1   0

Bit 15: Alpha/unused (1 = opaque in Direct format, ignored for palette)
Bits 10-14: Blue (0-31)
Bits 5-9:   Green (0-31)
Bits 0-4:   Red (0-31)
```

**Conversion to 8-bit per channel**:
```
R8 = R5 * 255 / 31
G8 = G5 * 255 / 31
B8 = B5 * 255 / 31
```

### 4.5 Decoding Algorithms

#### Palette4 (2 bits per pixel)

```
for each byte in data:
    for j = 0 to 3:  // 4 pixels per byte
        index = (byte >> (j * 2)) & 0x03
        pixel[pixelIndex++] = palette[index]
```

#### Palette16 (4 bits per pixel)

```
for each byte in data:
    for j = 0 to 1:  // 2 pixels per byte
        index = (byte >> (j * 4)) & 0x0F
        pixel[pixelIndex++] = palette[index]
```

#### Palette256 (8 bits per pixel)

```
for each byte in data:
    pixel[pixelIndex++] = palette[byte]
```

#### A3I5 (3-bit alpha, 5-bit index)

```
for each byte in data:
    index = byte & 0x1F          // Bits 0-4: color index
    alpha = (byte >> 5) & 0x07   // Bits 5-7: alpha

    pixel[i] = palette[index]
    pixel[i].alpha = alpha * 255 / 7  // Scale 0-7 to 0-255
```

#### A5I3 (5-bit alpha, 3-bit index)

```
for each byte in data:
    index = byte & 0x07          // Bits 0-2: color index
    alpha = (byte >> 3) & 0x1F   // Bits 3-7: alpha

    pixel[i] = palette[index]
    pixel[i].alpha = alpha * 255 / 31  // Scale 0-31 to 0-255
```

#### Direct Color (ABGR1555)

```
for each 2-byte word in data:
    color = word[0] | (word[1] << 8)  // Little-endian

    R = ((color >> 0) & 0x1F) * 255 / 31
    G = ((color >> 5) & 0x1F) * 255 / 31
    B = ((color >> 10) & 0x1F) * 255 / 31
    A = (color & 0x8000) ? 255 : 0
```

### 4.6 Color 0 Transparency

When the `color0Transparent` flag (bit 13 of texture params) is set:
- Palette index 0 is treated as fully transparent
- Alpha is set to 0 for any pixel using palette index 0

---

## 5. Stationery File Locations

ACWW stores stationery graphics in two locations:
1. **3D Textures** (`/ftr/6/`) - Lower resolution furniture preview textures (NSBTX format)
2. **2D Tile Graphics** (`/menu/paper/`) - Full resolution letter UI backgrounds (recommended)

### 5.1 2D Stationery Format (Recommended)

The high-quality stationery backgrounds used in the letter writing UI are stored as 2D tile-based graphics in `/menu/paper/`. This is the recommended source for extracting stationery.

#### Directory Structure

```
/menu/paper/
├── chr/     # Tile graphics (64 files: 000.bch - 063.bch)
├── plt/     # Palettes (64 files: 000.bpl - 063.bpl)
└── scr/     # Screen maps (64 files: 000.bsc - 063.bsc)
```

#### File Formats

**BCH (Character/Tile Data)**
- LZ77 compressed
- Decompressed size: ~3232 bytes (varies by paper)
- Contains 8x8 pixel tiles at 4bpp (32 bytes per tile)
- No header - raw tile data starts at offset 0
- Tile 0 in CHR corresponds to screen tile 16

**BPL (Palette Data)**
- LZ77 compressed
- Decompressed size: 96 bytes
- Contains 3 sub-palettes of 32 bytes each (16 colors × 2 bytes per RGB555 color)

| Sub-palette | Offset | Screen Palette Index |
|-------------|--------|---------------------|
| 0 | 0-31 | 15 |
| 1 | 32-63 | 13 |
| 2 | 64-95 | 14 |

**BSC (Screen Map Data)**
- LZ77 compressed
- Decompressed size: 2048 bytes (32×32 tile entries, but only 32×24 used)
- Maps tiles to screen positions for 256×192 display

#### Screen Map Entry Format (2 bytes)

```
Bits 0-9:   Tile number (add 16 to get CHR tile index, or 0 for transparent)
Bit 10:     Horizontal flip
Bit 11:     Vertical flip
Bits 12-15: Palette index (0=transparent, 13/14/15=sub-palettes)
```

#### Palette Index Mapping

| Screen Palette | BPL Offset | Description |
|----------------|------------|-------------|
| 0 | N/A | Fully transparent |
| 13 | 32 | Sub-palette 1 |
| 14 | 64 | Sub-palette 2 |
| 15 | 0 | Sub-palette 0 |

**Note**: Palette color index 0 within each sub-palette is also transparent.

#### 2D Extraction Algorithm

```
1. Load BCH, BPL, BSC files for paper number (000-063)
2. Decompress all three files if LZ77 compressed
3. Parse BPL into 3 sub-palettes, mapping to screen indices 13, 14, 15
4. For each tile position (32×24 grid = 256×192 pixels):
   a. Read 2-byte screen map entry
   b. Extract tile number, flip flags, and palette index
   c. If tile number > 0: subtract 16 to get CHR tile index
   d. Read 32 bytes of 4bpp tile data from CHR
   e. For each pixel in 8×8 tile:
      - Apply horizontal/vertical flip if flagged
      - Look up 4-bit color index in appropriate palette
      - If palette index 0 or color index 0: pixel is transparent
5. Output 256×192 RGBA image
```

### 5.2 3D Texture Format (Lower Quality)

The `/ftr/6/` directory contains 3D furniture preview textures in NSBTX format. These are lower resolution than the 2D versions and should only be used if the 2D source is unavailable.

#### Path Pattern

```
/ftr/6/<subdirectory>/<filename>.nsbtx
```

Where subdirectories are numbered (e.g., `/ftr/6/9/`, `/ftr/6/10/`, etc.)

#### File Naming Convention

Files follow the pattern: `XXYY.nsbtx`

Where XXYY is a hex identifier corresponding to the item ID in the game's furniture table.

**Examples**:
- `/ftr/6/9/069a.nsbtx`
- `/ftr/6/13/06d9.nsbtx`

#### Known Stationery ID Ranges

The stationery items are generally found in the range:
- Start: `0x069A` (approximately)
- End: `0x06D9` (approximately)

### 5.3 Extraction Workflow (3D Textures)

```
1. Load NDS ROM
2. Parse FAT and FNT to build file system tree
3. Find all files in /ftr/6/ with .nsbtx extension
4. For each file:
   a. Read file data from ROM using FAT offsets
   b. Check for LZ77 compression ("LZ77" header or 0x10 byte)
   c. If compressed, decompress using LZ77 algorithm
   d. Parse BTX0 header and locate TEX0 section
   e. Parse texture dictionary to get texture metadata
   f. Read texture data and palette data
   g. Decode pixels based on texture format
   h. Export to PNG or other image format
```

### 5.4 Stationery ID to Name Mapping

The game stores stationery names in separate string tables. The hex ID in the filename corresponds to the item's internal ID. To get human-readable names, you would need to:

1. Locate the game's item string table
2. Map the item ID to the corresponding string offset
3. Decode the string (typically Shift-JIS or UTF-16 depending on region)

---

## 6. Font Format

ACWW stores bitmap fonts in the `/font/` directory using a custom format with three files per font.

### 6.1 Font Files

Each font consists of three files:

```
/font/
├── fontA_head.bin    # Header (8 bytes)
├── fontA_attr.bin    # Character attributes
├── fontA_img.bin     # Glyph image data
├── fontASub_head.bin
├── fontASub_attr.bin
├── fontASub_img.bin
├── fontB_head.bin
├── fontB_attr.bin
├── fontB_img.bin
├── fontC_head.bin
├── fontC_attr.bin
├── fontC_img.bin
├── fontD_head.bin
├── fontD_attr.bin
└── fontD_img.bin
```

### 6.2 Available Fonts

| Font | Characters | Glyph Size | Description |
|------|------------|------------|-------------|
| fontA | 224 | 16×16 | Main game font (ASCII + extended characters) |
| fontASub | 8 | 16×16 | Supplementary characters |
| fontB | 224 | 8×8 | Small font variant |
| fontC | 20 | 16×16 | Special characters |
| fontD | 15 | 8×8 | Numeric/symbol font |

### 6.3 Header Format (_head.bin)

8 bytes, may be LZ77 compressed.

| Offset | Size | Description |
|--------|------|-------------|
| 0x00 | 2 | Number of characters |
| 0x02 | 2 | Reserved (0) |
| 0x04 | 2 | Glyph width in pixels |
| 0x06 | 2 | Glyph height in pixels |

### 6.4 Attribute Format (_attr.bin)

4 bytes per character, may be LZ77 compressed.

| Offset | Size | Description |
|--------|------|-------------|
| 0x00 | 2 | Character code (Unicode/ASCII) |
| 0x02 | 2 | Character width in pixels |

**Total size**: `numChars × 4` bytes

Character codes typically start at 0x20 (space) and include standard ASCII plus extended characters. The width value indicates the advance width for proportional text rendering.

### 6.5 Image Format (_img.bin)

Glyph bitmap data, may be LZ77 compressed.

**Pixel Format**: 1 bit per pixel (monochrome)

| Value | Color |
|-------|-------|
| 0 | Transparent |
| 1 | White (opaque) |

**Storage**: Glyphs stored sequentially in row-major order. Each glyph is `(width × height) / 8` bytes.

**Byte Layout** (1bpp, MSB first):
```
Byte: [P7] [P6] [P5] [P4] [P3] [P2] [P1] [P0]
      bit7 bit6 bit5 bit4 bit3 bit2 bit1 bit0

First pixel is in bit 7 (MSB), last pixel in bit 0 (LSB)
```

**Size Calculation**:
- 16×16 glyph @ 1bpp = 32 bytes per glyph (2 bytes per row × 16 rows)
- 8×8 glyph @ 1bpp = 8 bytes per glyph (1 byte per row × 8 rows)
- Total: `numChars × bytesPerGlyph`

### 6.6 Extraction Algorithm

```
1. Load _head.bin, _attr.bin, _img.bin for the font
2. Decompress each file if LZ77 compressed
3. Parse header to get character count and glyph dimensions
4. Parse attributes to get character codes and widths
5. Calculate bytes per glyph: (width × height) / 8
6. For each glyph:
   a. Read bytesPerGlyph bytes from image data
   b. Decode 1bpp pixels (8 pixels per byte, MSB first)
   c. Map bit values to colors (0=transparent, 1=white)
7. Arrange glyphs in spritesheet grid (typically 16 columns)
8. Export as PNG with transparency
```

### 6.7 Character Set (fontA)

fontA contains 224 characters starting at ASCII 0x20:

- 0x20-0x7E: Standard ASCII (space through tilde)
- 0x7F-0xFF: Extended characters (accented letters, symbols, currency)

The character map file lists the exact mapping of glyph index to character code.

---

## References

- GBATEK: NDS Technical Documentation
- Nintendo DS Architecture Documentation
- Nitro File System Specification
- ACWW Modding Community Research

---

## Appendix A: Quick Reference Tables

### ROM Header Key Offsets

| Purpose | Offset | Size |
|---------|--------|------|
| FNT Offset | 0x40 | 4 |
| FNT Size | 0x44 | 4 |
| FAT Offset | 0x48 | 4 |
| FAT Size | 0x4C | 4 |

### TEX0 Key Offsets (relative to TEX0 start)

| Purpose | Offset | Size |
|---------|--------|------|
| Texture Info Offset | 0x0E | 2 |
| Texture Data Offset | 0x14 | 4 |
| Palette Info Offset | 0x20 | 4 |
| Palette Data Offset | 0x24 | 4 |

### Texture Format Quick Reference

| ID | Name | BPP | Palette Size |
|----|------|-----|--------------|
| 1 | A3I5 | 8 | 32 colors |
| 2 | Palette4 | 2 | 4 colors |
| 3 | Palette16 | 4 | 16 colors |
| 4 | Palette256 | 8 | 256 colors |
| 6 | A5I3 | 8 | 8 colors |
| 7 | Direct | 16 | N/A |
