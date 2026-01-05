#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <memory>

// NDS ROM Header (0x4000 bytes, but we only need first 0x180)
struct NDSHeader {
    char gameTitle[12];
    char gameCode[4];
    char makerCode[2];
    uint8_t unitCode;
    uint8_t encryptionSeed;
    uint8_t deviceCapacity;
    uint8_t reserved1[7];
    uint8_t reserved2;
    uint8_t ndsRegion;
    uint8_t romVersion;
    uint8_t autostart;

    uint32_t arm9RomOffset;
    uint32_t arm9EntryAddress;
    uint32_t arm9RamAddress;
    uint32_t arm9Size;

    uint32_t arm7RomOffset;
    uint32_t arm7EntryAddress;
    uint32_t arm7RamAddress;
    uint32_t arm7Size;

    uint32_t fntOffset;      // File Name Table offset
    uint32_t fntSize;        // File Name Table size
    uint32_t fatOffset;      // File Allocation Table offset
    uint32_t fatSize;        // File Allocation Table size

    uint32_t arm9OverlayOffset;
    uint32_t arm9OverlaySize;
    uint32_t arm7OverlayOffset;
    uint32_t arm7OverlaySize;

    uint32_t normalCardControl;
    uint32_t secureCardControl;

    uint32_t iconBannerOffset;

    uint16_t secureAreaCRC;
    uint16_t secureTransferTimeout;

    uint32_t arm9Autoload;
    uint32_t arm7Autoload;

    uint64_t secureDisable;

    uint32_t romSize;
    uint32_t headerSize;
};

// File Allocation Table entry
struct FATEntry {
    uint32_t startOffset;
    uint32_t endOffset;
};

// File/Directory entry in NitroFS
struct NitroFSEntry {
    std::string name;
    bool isDirectory;
    uint16_t fileId;        // For files
    uint16_t parentId;      // For directories
    uint16_t firstChildId;  // For directories
    uint32_t offset;        // File offset in ROM
    uint32_t size;          // File size
    std::vector<std::shared_ptr<NitroFSEntry>> children;
};

class NDSRom {
public:
    NDSRom();
    ~NDSRom();

    bool load(const std::string& filepath);
    void close();

    std::shared_ptr<NitroFSEntry> getRoot() const { return m_root; }
    std::shared_ptr<NitroFSEntry> findFile(const std::string& path) const;
    std::vector<uint8_t> readFile(const NitroFSEntry& entry) const;
    std::vector<uint8_t> readFile(const std::string& path) const;

    void printFileSystem(std::shared_ptr<NitroFSEntry> entry = nullptr, int depth = 0) const;
    std::vector<std::string> findFilesByExtension(const std::string& ext) const;

private:
    bool parseHeader();
    bool parseFAT();
    bool parseFNT();
    void parseFNTDirectory(uint32_t offset, uint16_t firstFileId,
                           std::shared_ptr<NitroFSEntry> parent);
    void findFilesByExtensionRecursive(std::shared_ptr<NitroFSEntry> entry,
                                       const std::string& ext,
                                       std::vector<std::string>& results,
                                       const std::string& currentPath) const;

    std::ifstream m_file;
    NDSHeader m_header;
    std::vector<FATEntry> m_fat;
    std::shared_ptr<NitroFSEntry> m_root;
};
