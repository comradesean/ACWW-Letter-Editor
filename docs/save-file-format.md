# ACWW Save File Format Documentation

This document details the binary structure of Animal Crossing: Wild World save files (.sav), specifically focusing on letter data.

## Letter Structure

Each letter is **244 bytes (0xF4)** long.

### Letter Field Offsets

| Offset | Size | Field |
|--------|------|-------|
| `0x00-0x13` | 20 bytes | To Field (recipient name + town) |
| `0x14` | 1 byte | Receiver's town save slot position (0x00-0x03) |
| `0x14-0x17` | 4 bytes | Letter type flags (see below) |
| `0x18-0x2B` | 20 bytes | From Field (sender name + town) |
| `0x2C` | 1 byte | Sender's town save slot position (0x00-0x03) |
| `0x2C-0x2F` | 4 bytes | Sender type flags (see below) |
| `0x30-0x3F` | 16 bytes | Greeting/Subject Field |
| `0x3F-0x47` | 8 bytes | Unused (always `00 00 00 00 00 00 00 00`) |
| `0x48-0xAB` | 100 bytes | Letter Body Text |
| `0xAC-0xC7` | 28 bytes | Unused (always zeros) |
| `0xC8-0xE1` | 26 bytes | Signature Field |
| `0xE2-0xE7` | 6 bytes | Unused (always zeros) |
| `0xE8` | 1 byte | Name position in greeting (0x00-0x10) |
| `0xE9` | 1 byte | Stationery type (see table below) |
| `0xEA` | 1 byte | Letter status (read/unread) |
| `0xEB` | 1 byte | Letter origin type |
| `0xEC-0xED` | 2 bytes | Attached item hex code (little-endian) |
| `0xEE-0xF3` | 6 bytes | Unused (always zeros) |

### Letter Type Flags (0x14-0x17)

| Hex Value | Meaning |
|-----------|---------|
| `00 00 03 00` | Written Letters (Not Sent) |
| `00 00 06 00` | Bottled Mail (Received) |
| `01 00 02 00` | Valentine's Day Letter (Untouched) |
| `00 00 02 00` | Everything Else |

### Sender Type Flags (0x2C-0x2F)

| Hex Value | Meaning |
|-----------|---------|
| `00 00 04 00` | Bottle Mail |
| `01 00 02 00` | Tortimer President Letter |
| `00 00 02 00` | Everything Else |

### Letter Status (0xEA)

| Value | Meaning |
|-------|---------|
| `0x42` | Untouched/Unread letter |
| `0x03` | Opened letter |
| `0x06` | Opened bottled letter |

### Letter Origin Type (0xEB)

| Value | Meaning |
|-------|---------|
| `0x11` | Nintendo Letters |
| `0x0C` | Bottled Letter |
| `0x00` | Written Letters (Not Sent) |

---

## Stationery Types

| Item ID | Hex | Name |
|---------|-----|------|
| 1000 | 0x00 | Butterfly paper |
| 1004 | 0x01 | Airmail paper |
| 1008 | 0x02 | New year's cards |
| 100C | 0x03 | Lacy paper |
| 1010 | 0x04 | Cloudy paper |
| 1014 | 0x05 | Petal paper |
| 1018 | 0x06 | Snowy paper |
| 101C | 0x07 | Maple-leaf paper |
| 1020 | 0x08 | Lined paper |
| 1024 | 0x09 | Notebook paper |
| 1028 | 0x0A | Flowery paper |
| 102C | 0x0B | Polka-dot paper |
| 1030 | 0x0C | Bottle paper (not in catalog) |
| 1034 | 0x0D | Ribbon paper |
| 1038 | 0x0E | Sparkly paper |
| 103C | 0x0F | Vine paper |
| 1040 | 0x10 | Formal paper |
| 1044 | 0x11 | Snowman paper |
| 1048 | 0x12 | Card paper |
| 104C | 0x13 | Leopard paper |
| 1050 | 0x14 | Cow paper |
| 1054 | 0x15 | Camouflage paper |
| 1058 | 0x16 | Hamburger paper |
| 105C | 0x17 | Piano paper |
| 1060 | 0x18 | Nook paper |
| 1064 | 0x19 | Fox paper |
| 1068 | 0x1A | Birthday cards |
| 106C | 0x1B | Four-leaf paper |
| 1070 | 0x1C | Town-hall paper |
| 1074 | 0x1D | Tortimer paper |
| 1078 | 0x1E | Insurance paper |
| 107C | 0x1F | Academy paper |
| 1080 | 0x20 | Lovely paper |
| 1084 | 0x21 | Rainbow paper |
| 1088 | 0x22 | Egyptian paper |
| 108C | 0x23 | Lotus paper |
| 1090 | 0x24 | Tile paper |
| 1094 | 0x25 | Mosaic paper |
| 1098 | 0x26 | Elegant paper |
| 109C | 0x27 | Town view paper |
| 10A0 | 0x28 | Chinese paper |
| 10A4 | 0x29 | Ocean paper |
| 10A8 | 0x2A | Industrial paper |
| 10AC | 0x2B | Fireworks paper |
| 10B0 | 0x2C | Floral paper |
| 10B4 | 0x2D | Mushroom paper |
| 10B8 | 0x2E | Star paper |
| 10BC | 0x2F | Composer paper |
| 10C0 | 0x30 | Bathtub paper |
| 10C4 | 0x31 | SMB3 paper |
| 10C8 | 0x32 | Cool paper |
| 10CC | 0x33 | Forest paper |
| 10D0 | 0x34 | Bubble paper |
| 10D4 | 0x35 | Buttercup paper |
| 10D8 | 0x36 | Tartan paper |
| 10DC | 0x37 | Plaid paper |
| 10E0 | 0x38 | Lemon-lime paper |
| 10E4 | 0x39 | Crater paper |
| 10E8 | 0x3A | Bejeweled paper |
| 10EC | 0x3B | Geometric paper |
| 10F0 | 0x3C | Southwest paper |
| 10F4 | 0x3D | Night sky paper |
| 10F8 | 0x3E | Chic paper |
| 10FC | 0x3F | Goldfish paper |

