#include "cloth_loader.h"
#include "nds_rom.h"
#include "lz77.h"
#include <cstring>

namespace {
    struct Color { uint8_t r, g, b, a; };

    Color decodeRGB555(uint16_t color) {
        Color c;
        c.r = ((color >> 0) & 0x1F) * 255 / 31;
        c.g = ((color >> 5) & 0x1F) * 255 / 31;
        c.b = ((color >> 10) & 0x1F) * 255 / 31;
        c.a = 255;
        return c;
    }

    uint16_t readU16LE(const uint8_t* data) {
        return static_cast<uint16_t>(data[0]) | (static_cast<uint16_t>(data[1]) << 8);
    }

    uint32_t readU32LE(const uint8_t* data) {
        return static_cast<uint32_t>(data[0]) |
               (static_cast<uint32_t>(data[1]) << 8) |
               (static_cast<uint32_t>(data[2]) << 16) |
               (static_cast<uint32_t>(data[3]) << 24);
    }
}

bool ClothLoader::load(NDSRom& rom) {
    m_loaded = false;
    m_texture = QImage();

    // Load cloth082.nsbtx from ROM
    std::string clothPath = "/cloth/5/cloth082.nsbtx";
    auto data = rom.readFile(clothPath);

    if (data.empty()) {
        return false;
    }

    // Handle LZ77 compression
    if (LZ77::isCompressed(data)) {
        data = LZ77::decompress(data);
        if (data.empty()) {
            return false;
        }
    }

    if (!parseNSBTX(data)) {
        return false;
    }

    m_loaded = true;
    return true;
}

