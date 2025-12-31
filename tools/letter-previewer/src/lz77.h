#pragma once
#include <cstdint>
#include <vector>

// Nintendo DS LZ77 decompression
// Handles LZ77 (type 0x10) compression used in NDS games

class LZ77 {
public:
    // Decompress LZ77 compressed data
    // Returns empty vector on failure
    static std::vector<uint8_t> decompress(const std::vector<uint8_t>& input);

    // Check if data appears to be LZ77 compressed
    static bool isCompressed(const std::vector<uint8_t>& data);
};