---

## Letter Storage Locations

### Player Inventory Letters (10 slots each)

Letters carried in player's pockets.

| Player | Address Range | Backup Save Range |
|--------|--------------|-------------------|
| Player 1 | `0x1158-0x1ADF` | `0x17138-0x17ABF` |
| Player 2 | `0x33E4-0x3D6B` | `0x193C4-0x19D4B` |
| Player 3 | `0x5670-0x5FF7` | `0x1B650-0x1BFD7` |
| Player 4 | `0x78FC-0x8283` | `0x1D8DC-0x1E263` |

#### Player 1 Inventory Slots

| Slot | Start | End |
|------|-------|-----|
| 1 | 0x1158 | 0x124B |
| 2 | 0x124C | 0x133F |
| 3 | 0x1340 | 0x1433 |
| 4 | 0x1434 | 0x1527 |
| 5 | 0x1528 | 0x161B |
| 6 | 0x161C | 0x170F |
| 7 | 0x1710 | 0x1803 |
| 8 | 0x1804 | 0x18F7 |
| 9 | 0x18F8 | 0x19EB |
| 10 | 0x19EC | 0x1ADF |

#### Player 2 Inventory Slots

| Slot | Start | End |
|------|-------|-----|
| 1 | 0x33E4 | 0x34D7 |
| 2 | 0x34D8 | 0x35CB |
| 3 | 0x35CC | 0x36BF |
| 4 | 0x36C0 | 0x37B3 |
| 5 | 0x37B4 | 0x38A7 |
| 6 | 0x38A8 | 0x399B |
| 7 | 0x399C | 0x3A8F |
| 8 | 0x3A90 | 0x3B83 |
| 9 | 0x3B84 | 0x3C77 |
| 10 | 0x3C78 | 0x3D6B |

#### Player 3 Inventory Slots

| Slot | Start | End |
|------|-------|-----|
| 1 | 0x5670 | 0x5763 |
| 2 | 0x5764 | 0x5857 |
| 3 | 0x5858 | 0x594B |
| 4 | 0x594C | 0x5A3F |
| 5 | 0x5A40 | 0x5B33 |
| 6 | 0x5B34 | 0x5C27 |
| 7 | 0x5C28 | 0x5D1B |
| 8 | 0x5D1C | 0x5E0F |
| 9 | 0x5E10 | 0x5F03 |
| 10 | 0x5F04 | 0x5FF7 |

#### Player 4 Inventory Slots

| Slot | Start | End |
|------|-------|-----|
| 1 | 0x78FC | 0x79EF |
| 2 | 0x79F0 | 0x7AE3 |
| 3 | 0x7AE4 | 0x7BD7 |
| 4 | 0x7BD8 | 0x7CCB |
| 5 | 0x7CCC | 0x7DBF |
| 6 | 0x7DC0 | 0x7EB3 |
| 7 | 0x7EB4 | 0x7FA7 |
| 8 | 0x7FA8 | 0x809B |
| 9 | 0x809C | 0x818F |
| 10 | 0x8190 | 0x8283 |

