#pragma once
#include <cstdint>

/**
 * Letter icon definitions for inventory display
 *
 * Icons are from /menu/inventory/obj0+obj1.bch with palette from b_obj_itm.bpl
 * Each icon is 16x16 pixels, palette selects the color scheme for the state.
 */
namespace LetterIcons {

// Letter types
enum class LetterType {
    Regular,        // Standard letter
    Bottled,        // Message in a bottle
    SpecialDelivery // Favor/special delivery letter
};

// Letter states
enum class LetterState {
    Writing,        // Being written, not yet mailed
    Unopened,       // Received, not opened
    Opened          // Received and opened
};

// Icon definition structure
struct IconDef {
    int index;      // Icon index (0-79)
    int palette;    // Palette index (0-2)

    constexpr IconDef(int idx, int pal) : index(idx), palette(pal) {}
};

// =============================================================================
// Regular Letters (Palette 0 = Writing, Palette 1 = Received)
// =============================================================================

// Writing state (Palette 0)
constexpr IconDef LETTER_WRITING            {32, 0};  // Letter being written
constexpr IconDef LETTER_WRITING_GIFT       {33, 0};  // Letter being written with gift

// Received unopened (Palette 1)
constexpr IconDef LETTER_UNOPENED           {32, 1};  // Received letter, unopened
constexpr IconDef LETTER_UNOPENED_GIFT      {33, 1};  // Received letter, unopened with gift

// Received opened (Palette 1)
constexpr IconDef LETTER_OPENED             {48, 1};  // Received letter, opened
constexpr IconDef LETTER_OPENED_GIFT        {49, 1};  // Received letter, opened with gift

// =============================================================================
// Bottled Mail (Palette 0 = Writing, Palette 1 = Received)
// =============================================================================

// Writing state (Palette 0)
constexpr IconDef BOTTLE_WRITING            {73, 0};  // Bottled mail being written
constexpr IconDef BOTTLE_WRITING_GIFT       {75, 0};  // Bottled mail being written with gift

// Received unopened (Palette 1)
constexpr IconDef BOTTLE_UNOPENED           {73, 1};  // Received bottled mail, unopened
constexpr IconDef BOTTLE_UNOPENED_GIFT      {75, 1};  // Received bottled mail, unopened with gift

// Received opened (Palette 1)
constexpr IconDef BOTTLE_OPENED             {74, 1};  // Received bottled mail, opened
constexpr IconDef BOTTLE_OPENED_GIFT        {76, 1};  // Received bottled mail, opened with gift

// =============================================================================
// Special Delivery / Favor Letters (Palette 2)
// =============================================================================

// Unopened (Palette 2)
constexpr IconDef SPECIAL_UNOPENED          {32, 2};  // Special delivery, unopened
constexpr IconDef SPECIAL_UNOPENED_GIFT     {33, 2};  // Special delivery, unopened with gift

// Opened (Palette 2)
constexpr IconDef SPECIAL_OPENED            {48, 2};  // Special delivery, opened
constexpr IconDef SPECIAL_OPENED_GIFT       {49, 2};  // Special delivery, opened with gift

// =============================================================================
// Icon flag values (from letter offset 0xEE)
// =============================================================================
namespace IconFlag {
    constexpr uint8_t LETTER_WRITING        = 0x01;  // Letter being written
    constexpr uint8_t LETTER_UNOPENED       = 0x02;  // Received letter, unopened
    constexpr uint8_t LETTER_OPENED         = 0x03;  // Received letter, opened
    constexpr uint8_t BOTTLE_WRITING        = 0x04;  // Bottled mail being written
    constexpr uint8_t BOTTLE_UNOPENED       = 0x05;  // Received bottled mail, unopened
    constexpr uint8_t BOTTLE_OPENED         = 0x06;  // Received bottled mail, opened
    constexpr uint8_t SPECIAL_UNOPENED      = 0x07;  // Special delivery, unopened
    constexpr uint8_t SPECIAL_OPENED        = 0x08;  // Special delivery, opened
}

// =============================================================================
// Lookup functions
// =============================================================================

/**
 * Get the appropriate icon based on the iconFlags byte (0xEE) and gift status
 *
 * @param iconFlags Value from letter offset 0xEE (0x01-0x08 or 0x41-0x48)
 * @param hasGift   Whether the letter has an attached gift (0xF0 != 0xFFF1)
 * @return          IconDef with index and palette, or {-1, -1} if invalid
 *
 * Note: The upper nibble (0x40) is masked off - both 0x01 and 0x41 map to the same icon
 */
constexpr IconDef getIconFromFlags(uint8_t iconFlags, bool hasGift) {
    // Mask to lower nibble - 0x41-0x48 should display same as 0x01-0x08
    uint8_t iconType = iconFlags & 0x0F;
    switch (iconType) {
        case IconFlag::LETTER_WRITING:
            return hasGift ? LETTER_WRITING_GIFT : LETTER_WRITING;
        case IconFlag::LETTER_UNOPENED:
            return hasGift ? LETTER_UNOPENED_GIFT : LETTER_UNOPENED;
        case IconFlag::LETTER_OPENED:
            return hasGift ? LETTER_OPENED_GIFT : LETTER_OPENED;
        case IconFlag::BOTTLE_WRITING:
            return hasGift ? BOTTLE_WRITING_GIFT : BOTTLE_WRITING;
        case IconFlag::BOTTLE_UNOPENED:
            return hasGift ? BOTTLE_UNOPENED_GIFT : BOTTLE_UNOPENED;
        case IconFlag::BOTTLE_OPENED:
            return hasGift ? BOTTLE_OPENED_GIFT : BOTTLE_OPENED;
        case IconFlag::SPECIAL_UNOPENED:
            return hasGift ? SPECIAL_UNOPENED_GIFT : SPECIAL_UNOPENED;
        case IconFlag::SPECIAL_OPENED:
            return hasGift ? SPECIAL_OPENED_GIFT : SPECIAL_OPENED;
        default:
            return {-1, -1};  // Unknown flag value
    }
}

/**
 * Get the appropriate icon for a letter based on type, state, and gift status
 *
 * @param type      Letter type (Regular, Bottled, SpecialDelivery)
 * @param state     Letter state (Writing, Unopened, Opened)
 * @param hasGift   Whether the letter has an attached gift
 * @return          IconDef with index and palette, or {-1, -1} if invalid
 */
constexpr IconDef getIcon(LetterType type, LetterState state, bool hasGift) {
    switch (type) {
        case LetterType::Regular:
            switch (state) {
                case LetterState::Writing:
                    return hasGift ? LETTER_WRITING_GIFT : LETTER_WRITING;
                case LetterState::Unopened:
                    return hasGift ? LETTER_UNOPENED_GIFT : LETTER_UNOPENED;
                case LetterState::Opened:
                    return hasGift ? LETTER_OPENED_GIFT : LETTER_OPENED;
            }
            break;

        case LetterType::Bottled:
            switch (state) {
                case LetterState::Writing:
                    return hasGift ? BOTTLE_WRITING_GIFT : BOTTLE_WRITING;
                case LetterState::Unopened:
                    return hasGift ? BOTTLE_UNOPENED_GIFT : BOTTLE_UNOPENED;
                case LetterState::Opened:
                    return hasGift ? BOTTLE_OPENED_GIFT : BOTTLE_OPENED;
            }
            break;

        case LetterType::SpecialDelivery:
            // Special delivery has no "writing" state - it's always received
            switch (state) {
                case LetterState::Writing:
                    // Fall through to unopened for special delivery
                case LetterState::Unopened:
                    return hasGift ? SPECIAL_UNOPENED_GIFT : SPECIAL_UNOPENED;
                case LetterState::Opened:
                    return hasGift ? SPECIAL_OPENED_GIFT : SPECIAL_OPENED;
            }
            break;
    }

    return {-1, -1};  // Invalid combination
}

} // namespace LetterIcons
