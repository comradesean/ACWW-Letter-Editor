#pragma once

// Shared constants for letter layout and limits
namespace LetterConstants {
    // Layout constants (in 1x coordinates)
    constexpr int HEADER_LEFT = 48;
    constexpr int HEADER_TOP = 40;
    constexpr int BODY_LEFT = 48;
    constexpr int BODY_TOP = 64;
    constexpr int BODY_LINES = 4;
    constexpr int FOOTER_RIGHT = 208;
    constexpr int FOOTER_TOP = 136;
    constexpr int LINE_HEIGHT = 16;
    constexpr int GLYPH_SPACING = 1;
    constexpr int MAX_LINE_WIDTH = 150;
    constexpr int MAX_FOOTER_WIDTH = 160;
    constexpr int NAME_TOKEN_WIDTH = 55;

    // Character limits
    // File format: subject (16) + body (100) + signature (26)
    // Display: header can be template (16) + inserted name (8) = 24 max
    constexpr int MAX_HEADER_CHARS = 24;   // Template (16) + Name (8)
    constexpr int MAX_BODY_CHARS = 128;
    constexpr int MAX_FOOTER_CHARS = 32;
}
