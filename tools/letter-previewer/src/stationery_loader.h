#pragma once
#include <QImage>
#include <vector>
#include <string>

class NDSRom;

class StationeryLoader {
public:
    static constexpr int PAPER_WIDTH = 256;
    static constexpr int PAPER_HEIGHT = 192;
    static constexpr int NUM_PAPERS = 64;

    bool load(NDSRom& rom);

    const QImage& getPaper(int index) const;
    int count() const { return static_cast<int>(m_papers.size()); }

private:
    QImage loadPaper(NDSRom& rom, int paperNum);
    std::vector<QImage> m_papers;
};