---

### Mailbox Letters (10 slots each)

Letters stored in player's home mailbox. Each mailbox section ends with a 4-byte buffer (`00 00 00 00`).

| Player | Address Range | Backup Save Range |
|--------|--------------|-------------------|
| Player 1 | `0x12010-0x12997` | `0x27FF0-0x28977` |
| Player 2 | `0x1299C-0x13323` | `0x2897C-0x29303` |
| Player 3 | `0x13328-0x13CAF` | `0x29308-0x29C8F` |
| Player 4 | `0x13CB4-0x1463B` | `0x29C94-0x2A61B` |

#### Player 1 Mailbox Slots

| Slot | Start | End |
|------|-------|-----|
| 1 | 0x12010 | 0x12103 |
| 2 | 0x12104 | 0x121F7 |
| 3 | 0x121F8 | 0x122EB |
| 4 | 0x122EC | 0x123DF |
| 5 | 0x123E0 | 0x124D3 |
| 6 | 0x124D4 | 0x125C7 |
| 7 | 0x125C8 | 0x126BB |
| 8 | 0x126BC | 0x127AF |
| 9 | 0x127B0 | 0x128A3 |
| 10 | 0x128A4 | 0x12997 |

#### Player 2 Mailbox Slots

| Slot | Start | End |
|------|-------|-----|
| 1 | 0x1299C | 0x12A8F |
| 2 | 0x12A90 | 0x12B83 |
| 3 | 0x12B84 | 0x12C77 |
| 4 | 0x12C78 | 0x12D6B |
| 5 | 0x12D6C | 0x12E5F |
| 6 | 0x12E60 | 0x12F53 |
| 7 | 0x12F54 | 0x13047 |
| 8 | 0x13048 | 0x1313B |
| 9 | 0x1313C | 0x1322F |
| 10 | 0x13230 | 0x13323 |

#### Player 3 Mailbox Slots

| Slot | Start | End |
|------|-------|-----|
| 1 | 0x13328 | 0x1341B |
| 2 | 0x1341C | 0x1350F |
| 3 | 0x13510 | 0x13603 |
| 4 | 0x13604 | 0x136F7 |
| 5 | 0x136F8 | 0x137EB |
| 6 | 0x137EC | 0x138DF |
| 7 | 0x138E0 | 0x139D3 |
| 8 | 0x139D4 | 0x13AC7 |
| 9 | 0x13AC8 | 0x13BBB |
| 10 | 0x13BBC | 0x13CAF |

#### Player 4 Mailbox Slots

| Slot | Start | End |
|------|-------|-----|
| 1 | 0x13CB4 | 0x13DA7 |
| 2 | 0x13DA8 | 0x13E9B |
| 3 | 0x13E9C | 0x13F8F |
| 4 | 0x13F90 | 0x14083 |
| 5 | 0x14084 | 0x14177 |
| 6 | 0x14178 | 0x1426B |
| 7 | 0x1426C | 0x1435F |
| 8 | 0x14360 | 0x14453 |
| 9 | 0x14454 | 0x14547 |
| 10 | 0x14548 | 0x1463B |

---

### Bank Storage Letters (75 slots each, 3 pages of 25)

Letters stored at the town hall post office. Total range: `0x23210-0x3FFFF`

| Player | Address Range |
|--------|--------------|
| Player 1 | `0x23210-0x3298B` |
| Player 2 | `0x3298C-0x37107` |
| Player 3 | `0x37108-0x3B883` |
| Player 4 | `0x3B884-0x3FFFF` |

#### Player 1 Bank Storage

**Page 1 (Slots 1-25): `0x2E210-0x2F9E3`**

| Slot | Start | End |
|------|-------|-----|
| 1 | 0x2E210 | 0x2E303 |
| 2 | 0x2E304 | 0x2E3F7 |
| 3 | 0x2E3F8 | 0x2E4EB |
| 4 | 0x2E4EC | 0x2E5DF |
| 5 | 0x2E5E0 | 0x2E6D3 |
| 6 | 0x2E6D4 | 0x2E7C7 |
| 7 | 0x2E7C8 | 0x2E8BB |
| 8 | 0x2E8BC | 0x2E9AF |
| 9 | 0x2E9B0 | 0x2EAA3 |
| 10 | 0x2EAA4 | 0x2EB97 |
| 11 | 0x2EB98 | 0x2EC8B |
| 12 | 0x2EC8C | 0x2ED7F |
| 13 | 0x2ED80 | 0x2EE73 |
| 14 | 0x2EE74 | 0x2EF67 |
| 15 | 0x2EF68 | 0x2F05B |
| 16 | 0x2F05C | 0x2F14F |
| 17 | 0x2F150 | 0x2F243 |
| 18 | 0x2F244 | 0x2F337 |
| 19 | 0x2F338 | 0x2F42B |
| 20 | 0x2F42C | 0x2F51F |
| 21 | 0x2F520 | 0x2F613 |
| 22 | 0x2F614 | 0x2F707 |
| 23 | 0x2F708 | 0x2F7FB |
| 24 | 0x2F7FC | 0x2F8EF |
| 25 | 0x2F8F0 | 0x2F9E3 |

