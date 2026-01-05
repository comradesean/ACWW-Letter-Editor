#pragma once
#include <QString>
#include <QMap>
#include <QList>
#include <QPair>
#include <cstdint>

// ACWW Item Database
// Maps item hex codes (uint16_t) to item names (QString)
// Reference: docs/save-file-format.md - Item Hex Codes section

namespace ItemDatabase {

// Special item codes
constexpr uint16_t NO_ITEM = 0xFFF1;
constexpr uint16_t EMPTY_SLOT = 0x0000;

// Item category enumeration for grouping
enum class Category {
    Unknown,
    Stationery,
    Wallpaper,
    Carpet,
    Clothing,
    Insects,
    Fish,
    Trash,
    Songs,
    Tools,
    Umbrellas,
    Hats,
    HairAccessories,
    Flowers,
    Glasses,
    FacialAccessories,
    Bells,
    Furniture,
    Paintings,
    MuseumDisplays,
    VillagerPictures
};

// Get the name of an item by its hex code
// Returns empty string if the item is not found
QString getItemName(uint16_t hexCode);

// Get all items as a list of (hexCode, name) pairs for populating dropdowns
QList<QPair<uint16_t, QString>> getAllItems();

// Get items filtered by category
QList<QPair<uint16_t, QString>> getItemsByCategory(Category category);

// Get the category of an item by its hex code
Category getItemCategory(uint16_t hexCode);

// Get a human-readable name for a category
QString getCategoryName(Category category);

// Check if an item code represents "no item" (empty attachment)
inline bool isNoItem(uint16_t hexCode) {
    return hexCode == NO_ITEM || hexCode == EMPTY_SLOT;
}

// Item category ranges (for filtering/display)
namespace Categories {
    // Stationery: 0x1000 - 0x10FC (every 4th value)
    constexpr uint16_t STATIONERY_START = 0x1000;
    constexpr uint16_t STATIONERY_END = 0x10FC;

    // Wallpaper: 0x1100 - 0x113F
    constexpr uint16_t WALLPAPER_START = 0x1100;
    constexpr uint16_t WALLPAPER_END = 0x113F;

    // Carpet/Flooring: 0x1144 - 0x1183
    constexpr uint16_t CARPET_START = 0x1144;
    constexpr uint16_t CARPET_END = 0x1183;

    // Clothing: 0x11A8 - 0x12A7
    constexpr uint16_t CLOTHING_START = 0x11A8;
    constexpr uint16_t CLOTHING_END = 0x12A7;

    // Insects: 0x12B0 - 0x12E7
    constexpr uint16_t INSECTS_START = 0x12B0;
    constexpr uint16_t INSECTS_END = 0x12E7;

    // Fish: 0x12E8 - 0x131F
    constexpr uint16_t FISH_START = 0x12E8;
    constexpr uint16_t FISH_END = 0x131F;

    // Trash: 0x1320 - 0x1322
    constexpr uint16_t TRASH_START = 0x1320;
    constexpr uint16_t TRASH_END = 0x1322;

    // K.K. Songs: 0x1323 - 0x1368
    constexpr uint16_t SONGS_START = 0x1323;
    constexpr uint16_t SONGS_END = 0x1368;

    // Tools: 0x1369 - 0x137F
    constexpr uint16_t TOOLS_START = 0x1369;
    constexpr uint16_t TOOLS_END = 0x137F;

    // Umbrellas: 0x1380 - 0x139F
    constexpr uint16_t UMBRELLAS_START = 0x1380;
    constexpr uint16_t UMBRELLAS_END = 0x139F;

    // Hats: 0x13A8 - 0x13F7
    constexpr uint16_t HATS_START = 0x13A8;
    constexpr uint16_t HATS_END = 0x13F7;

    // Hair Accessories: 0x13F8 - 0x1407
    constexpr uint16_t HAIR_ACCESSORIES_START = 0x13F8;
    constexpr uint16_t HAIR_ACCESSORIES_END = 0x1407;

    // Flowers: 0x1408 - 0x148D (non-contiguous)
    constexpr uint16_t FLOWERS_START = 0x1408;
    constexpr uint16_t FLOWERS_END = 0x148D;

    // Glasses: 0x1431 - 0x144E
    constexpr uint16_t GLASSES_START = 0x1431;
    constexpr uint16_t GLASSES_END = 0x144E;

    // Facial Accessories: 0x144F - 0x1470
    constexpr uint16_t FACIAL_START = 0x144F;
    constexpr uint16_t FACIAL_END = 0x1470;

    // Bags of Bells: 0x1492 - 0x14FD
    constexpr uint16_t BELLS_START = 0x1492;
    constexpr uint16_t BELLS_END = 0x14FD;

    // Furniture: 0x3000 - 0x3880
    constexpr uint16_t FURNITURE_START = 0x3000;
    constexpr uint16_t FURNITURE_END = 0x3880;

    // Paintings: 0x3894 - 0x3930
    constexpr uint16_t PAINTINGS_START = 0x3894;
    constexpr uint16_t PAINTINGS_END = 0x3930;

    // Museum Displays: 0x42A4 - 0x472C
    constexpr uint16_t MUSEUM_START = 0x42A4;
    constexpr uint16_t MUSEUM_END = 0x472C;

    // Villager Pictures: 0x47D8 - 0x4A44
    constexpr uint16_t PICTURES_START = 0x47D8;
    constexpr uint16_t PICTURES_END = 0x4A44;
}

// Internal implementation - builds the item database on first access
namespace Detail {
    const QMap<uint16_t, QString>& getDatabase();
}

} // namespace ItemDatabase
