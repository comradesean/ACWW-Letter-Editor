#include "nds_rom.h"
#include <iostream>
#include <cstring>
#include <algorithm>

NDSRom::NDSRom() : m_root(nullptr) {
    memset(&m_header, 0, sizeof(m_header));
}

NDSRom::~NDSRom() {
    close();
}

bool NDSRom::load(const std::string& filepath) {
    m_file.open(filepath, std::ios::binary);
    if (!m_file.is_open()) {
        std::cerr << "Failed to open ROM file: " << filepath << std::endl;
        return false;
    }

    if (!parseHeader()) {
        std::cerr << "Failed to parse ROM header" << std::endl;
        return false;
    }

    if (!parseFAT()) {
        std::cerr << "Failed to parse FAT" << std::endl;
        return false;
    }

    if (!parseFNT()) {
        std::cerr << "Failed to parse FNT" << std::endl;
        return false;
    }

    return true;
}

void NDSRom::close() {
    if (m_file.is_open()) {
        m_file.close();
    }
    m_fat.clear();
    m_root = nullptr;
}

bool NDSRom::parseHeader() {
    m_file.seekg(0);
    m_file.read(reinterpret_cast<char*>(&m_header), sizeof(NDSHeader));

    if (!m_file) {
        return false;
    }

    return true;
}

bool NDSRom::parseFAT() {
    uint32_t numEntries = m_header.fatSize / 8;
    m_fat.resize(numEntries);

    m_file.seekg(m_header.fatOffset);
    m_file.read(reinterpret_cast<char*>(m_fat.data()), m_header.fatSize);

    if (!m_file) {
        return false;
    }

    return true;
}

bool NDSRom::parseFNT() {
    // Read FNT main table first entry to get info about root
    m_file.seekg(m_header.fntOffset);

    uint32_t rootSubtableOffset;
    uint16_t rootFirstFileId;
    uint16_t rootParentId;

    m_file.read(reinterpret_cast<char*>(&rootSubtableOffset), 4);
    m_file.read(reinterpret_cast<char*>(&rootFirstFileId), 2);
    m_file.read(reinterpret_cast<char*>(&rootParentId), 2);

    // Create root directory
    m_root = std::make_shared<NitroFSEntry>();
    m_root->name = "/";
    m_root->isDirectory = true;
    m_root->parentId = 0;
    m_root->firstChildId = rootFirstFileId;

    // Parse root directory contents
    parseFNTDirectory(m_header.fntOffset + rootSubtableOffset, rootFirstFileId, m_root);

    return true;
}

void NDSRom::parseFNTDirectory(uint32_t offset, uint16_t firstFileId,
                                std::shared_ptr<NitroFSEntry> parent) {
    m_file.seekg(offset);
    uint16_t currentFileId = firstFileId;

    while (true) {
        uint8_t typeLength;
        m_file.read(reinterpret_cast<char*>(&typeLength), 1);

        if (typeLength == 0) {
            // End of directory
            break;
        }

        bool isDirectory = (typeLength & 0x80) != 0;
        uint8_t nameLength = typeLength & 0x7F;

        // Read name
        std::string name(nameLength, '\0');
        m_file.read(&name[0], nameLength);

        auto entry = std::make_shared<NitroFSEntry>();
        entry->name = name;
        entry->isDirectory = isDirectory;

        if (isDirectory) {
            // Read directory ID
            uint16_t dirId;
            m_file.read(reinterpret_cast<char*>(&dirId), 2);

            // Read directory info from main FNT table
            uint32_t savedPos = m_file.tellg();

            m_file.seekg(m_header.fntOffset + (dirId & 0xFFF) * 8);
            uint32_t subtableOffset;
            uint16_t dirFirstFileId;
            uint16_t dirParentId;

            m_file.read(reinterpret_cast<char*>(&subtableOffset), 4);
            m_file.read(reinterpret_cast<char*>(&dirFirstFileId), 2);
            m_file.read(reinterpret_cast<char*>(&dirParentId), 2);

            entry->firstChildId = dirFirstFileId;
            entry->parentId = dirParentId;

            // Recursively parse subdirectory
            parseFNTDirectory(m_header.fntOffset + subtableOffset, dirFirstFileId, entry);

            // Restore position
            m_file.seekg(savedPos);
        } else {
            // File entry
            entry->fileId = currentFileId;
            if (currentFileId < m_fat.size()) {
                entry->offset = m_fat[currentFileId].startOffset;
                entry->size = m_fat[currentFileId].endOffset - m_fat[currentFileId].startOffset;
            }
            currentFileId++;
        }

        parent->children.push_back(entry);
    }
}