**Page 2 (Slots 26-50): `0x2F9E4-0x311B7`**

| Slot | Start | End |
|------|-------|-----|
| 26 | 0x2F9E4 | 0x2FAD7 |
| 27 | 0x2FAD8 | 0x2FBCB |
| 28 | 0x2FBCC | 0x2FCBF |
| 29 | 0x2FCC0 | 0x2FDB3 |
| 30 | 0x2FDB4 | 0x2FEA7 |
| 31 | 0x2FEA8 | 0x2FF9B |
| 32 | 0x2FF9C | 0x3008F |
| 33 | 0x30090 | 0x30183 |
| 34 | 0x30184 | 0x30277 |
| 35 | 0x30278 | 0x3036B |
| 36 | 0x3036C | 0x3045F |
| 37 | 0x30460 | 0x30553 |
| 38 | 0x30554 | 0x30647 |
| 39 | 0x30648 | 0x3073B |
| 40 | 0x3073C | 0x3082F |
| 41 | 0x30830 | 0x30923 |
| 42 | 0x30924 | 0x30A17 |
| 43 | 0x30A18 | 0x30B0B |
| 44 | 0x30B0C | 0x30BFF |
| 45 | 0x30C00 | 0x30CF3 |
| 46 | 0x30CF4 | 0x30DE7 |
| 47 | 0x30DE8 | 0x30EDB |
| 48 | 0x30EDC | 0x30FCF |
| 49 | 0x30FD0 | 0x310C3 |
| 50 | 0x310C4 | 0x311B7 |

**Page 3 (Slots 51-75): `0x311B8-0x3298B`**

| Slot | Start | End |
|------|-------|-----|
| 51 | 0x311B8 | 0x312AB |
| 52 | 0x312AC | 0x3139F |
| 53 | 0x313A0 | 0x31493 |
| 54 | 0x31494 | 0x31587 |
| 55 | 0x31588 | 0x3167B |
| 56 | 0x3167C | 0x3176F |
| 57 | 0x31770 | 0x31863 |
| 58 | 0x31864 | 0x31957 |
| 59 | 0x31958 | 0x31A4B |
| 60 | 0x31A4C | 0x31B3F |
| 61 | 0x31B40 | 0x31C33 |
| 62 | 0x31C34 | 0x31D27 |
| 63 | 0x31D28 | 0x31E1B |
| 64 | 0x31E1C | 0x31F0F |
| 65 | 0x31F10 | 0x32003 |
| 66 | 0x32004 | 0x320F7 |
| 67 | 0x320F8 | 0x321EB |
| 68 | 0x321EC | 0x322DF |
| 69 | 0x322E0 | 0x323D3 |
| 70 | 0x323D4 | 0x324C7 |
| 71 | 0x324C8 | 0x325BB |
| 72 | 0x325BC | 0x326AF |
| 73 | 0x326B0 | 0x327A3 |
| 74 | 0x327A4 | 0x32897 |
| 75 | 0x32898 | 0x3298B |

---

## Known Sender Examples

| Sender | Hex Data |
|--------|----------|
| Nintendo from Redmond | `91 F2 12 1F 1E 27 29 28 1E 00 C9 F9 0E 23 28 2E 1F 28 1E 29` (slot 0x00) |
| Tortimer | `91 F2 12 1F 1E 27 29 28 1E 00 D8 B1 14 29 2C 2E 23 27 1F 2C` (slot 0x01) |
| Sean from Bastok | `17 F5 02 1B 2D 2E 29 25 00 00 A4 E4 13 1F 1B 28 00 00 00 00` |

---

## Notes

- The save file contains both primary and backup copies of letter data
- There is a header region at `0x1124-0x1157` before inventory letters
- Mailbox slot 10 sometimes ends with `00 00 33 00 00 00` instead of zeros
- A 4-byte buffer (`00 00 00 00`) separates each player's mailbox section
- Item hex codes reference: https://animalcrossing1097.wixsite.com/animalcrossingblog/acww_ar_item_hex_codes