bool ClothLoader::parseNSBTX(const std::vector<uint8_t>& data) {
    // Check minimum size for BTX0 header
    if (data.size() < 0x14) {
        return false;
    }

    // Check BTX0 magic at offset 0x00
    if (data[0] != 'B' || data[1] != 'T' || data[2] != 'X' || data[3] != '0') {
        return false;
    }

    // Read section count at 0x0E
    uint16_t sectionCount = readU16LE(&data[0x0E]);
    if (sectionCount < 1) {
        return false;
    }

    // Read TEX0 offset at 0x10
    uint32_t tex0Offset = readU32LE(&data[0x10]);
    if (tex0Offset >= data.size()) {
        return false;
    }

    // Parse TEX0 section
    const uint8_t* tex0 = &data[tex0Offset];
    size_t tex0Size = data.size() - tex0Offset;

    // Check TEX0 magic
    if (tex0Size < 0x30 || tex0[0] != 'T' || tex0[1] != 'E' || tex0[2] != 'X' || tex0[3] != '0') {
        return false;
    }

    // TEX0 structure:
    // 0x00: Magic "TEX0"
    // 0x04: Section size
    // 0x08: Padding
    // 0x0C: Texture data size (shifted by 3)
    // 0x10: Texture info offset (relative to TEX0 section start)
    // 0x14: Padding
    // 0x18: Texture data offset (relative to TEX0 section start)
    // 0x1C: Padding
    // 0x20: Compressed texture data size (shifted by 3)
    // 0x24: Compressed texture info offset
    // 0x28: Padding
    // 0x2C: Compressed texture data offset
    // 0x30: Padding
    // 0x34: Palette data size (shifted by 3)
    // 0x38: Palette info dict offset
    // 0x3C: Palette data offset

    uint32_t texInfoOffset = readU32LE(&tex0[0x10]);
    uint32_t texDataOffset = readU32LE(&tex0[0x18]);
    uint32_t palInfoOffset = readU32LE(&tex0[0x38]);
    uint32_t palDataOffset = readU32LE(&tex0[0x3C]);

    // Validate offsets
    if (texInfoOffset >= tex0Size || texDataOffset >= tex0Size ||
        palInfoOffset >= tex0Size || palDataOffset >= tex0Size) {
        return false;
    }

    // Parse texture dictionary to get texture info
    // Dictionary structure:
    // 0x00: Dummy
    // 0x01: Number of textures
    // 0x02-0x03: Size of dictionary (in 2-byte units)
    // Then entries follow

    const uint8_t* texInfo = &tex0[texInfoOffset];
    uint8_t numTextures = texInfo[1];
    if (numTextures < 1) {
        return false;
    }

    // Dictionary header is 8 bytes, then we have section data
    // After the header, each entry has:
    // - 4 bytes: offset/size data
    // - For texture: params at entry data offset

    // Skip dictionary header (8 bytes)
    // The actual texture parameters are at the data portion after the dictionary
    // Format: offset at [headerSize + entryIdx * 4], params are stored in entry data

    uint16_t dictSize = readU16LE(&texInfo[2]);

    // Find first texture's parameters
    // Dictionary layout: header (8 bytes) + entry data (4 bytes per entry) + section for each entry
    // The params are stored as: offset (2 bytes) + params (2 bytes) at entry data start + 0

    // Simplified parsing - look for texture params after dictionary header
    // Entry 0 data starts at offset 8 in the dictionary
    // Each entry is 4 bytes, then names follow

    if (texInfoOffset + 8 + 4 > tex0Size) {
        return false;
    }

    // Read texture params from first entry
    // In the dictionary, after the 8-byte header, each entry has 4 bytes of data
    // bytes 0-1: offset to texture data / 8
    // bytes 2-3: params containing format, width, height

    uint16_t texOffset = readU16LE(&texInfo[8]);
    uint16_t params = readU16LE(&texInfo[10]);

    // Extract dimensions from params
    // params format:
    // bits 0-3: repeat mode
    // bits 4-6: width exponent (actual = 8 << exp)
    // bits 7-9: height exponent
    // bits 10-12: format
    // bits 13-15: other flags

    int widthExp = (params >> 4) & 7;
    int heightExp = (params >> 7) & 7;
    int format = (params >> 10) & 7;

    int width = 8 << widthExp;
    int height = 8 << heightExp;

    // Format 4 = Palette256 (8 bits per pixel, 256 color palette)
    // Format 5 = 4x4 compressed
    // Format 3 = Palette16 (4 bits per pixel)
    // Format 2 = Palette4 (2 bits per pixel)

    if (format != 4) {
        // We only support Palette256 format for now
        // Try to handle it anyway for other formats
        if (format != 3 && format != 2) {
            return false;
        }
    }

    // Calculate texture data size
    int texDataSize = 0;
    if (format == 4) {
        texDataSize = width * height;  // 1 byte per pixel
    } else if (format == 3) {
        texDataSize = (width * height) / 2;  // 4 bits per pixel
    } else if (format == 2) {
        texDataSize = (width * height) / 4;  // 2 bits per pixel
    }

    // Extract texture data
    uint32_t actualTexOffset = texDataOffset + (texOffset * 8);
    if (actualTexOffset + texDataSize > tex0Size) {
        return false;
    }

    std::vector<uint8_t> texData(&tex0[actualTexOffset], &tex0[actualTexOffset + texDataSize]);

    // Parse palette dictionary to get palette data
    const uint8_t* palInfo = &tex0[palInfoOffset];
    uint8_t numPalettes = palInfo[1];
    if (numPalettes < 1) {
        return false;
    }

    // Read first palette offset
    uint16_t palOffset = readU16LE(&palInfo[8]);

    // Calculate palette size based on format
    int numColors = 0;
    if (format == 4) {
        numColors = 256;
    } else if (format == 3) {
        numColors = 16;
    } else if (format == 2) {
        numColors = 4;
    }

    int palDataSize = numColors * 2;  // RGB555 = 2 bytes per color

    uint32_t actualPalOffset = palDataOffset + (palOffset * 8);
    if (actualPalOffset + palDataSize > tex0Size) {
        return false;
    }

    std::vector<uint8_t> palData(&tex0[actualPalOffset], &tex0[actualPalOffset + palDataSize]);

    // Decode texture with palette
    m_texture = decodeTexture(texData, width, height, palData);

    return !m_texture.isNull();
}

QImage ClothLoader::decodeTexture(const std::vector<uint8_t>& texData, int width, int height,
                                   const std::vector<uint8_t>& palData) {
    QImage image(width, height, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    // Decode palette
    int numColors = static_cast<int>(palData.size() / 2);
    std::vector<Color> palette(numColors);
    for (int i = 0; i < numColors; i++) {
        uint16_t col = readU16LE(&palData[i * 2]);
        palette[i] = decodeRGB555(col);
        // First color is typically transparent
        if (i == 0) {
            palette[i].a = 0;
        }
    }

    // Decode texture (Palette256 format - 1 byte per pixel)
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int pixelIdx = y * width + x;
            if (pixelIdx >= static_cast<int>(texData.size())) {
                continue;
            }

            uint8_t colorIdx = texData[pixelIdx];
            if (colorIdx < palette.size()) {
                const Color& c = palette[colorIdx];
                image.setPixelColor(x, y, QColor(c.r, c.g, c.b, c.a));
            }
        }
    }

    return image;
}
