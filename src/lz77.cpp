#include "lz77.h"
#include <iostream>
#include <cstring>

bool LZ77::isCompressed(const std::vector<uint8_t>& data) {
    if (data.size() < 4) return false;

    // Check for "LZ77" magic string
    if (memcmp(data.data(), "LZ77", 4) == 0) {
        return true;
    }

    // Check for standard NDS LZ77 header (type 0x10)
    if (data[0] == 0x10) {
        return true;
    }

    return false;
}

std::vector<uint8_t> LZ77::decompress(const std::vector<uint8_t>& input) {
    if (input.size() < 4) {
        return std::vector<uint8_t>();
    }

    size_t srcPos = 0;
    uint32_t decompressedSize = 0;

    // Check for "LZ77" magic string (used in ACWW)
    if (memcmp(input.data(), "LZ77", 4) == 0) {
        // Custom LZ77 header: "LZ77" + 1 byte type + 3 bytes size
        if (input.size() < 8) {
            return std::vector<uint8_t>();
        }

        uint8_t compressionType = input[4];
        if ((compressionType & 0xF0) != 0x10) {
            std::cerr << "Unknown LZ77 compression type: " << (int)compressionType << std::endl;
            return std::vector<uint8_t>();
        }

        decompressedSize = input[5] | (input[6] << 8) | (input[7] << 16);
        srcPos = 8;
    }
    // Standard NDS LZ77 header
    else if (input[0] == 0x10) {
        decompressedSize = input[1] | (input[2] << 8) | (input[3] << 16);
        srcPos = 4;
    }
    else {
        std::cerr << "Not LZ77 compressed data" << std::endl;
        return std::vector<uint8_t>();
    }

    if (decompressedSize == 0 || decompressedSize > 16 * 1024 * 1024) {
        std::cerr << "Invalid decompressed size: " << decompressedSize << std::endl;
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> output;
    output.reserve(decompressedSize);

    while (output.size() < decompressedSize && srcPos < input.size()) {
        uint8_t flags = input[srcPos++];

        for (int i = 0; i < 8 && output.size() < decompressedSize; i++) {
            if (srcPos >= input.size()) {
                break;
            }

            if (flags & 0x80) {
                // Compressed block (backreference)
                if (srcPos + 1 >= input.size()) {
                    break;
                }

                uint8_t byte1 = input[srcPos++];
                uint8_t byte2 = input[srcPos++];

                // Length: upper 4 bits of byte1 + 3
                uint32_t length = ((byte1 >> 4) & 0x0F) + 3;

                // Offset: lower 4 bits of byte1 << 8 | byte2, + 1
                uint32_t offset = (((byte1 & 0x0F) << 8) | byte2) + 1;

                // Copy from already-decompressed data
                if (offset > output.size()) {
                    std::cerr << "Invalid backreference offset" << std::endl;
                    return std::vector<uint8_t>();
                }

                size_t copyPos = output.size() - offset;
                for (uint32_t j = 0; j < length && output.size() < decompressedSize; j++) {
                    output.push_back(output[copyPos + j]);
                }
            }
            else {
                // Uncompressed byte
                output.push_back(input[srcPos++]);
            }

            flags <<= 1;
        }
    }

    if (output.size() != decompressedSize) {
        std::cerr << "Decompression size mismatch: got " << output.size()
                  << ", expected " << decompressedSize << std::endl;
    }

    return output;
}
