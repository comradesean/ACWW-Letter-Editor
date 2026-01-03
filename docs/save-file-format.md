# Animal Crossing: Wild World - Save File Format

Technical documentation for the binary structure of ACWW save files (`.sav`), focusing on letter data for the **EUR/USA region**.

---

## Table of Contents

1. [Overview](#overview)
2. [Letter Structure](#letter-structure)
3. [Letter Storage Locations](#letter-storage-locations)
4. [Stationery Reference](#stationery-reference)
5. [Appendix](#appendix)

---

## Overview

### Key Constants

| Property | Value | Description |
|----------|-------|-------------|
| Letter size | `0xF4` (244 bytes) | Total size of each letter record |
| Header | 4 bytes (`0x00-0x03`) | Always `00 00 00 00` |
| Footer | 2 bytes (`0xF2-0xF3`) | Always `00 00` |
| Greeting field | `0x18` (24 bytes) | Subject/greeting line |
| Body field | `0x80` (128 bytes) | Main letter content |
| Signature field | `0x20` (32 bytes) | Letter closing/signature |

### Important Notes

- All offsets in this document are for **EUR/USA region** save files
- The save file contains both **primary** and **backup** copies of letter data
- All field offsets within the letter structure are **relative to the start of each letter**
- All save file offsets in this document have been corrected by -4 bytes from older documentation

---

## Letter Structure

Each letter occupies exactly **244 bytes (`0xF4`)**, structured as follows:

### Field Layout

| Offset | Size | Field | Description |
|--------|------|-------|-------------|
| `0x00-0x03` | 4 bytes | Header | Always `00 00 00 00` |
| `0x04-0x05` | 2 bytes | Recipient Town ID | Unique identifier for recipient's town |
| `0x06-0x0D` | 8 bytes | Recipient Town Name | ACWW-encoded town name |
| `0x0E-0x0F` | 2 bytes | Recipient Player ID | Unique identifier for recipient |
| `0x10-0x17` | 8 bytes | Recipient Player Name | ACWW-encoded player name |
| `0x18-0x1B` | 4 bytes | Receiver Type Flags | See [Receiver Type Flags](#receiver-type-flags) |
| `0x1C-0x1D` | 2 bytes | Sender Town ID | Unique identifier for sender's town |
| `0x1E-0x25` | 8 bytes | Sender Town Name | ACWW-encoded town name |
| `0x26-0x27` | 2 bytes | Sender Player ID | Unique identifier for sender |
| `0x28-0x2F` | 8 bytes | Sender Player Name | ACWW-encoded player name |
| `0x30-0x33` | 4 bytes | Sender Type Flags | See [Sender Type Flags](#sender-type-flags) |
| `0x34-0x4B` | 24 bytes (`0x18`) | Greeting | Subject/greeting line |
| `0x4C-0xCB` | 128 bytes (`0x80`) | Body | Main letter content |
| `0xCC-0xEB` | 32 bytes (`0x20`) | Signature | Letter closing/signature |
| `0xEC` | 1 byte | Name Position | Position of recipient name in greeting (`0x00-0x18`, i.e. 0-24) |
| `0xED` | 1 byte | Stationery ID | Paper background (0-63); see [Stationery Reference](#stationery-reference) |
| `0xEE` | 1 byte | Letter Icon/Flags | Icon and state flags; see [Letter Icon/Flags](#letter-iconflags) |
| `0xEF` | 1 byte | Letter Source | NPC/system sender; see [Letter Source](#letter-source) |
| `0xF0-0xF1` | 2 bytes | Attached Item | Item hex code (little-endian); `0xFFF1` = no item |
| `0xF2-0xF3` | 2 bytes | Footer | Always `00 00` |

### Visual Layout

```
+------------------+------------------+------------------+------------------+
| 0x00-0x03        | 0x04-0x1B        | 0x1C-0x33        | 0x34-0xEB        |
| Header (4B)      | Recipient (24B)  | Sender (24B)     | Content (184B)   |
+------------------+------------------+------------------+------------------+
| 0xEC-0xF1        | 0xF2-0xF3        |
| Metadata (6B)    | Footer (2B)      |
+-----------------+------------------+
```

---

### Receiver Type Flags

Located at offset `0x18-0x1B` (4 bytes).

| Hex Value | Meaning |
|-----------|---------|
| `00 00 03 00` | Written letter (not sent) |
| `00 00 06 00` | Bottled mail (received) |
| `01 00 02 00` | Valentine's Day letter (untouched) |
| `00 00 02 00` | All other letters |

---

### Sender Type Flags

Located at offset `0x30-0x33` (4 bytes).

| Hex Value | Meaning |
|-----------|---------|
| `00 00 04 00` | Bottle mail |
| `01 00 02 00` | Tortimer President letter |
| `00 00 02 00` | All other letters |

---

### Letter Icon/Flags

Located at offset `0xEE` (1 byte).

**Base Icon Values:**

| Value | Icon | Meaning |
|-------|------|---------|
| `0x00` | - | Non-existent letter (should only appear in zeroed-out slots) |
| `0x01` | White | Letter being written, not yet mailed |
| `0x02` | White sealed | Received letter, unopened |
| `0x03` | White open | Received letter, opened |
| `0x04` | Bottle | Bottle Mail being written |
| `0x05` | Bottle sealed | Received Bottle Mail, unopened |
| `0x06` | Bottle open | Received Bottle Mail, opened |
| `0x07` | Green sealed | Villager favor letter (Letter Delivery), unopened |
| `0x08` | Green open | Villager favor letter, opened |

**Attachment Flag:**

| Flag | Effect |
|------|--------|
| `+ 0x40` | Received letter has attachment (e.g., `0x02` + `0x40` = `0x42`) |

Only applies to received/delivered letters (`0x02`, `0x03`, `0x05`*, `0x06`*, `0x07`*, `0x08`*). Outgoing letters (`0x01`, `0x04`*) do not use this flag.

*`0x04`-`0x08` believed to follow the same rules but not observed in-game.

**Notes:**
- `0x00`, `0x40`, `0x80`, `0xC0` appear identical, but `0x80` and `0xC0` have not been observed in-game
- Values not listed above result in glitchy icons

---

### Letter Source

Located at offset `0xEF` (1 byte).

Identifies the NPC or system that sent the letter. Most NPC sources have a unique identifier.

| Value | Source |
|-------|--------|
| `0x00` | Players, Villagers or Bottle (e.g., Katrina's "Dear Some Stranger,") |
| `0x01` | Mom |
| `0x02` | H.R.A. (Happy Room Academy) |
| `0x03` | Tom Nook |
| `0x04` | Museum |
| `0x06` | Town Hall |
| `0x07` | Lyle (Insurance) |
| `0x0A` | Snowman |
| `0x0B` | Redd |
| `0x0C` | Bottle (e.g., Hayseed Hilda's "I Crave Apples") |
| `0x11` | Nintendo / S. Iwata / Tortimer |

---

## Letter Storage Locations

Letters are stored in three locations per player:

| Storage Type | Slots per Player | Description |
|--------------|------------------|-------------|
| Inventory | 10 | Letters carried in player's pockets |
| Mailbox | 10 | Letters stored at player's home mailbox |
| Bank Storage | 75 (3 pages of 25) | Letters stored at town hall post office |

### Address Calculation

To calculate any letter slot address:

```
slot_address = base_address + (slot_index * 0xF4)
```

Where `0xF4` (244) is the letter size in bytes.

---

### Inventory Letters

Letters carried in the player's pockets (10 slots per player).

#### Summary Table

| Player | Primary Range | Backup Range |
|--------|---------------|--------------|
| Player 1 | `0x01154` - `0x01ADB` | `0x17134` - `0x17ABB` |
| Player 2 | `0x033E0` - `0x03D67` | `0x193C0` - `0x19D47` |
| Player 3 | `0x0566C` - `0x05FF3` | `0x1B64C` - `0x1BFD3` |
| Player 4 | `0x078F8` - `0x0827F` | `0x1D8D8` - `0x1E25F` |

#### Player 1 Inventory Slots

| Slot | Start | End |
|------|-------|-----|
| 1 | `0x01154` | `0x01247` |
| 2 | `0x01248` | `0x0133B` |
| 3 | `0x0133C` | `0x0142F` |
| 4 | `0x01430` | `0x01523` |
| 5 | `0x01524` | `0x01617` |
| 6 | `0x01618` | `0x0170B` |
| 7 | `0x0170C` | `0x017FF` |
| 8 | `0x01800` | `0x018F3` |
| 9 | `0x018F4` | `0x019E7` |
| 10 | `0x019E8` | `0x01ADB` |

#### Player 2 Inventory Slots

| Slot | Start | End |
|------|-------|-----|
| 1 | `0x033E0` | `0x034D3` |
| 2 | `0x034D4` | `0x035C7` |
| 3 | `0x035C8` | `0x036BB` |
| 4 | `0x036BC` | `0x037AF` |
| 5 | `0x037B0` | `0x038A3` |
| 6 | `0x038A4` | `0x03997` |
| 7 | `0x03998` | `0x03A8B` |
| 8 | `0x03A8C` | `0x03B7F` |
| 9 | `0x03B80` | `0x03C73` |
| 10 | `0x03C74` | `0x03D67` |

#### Player 3 Inventory Slots

| Slot | Start | End |
|------|-------|-----|
| 1 | `0x0566C` | `0x0575F` |
| 2 | `0x05760` | `0x05853` |
| 3 | `0x05854` | `0x05947` |
| 4 | `0x05948` | `0x05A3B` |
| 5 | `0x05A3C` | `0x05B2F` |
| 6 | `0x05B30` | `0x05C23` |
| 7 | `0x05C24` | `0x05D17` |
| 8 | `0x05D18` | `0x05E0B` |
| 9 | `0x05E0C` | `0x05EFF` |
| 10 | `0x05F00` | `0x05FF3` |

#### Player 4 Inventory Slots

| Slot | Start | End |
|------|-------|-----|
| 1 | `0x078F8` | `0x079EB` |
| 2 | `0x079EC` | `0x07ADF` |
| 3 | `0x07AE0` | `0x07BD3` |
| 4 | `0x07BD4` | `0x07CC7` |
| 5 | `0x07CC8` | `0x07DBB` |
| 6 | `0x07DBC` | `0x07EAF` |
| 7 | `0x07EB0` | `0x07FA3` |
| 8 | `0x07FA4` | `0x08097` |
| 9 | `0x08098` | `0x0818B` |
| 10 | `0x0818C` | `0x0827F` |

---

### Mailbox Letters

Letters stored in the player's home mailbox (10 slots per player).

#### Summary Table

| Player | Primary Range | Backup Range |
|--------|---------------|--------------|
| Player 1 | `0x1200C` - `0x12993` | `0x27FEC` - `0x28973` |
| Player 2 | `0x12998` - `0x1331F` | `0x28978` - `0x292FF` |
| Player 3 | `0x13324` - `0x13CAB` | `0x29304` - `0x29C8B` |
| Player 4 | `0x13CB0` - `0x14637` | `0x29C90` - `0x2A617` |

#### Player 1 Mailbox Slots

| Slot | Start | End |
|------|-------|-----|
| 1 | `0x1200C` | `0x120FF` |
| 2 | `0x12100` | `0x121F3` |
| 3 | `0x121F4` | `0x122E7` |
| 4 | `0x122E8` | `0x123DB` |
| 5 | `0x123DC` | `0x124CF` |
| 6 | `0x124D0` | `0x125C3` |
| 7 | `0x125C4` | `0x126B7` |
| 8 | `0x126B8` | `0x127AB` |
| 9 | `0x127AC` | `0x1289F` |
| 10 | `0x128A0` | `0x12993` |

#### Player 2 Mailbox Slots

| Slot | Start | End |
|------|-------|-----|
| 1 | `0x12998` | `0x12A8B` |
| 2 | `0x12A8C` | `0x12B7F` |
| 3 | `0x12B80` | `0x12C73` |
| 4 | `0x12C74` | `0x12D67` |
| 5 | `0x12D68` | `0x12E5B` |
| 6 | `0x12E5C` | `0x12F4F` |
| 7 | `0x12F50` | `0x13043` |
| 8 | `0x13044` | `0x13137` |
| 9 | `0x13138` | `0x1322B` |
| 10 | `0x1322C` | `0x1331F` |

#### Player 3 Mailbox Slots

| Slot | Start | End |
|------|-------|-----|
| 1 | `0x13324` | `0x13417` |
| 2 | `0x13418` | `0x1350B` |
| 3 | `0x1350C` | `0x135FF` |
| 4 | `0x13600` | `0x136F3` |
| 5 | `0x136F4` | `0x137E7` |
| 6 | `0x137E8` | `0x138DB` |
| 7 | `0x138DC` | `0x139CF` |
| 8 | `0x139D0` | `0x13AC3` |
| 9 | `0x13AC4` | `0x13BB7` |
| 10 | `0x13BB8` | `0x13CAB` |

#### Player 4 Mailbox Slots

| Slot | Start | End |
|------|-------|-----|
| 1 | `0x13CB0` | `0x13DA3` |
| 2 | `0x13DA4` | `0x13E97` |
| 3 | `0x13E98` | `0x13F8B` |
| 4 | `0x13F8C` | `0x1407F` |
| 5 | `0x14080` | `0x14173` |
| 6 | `0x14174` | `0x14267` |
| 7 | `0x14268` | `0x1435B` |
| 8 | `0x1435C` | `0x1444F` |
| 9 | `0x14450` | `0x14543` |
| 10 | `0x14544` | `0x14637` |

---

### Bank Storage Letters

Letters stored at the town hall post office (75 slots per player, organized as 3 pages of 25).

#### Address Calculation

```
player_base = 0x2E20C + (player_index * 0x4778)
slot_address = player_base + (slot_index * 0xF4)
```

Where:
- `player_index` = 0-3 (for Players 1-4)
- `slot_index` = 0-74 (for slots 1-75)
- `0x4778` = player stride (18,296 bytes between players)
- `0xF4` = letter size (244 bytes)

#### Summary Table

| Player | Address Range |
|--------|---------------|
| Player 1 | `0x2E20C` - `0x32987` |
| Player 2 | `0x32984` - `0x370FB` |
| Player 3 | `0x370FC` - `0x3B873` |
| Player 4 | `0x3B874` - `0x3FFEB` |

#### Player 1 Bank Storage

**Page 1 (Slots 1-25): `0x2E20C` - `0x2F9DF`**

| Slot | Start | End |
|------|-------|-----|
| 1 | `0x2E20C` | `0x2E2FF` |
| 2 | `0x2E300` | `0x2E3F3` |
| 3 | `0x2E3F4` | `0x2E4E7` |
| 4 | `0x2E4E8` | `0x2E5DB` |
| 5 | `0x2E5DC` | `0x2E6CF` |
| 6 | `0x2E6D0` | `0x2E7C3` |
| 7 | `0x2E7C4` | `0x2E8B7` |
| 8 | `0x2E8B8` | `0x2E9AB` |
| 9 | `0x2E9AC` | `0x2EA9F` |
| 10 | `0x2EAA0` | `0x2EB93` |
| 11 | `0x2EB94` | `0x2EC87` |
| 12 | `0x2EC88` | `0x2ED7B` |
| 13 | `0x2ED7C` | `0x2EE6F` |
| 14 | `0x2EE70` | `0x2EF63` |
| 15 | `0x2EF64` | `0x2F057` |
| 16 | `0x2F058` | `0x2F14B` |
| 17 | `0x2F14C` | `0x2F23F` |
| 18 | `0x2F240` | `0x2F333` |
| 19 | `0x2F334` | `0x2F427` |
| 20 | `0x2F428` | `0x2F51B` |
| 21 | `0x2F51C` | `0x2F60F` |
| 22 | `0x2F610` | `0x2F703` |
| 23 | `0x2F704` | `0x2F7F7` |
| 24 | `0x2F7F8` | `0x2F8EB` |
| 25 | `0x2F8EC` | `0x2F9DF` |

**Page 2 (Slots 26-50): `0x2F9E0` - `0x311B3`**

| Slot | Start | End |
|------|-------|-----|
| 26 | `0x2F9E0` | `0x2FAD3` |
| 27 | `0x2FAD4` | `0x2FBC7` |
| 28 | `0x2FBC8` | `0x2FCBB` |
| 29 | `0x2FCBC` | `0x2FDAF` |
| 30 | `0x2FDB0` | `0x2FEA3` |
| 31 | `0x2FEA4` | `0x2FF97` |
| 32 | `0x2FF98` | `0x3008B` |
| 33 | `0x3008C` | `0x3017F` |
| 34 | `0x30180` | `0x30273` |
| 35 | `0x30274` | `0x30367` |
| 36 | `0x30368` | `0x3045B` |
| 37 | `0x3045C` | `0x3054F` |
| 38 | `0x30550` | `0x30643` |
| 39 | `0x30644` | `0x30737` |
| 40 | `0x30738` | `0x3082B` |
| 41 | `0x3082C` | `0x3091F` |
| 42 | `0x30920` | `0x30A13` |
| 43 | `0x30A14` | `0x30B07` |
| 44 | `0x30B08` | `0x30BFB` |
| 45 | `0x30BFC` | `0x30CEF` |
| 46 | `0x30CF0` | `0x30DE3` |
| 47 | `0x30DE4` | `0x30ED7` |
| 48 | `0x30ED8` | `0x30FCB` |
| 49 | `0x30FCC` | `0x310BF` |
| 50 | `0x310C0` | `0x311B3` |

**Page 3 (Slots 51-75): `0x311B4` - `0x32987`**

| Slot | Start | End |
|------|-------|-----|
| 51 | `0x311B4` | `0x312A7` |
| 52 | `0x312A8` | `0x3139B` |
| 53 | `0x3139C` | `0x3148F` |
| 54 | `0x31490` | `0x31583` |
| 55 | `0x31584` | `0x31677` |
| 56 | `0x31678` | `0x3176B` |
| 57 | `0x3176C` | `0x3185F` |
| 58 | `0x31860` | `0x31953` |
| 59 | `0x31954` | `0x31A47` |
| 60 | `0x31A48` | `0x31B3B` |
| 61 | `0x31B3C` | `0x31C2F` |
| 62 | `0x31C30` | `0x31D23` |
| 63 | `0x31D24` | `0x31E17` |
| 64 | `0x31E18` | `0x31F0B` |
| 65 | `0x31F0C` | `0x31FFF` |
| 66 | `0x32000` | `0x320F3` |
| 67 | `0x320F4` | `0x321E7` |
| 68 | `0x321E8` | `0x322DB` |
| 69 | `0x322DC` | `0x323CF` |
| 70 | `0x323D0` | `0x324C3` |
| 71 | `0x324C4` | `0x325B7` |
| 72 | `0x325B8` | `0x326AB` |
| 73 | `0x326AC` | `0x3279F` |
| 74 | `0x327A0` | `0x32893` |
| 75 | `0x32894` | `0x32987` |

---

## Stationery Reference

The stationery ID at offset `0xED` maps to one of 64 paper backgrounds (values 0-63).

| ID | Hex | Item Hex | Name |
|----|-----|----------|------|
| 0 | `0x00` | `0x1000` | Butterfly paper |
| 1 | `0x01` | `0x1004` | Airmail paper |
| 2 | `0x02` | `0x1008` | New year's cards |
| 3 | `0x03` | `0x100C` | Lacy paper |
| 4 | `0x04` | `0x1010` | Cloudy paper |
| 5 | `0x05` | `0x1014` | Petal paper |
| 6 | `0x06` | `0x1018` | Snowy paper |
| 7 | `0x07` | `0x101C` | Maple-leaf paper |
| 8 | `0x08` | `0x1020` | Lined paper |
| 9 | `0x09` | `0x1024` | Notebook paper |
| 10 | `0x0A` | `0x1028` | Flowery paper |
| 11 | `0x0B` | `0x102C` | Polka-dot paper |
| 12 | `0x0C` | `0x1030` | Bottle paper (not in catalog) |
| 13 | `0x0D` | `0x1034` | Ribbon paper |
| 14 | `0x0E` | `0x1038` | Sparkly paper |
| 15 | `0x0F` | `0x103C` | Vine paper |
| 16 | `0x10` | `0x1040` | Formal paper |
| 17 | `0x11` | `0x1044` | Snowman paper |
| 18 | `0x12` | `0x1048` | Card paper |
| 19 | `0x13` | `0x104C` | Leopard paper |
| 20 | `0x14` | `0x1050` | Cow paper |
| 21 | `0x15` | `0x1054` | Camouflage paper |
| 22 | `0x16` | `0x1058` | Hamburger paper |
| 23 | `0x17` | `0x105C` | Piano paper |
| 24 | `0x18` | `0x1060` | Nook paper |
| 25 | `0x19` | `0x1064` | Fox paper |
| 26 | `0x1A` | `0x1068` | Birthday cards |
| 27 | `0x1B` | `0x106C` | Four-leaf paper |
| 28 | `0x1C` | `0x1070` | Town-hall paper |
| 29 | `0x1D` | `0x1074` | Tortimer paper |
| 30 | `0x1E` | `0x1078` | Insurance paper |
| 31 | `0x1F` | `0x107C` | Academy paper |
| 32 | `0x20` | `0x1080` | Lovely paper |
| 33 | `0x21` | `0x1084` | Rainbow paper |
| 34 | `0x22` | `0x1088` | Egyptian paper |
| 35 | `0x23` | `0x108C` | Lotus paper |
| 36 | `0x24` | `0x1090` | Tile paper |
| 37 | `0x25` | `0x1094` | Mosaic paper |
| 38 | `0x26` | `0x1098` | Elegant paper |
| 39 | `0x27` | `0x109C` | Town view paper |
| 40 | `0x28` | `0x10A0` | Chinese paper |
| 41 | `0x29` | `0x10A4` | Ocean paper |
| 42 | `0x2A` | `0x10A8` | Industrial paper |
| 43 | `0x2B` | `0x10AC` | Fireworks paper |
| 44 | `0x2C` | `0x10B0` | Floral paper |
| 45 | `0x2D` | `0x10B4` | Mushroom paper |
| 46 | `0x2E` | `0x10B8` | Star paper |
| 47 | `0x2F` | `0x10BC` | Composer paper |
| 48 | `0x30` | `0x10C0` | Bathtub paper |
| 49 | `0x31` | `0x10C4` | SMB3 paper |
| 50 | `0x32` | `0x10C8` | Cool paper |
| 51 | `0x33` | `0x10CC` | Forest paper |
| 52 | `0x34` | `0x10D0` | Bubble paper |
| 53 | `0x35` | `0x10D4` | Buttercup paper |
| 54 | `0x36` | `0x10D8` | Tartan paper |
| 55 | `0x37` | `0x10DC` | Plaid paper |
| 56 | `0x38` | `0x10E0` | Lemon-lime paper |
| 57 | `0x39` | `0x10E4` | Crater paper |
| 58 | `0x3A` | `0x10E8` | Bejeweled paper |
| 59 | `0x3B` | `0x10EC` | Geometric paper |
| 60 | `0x3C` | `0x10F0` | Southwest paper |
| 61 | `0x3D` | `0x10F4` | Night sky paper |
| 62 | `0x3E` | `0x10F8` | Chic paper |
| 63 | `0x3F` | `0x10FC` | Goldfish paper |

---


## Appendix

### Known Sender Examples

Example raw hex data showing sender information patterns:

| Sender | Raw Hex Data |
|--------|--------------|
| Nintendo (Redmond) | `91 F2 12 1F 1E 27 29 28 1E 00 C9 F9 0E 23 28 2E 1F 28 1E 29` |
| Tortimer | `91 F2 12 1F 1E 27 29 28 1E 00 D8 B1 14 29 2C 2E 23 27 1F 2C` |
| Player "Sean" from "Bastok" | `17 F5 02 1B 2D 2E 29 25 00 00 A4 E4 13 1F 1B 28 00 00 00 00` |

### Additional Notes

- **Header region**: There is a header region at `0x1120-0x1153` before inventory letters
- **Attached item field**: Located at offset `0xF0` (not `0xEC` as in some older documentation)
- **Item hex codes reference**: https://animalcrossing1097.wixsite.com/animalcrossingblog/acww_ar_item_hex_codes

### Hex Notation Convention

All hex values in this document use the `0x` prefix for clarity:
- Addresses: `0x1154` (not `1154` or `$1154`)
- Single bytes: `0x42` (not `42h` or `$42`)
- Multi-byte values: shown as space-separated bytes (e.g., `00 00 03 00`)
