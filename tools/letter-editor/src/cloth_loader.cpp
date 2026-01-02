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

    // Load cloth texture from ROM
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

    // Check BTX0 magic
    if (data[0] != 'B' || data[1] != 'T' || data[2] != 'X' || data[3] != '0') {
        return false;
    }

    // Read section count and TEX0 offset
    uint16_t sectionCount = readU16LE(&data[0x0E]);
    if (sectionCount < 1) {
        return false;
    }

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

    // Read TEX0 header offsets
    uint16_t texInfoOffset = readU16LE(&tex0[0x0E]);
    uint32_t texDataOffset = readU32LE(&tex0[0x14]);
    uint32_t palInfoOffset = readU32LE(&tex0[0x20]);
    uint32_t palDataOffset = readU32LE(&tex0[0x24]);

    // Validate offsets (palInfoOffset can be 0 if no palette dictionary)
    if (texInfoOffset >= tex0Size || texDataOffset >= tex0Size || palDataOffset >= tex0Size) {
        return false;
    }
    if (palInfoOffset != 0 && palInfoOffset >= tex0Size) {
        return false;
    }

    // Parse texture dictionary
    const uint8_t* texInfo = &tex0[texInfoOffset];
    uint8_t numTextures = texInfo[1];
    if (numTextures < 1) {
        return false;
    }

    // Calculate offset to data entries in dictionary
    // Structure: info header (4) + dict header (4) + tree nodes ((n+2)*4)
    int treeSize = (numTextures + 2) * 4;
    int dataEntriesOffset = 4 + 4 + treeSize;

    if (texInfoOffset + dataEntriesOffset + 8 > tex0Size) {
        return false;
    }

    // Read texture params from first data entry
    uint16_t texOffset = readU16LE(&texInfo[dataEntriesOffset]);
    uint16_t params = readU16LE(&texInfo[dataEntriesOffset + 2]);

    // Decode params: format (bits 10-12), width (bits 4-6), height (bits 7-9)
    int format = (params >> 10) & 7;
    int width = 8 << ((params >> 4) & 7);
    int height = 8 << ((params >> 7) & 7);

    // Only support paletted formats
    if (format != 4 && format != 3 && format != 2) {
        return false;
    }

    // Calculate texture data size based on format
    int texDataSize = 0;
    if (format == 4) {
        texDataSize = width * height;       // Palette256: 8bpp
    } else if (format == 3) {
        texDataSize = (width * height) / 2; // Palette16: 4bpp
    } else if (format == 2) {
        texDataSize = (width * height) / 4; // Palette4: 2bpp
    }

    // Extract texture data
    uint32_t actualTexOffset = texDataOffset + (texOffset * 8);
    if (actualTexOffset + texDataSize > tex0Size) {
        return false;
    }
    std::vector<uint8_t> texData(&tex0[actualTexOffset], &tex0[actualTexOffset + texDataSize]);

    // Get palette offset
    uint32_t actualPalOffset;
    if (palInfoOffset == 0) {
        // No palette dictionary - use direct offset
        actualPalOffset = palDataOffset;
    } else {
        // Parse palette dictionary
        const uint8_t* palInfo = &tex0[palInfoOffset];
        uint8_t numPalettes = palInfo[1];
        if (numPalettes < 1) {
            return false;
        }

        int palTreeSize = (numPalettes + 2) * 4;
        int palDataEntriesOffset = 4 + 4 + palTreeSize;
        uint16_t palOffset = readU16LE(&palInfo[palDataEntriesOffset]);
        actualPalOffset = palDataOffset + (palOffset * 8);
    }

    // Calculate palette size
    int numColors = (format == 4) ? 256 : (format == 3) ? 16 : 4;
    int palDataSize = numColors * 2;

    if (actualPalOffset + palDataSize > tex0Size) {
        return false;
    }
    std::vector<uint8_t> palData(&tex0[actualPalOffset], &tex0[actualPalOffset + palDataSize]);

    // Decode texture
    m_texture = decodeTexture(texData, width, height, palData, format);
    return !m_texture.isNull();
}

QImage ClothLoader::decodeTexture(const std::vector<uint8_t>& texData, int width, int height,
                                   const std::vector<uint8_t>& palData, int format) {
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

    if (format == 3) {
        // Palette16 format - 4 bits per pixel (2 pixels per byte)
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int pixelIdx = y * width + x;
                int byteIdx = pixelIdx / 2;
                if (byteIdx >= static_cast<int>(texData.size())) {
                    continue;
                }

                uint8_t byte = texData[byteIdx];
                // Lower nibble is first pixel, upper nibble is second pixel
                uint8_t colorIdx = (pixelIdx % 2 == 0) ? (byte & 0x0F) : ((byte >> 4) & 0x0F);
                if (colorIdx < palette.size()) {
                    const Color& c = palette[colorIdx];
                    image.setPixelColor(x, y, QColor(c.r, c.g, c.b, c.a));
                }
            }
        }
    } else {
        // Palette256 format - 1 byte per pixel (and fallback for other formats)
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
    }

    return image;
}