std::shared_ptr<NitroFSEntry> NDSRom::findFile(const std::string& path) const {
    if (!m_root || path.empty()) {
        return nullptr;
    }

    std::string searchPath = path;
    if (searchPath[0] == '/') {
        searchPath = searchPath.substr(1);
    }

    std::shared_ptr<NitroFSEntry> current = m_root;
    size_t pos = 0;
    std::string token;

    while ((pos = searchPath.find('/')) != std::string::npos || !searchPath.empty()) {
        if (pos == std::string::npos) {
            token = searchPath;
            searchPath.clear();
        } else {
            token = searchPath.substr(0, pos);
            searchPath = searchPath.substr(pos + 1);
        }

        if (token.empty()) continue;

        bool found = false;
        for (const auto& child : current->children) {
            if (child->name == token) {
                current = child;
                found = true;
                break;
            }
        }

        if (!found) {
            return nullptr;
        }
    }

    return current;
}

std::vector<uint8_t> NDSRom::readFile(const NitroFSEntry& entry) const {
    std::vector<uint8_t> data;

    if (entry.isDirectory || entry.size == 0) {
        return data;
    }

    data.resize(entry.size);

    std::ifstream& file = const_cast<std::ifstream&>(m_file);
    file.seekg(entry.offset);
    file.read(reinterpret_cast<char*>(data.data()), entry.size);

    return data;
}

std::vector<uint8_t> NDSRom::readFile(const std::string& path) const {
    auto entry = findFile(path);
    if (!entry) {
        return std::vector<uint8_t>();
    }
    return readFile(*entry);
}

void NDSRom::printFileSystem(std::shared_ptr<NitroFSEntry> entry, int depth) const {
    if (!entry) {
        entry = m_root;
    }
    if (!entry) return;

    std::string indent(depth * 2, ' ');

    if (entry->isDirectory) {
        std::cout << indent << "[" << entry->name << "]" << std::endl;
        for (const auto& child : entry->children) {
            printFileSystem(child, depth + 1);
        }
    } else {
        std::cout << indent << entry->name
                  << " (0x" << std::hex << entry->offset
                  << ", " << std::dec << entry->size << " bytes)" << std::endl;
    }
}

void NDSRom::findFilesByExtensionRecursive(std::shared_ptr<NitroFSEntry> entry,
                                            const std::string& ext,
                                            std::vector<std::string>& results,
                                            const std::string& currentPath) const {
    if (!entry) return;

    std::string path = currentPath;
    if (!entry->name.empty() && entry->name != "/") {
        path += "/" + entry->name;
    }

    if (entry->isDirectory) {
        for (const auto& child : entry->children) {
            findFilesByExtensionRecursive(child, ext, results, path);
        }
    } else {
        // Check extension
        size_t dotPos = entry->name.rfind('.');
        if (dotPos != std::string::npos) {
            std::string fileExt = entry->name.substr(dotPos);
            std::string extLower = ext;
            std::string fileExtLower = fileExt;
            std::transform(extLower.begin(), extLower.end(), extLower.begin(), ::tolower);
            std::transform(fileExtLower.begin(), fileExtLower.end(), fileExtLower.begin(), ::tolower);

            if (fileExtLower == extLower) {
                results.push_back(path);
            }
        }
    }
}

std::vector<std::string> NDSRom::findFilesByExtension(const std::string& ext) const {
    std::vector<std::string> results;
    findFilesByExtensionRecursive(m_root, ext, results, "");
    return results;
}
