#pragma once
#include <QImage>
#include <cstdint>
#include <vector>
#include <string>

class NDSRom;

class ClothLoader {
public:
    static constexpr int CLOTH_WIDTH = 32;
    static constexpr int CLOTH_HEIGHT = 32;

    bool load(NDSRom& rom);

    const QImage& getTexture() const { return m_texture; }
    bool isLoaded() const { return m_loaded; }

private:
    bool parseNSBTX(const std::vector<uint8_t>& data);
    QImage decodeTexture(const std::vector<uint8_t>& texData, int width, int height,
                         const std::vector<uint8_t>& palData);

    QImage m_texture;
    bool m_loaded = false;
};
