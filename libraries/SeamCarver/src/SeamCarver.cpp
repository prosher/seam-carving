#include "SeamCarver.hpp"

#include <algorithm>
#include <cmath>

SeamCarver::SeamCarver(Image image)
    : m_image(std::move(image))
    , m_energyTable(Table(GetImageWidth(), Line(GetImageHeight())))
    , m_vEnergyPathTable(Table(GetImageWidth(), Line(GetImageHeight())))
    , m_hEnergyPathTable(Table(GetImageWidth(), Line(GetImageHeight()))) {
    RecalcTableEnergy();
}

const Image &SeamCarver::GetImage() const noexcept {
    return m_image;
}

size_t SeamCarver::GetImageWidth() const noexcept {
    return m_image.m_table.size();
}

size_t SeamCarver::GetImageHeight() const noexcept {
    return m_image.m_table[0].size();
}

double SeamCarver::GetPixelEnergy(const size_t columnId, const size_t rowId) const noexcept {
    return m_energyTable[columnId][rowId];
}

static double BoundaryDiff(const size_t value, const int diff, const size_t upperBound) noexcept {
    return value > 0 ? std::min(value + diff, upperBound - 1) : value == 0 && diff > 0 ? value + diff : 0;
}

SeamCarver::Seam SeamCarver::FindHorizontalSeam() const {
    if (!m_hEnergyPathTableIsValid) {
        RecalcHorizontalEnergyPathTable();
    }
    Seam seam(GetImageWidth());
    seam.back() = std::min_element(m_hEnergyPathTable.back().begin(), m_hEnergyPathTable.back().end()) -
                  m_hEnergyPathTable.back().begin();
    for (size_t columnId = GetImageWidth() - 1; columnId > 0; --columnId) {
        double nextMinEnergy = std::numeric_limits<double>::infinity();
        size_t nextMinEnergyIndex;
        for (const int &rowDiff : {0, -1, 1}) {
            const size_t nextRow = BoundaryDiff(seam[columnId], rowDiff, GetImageHeight());
            if (m_hEnergyPathTable[columnId - 1][nextRow] < nextMinEnergy) {
                nextMinEnergy      = m_hEnergyPathTable[columnId - 1][nextRow];
                nextMinEnergyIndex = nextRow;
            }
        }
        seam[columnId - 1] = nextMinEnergyIndex;
    }
    return seam;
}

SeamCarver::Seam SeamCarver::FindVerticalSeam() const {
    if (!m_vEnergyPathTableIsValid) {
        RecalcVerticalEnergyPathTable();
    }
    Seam seam(GetImageHeight());
    seam.back() = std::min_element(m_vEnergyPathTable.begin(), m_vEnergyPathTable.end(),
                                   [](const auto &vec1, const auto &vec2) { return vec1.back() < vec2.back(); }) -
                  m_vEnergyPathTable.begin();
    for (size_t rowId = GetImageHeight() - 1; rowId > 0; --rowId) {
        double nextMinEnergy = std::numeric_limits<double>::infinity();
        size_t nextMinEnergyIndex;
        for (const int &rowDiff : {0, -1, 1}) {
            const size_t nextColumn = BoundaryDiff(seam[rowId], rowDiff, GetImageWidth());
            if (m_vEnergyPathTable[nextColumn][rowId - 1] < nextMinEnergy) {
                nextMinEnergy      = m_vEnergyPathTable[nextColumn][rowId - 1];
                nextMinEnergyIndex = nextColumn;
            }
        }
        seam[rowId - 1] = nextMinEnergyIndex;
    }
    return seam;
}

void SeamCarver::RemoveHorizontalSeam(const Seam &seam) {
    for (size_t columnId = 0; columnId < GetImageWidth(); ++columnId) {
        const size_t rowId = seam[columnId];
        auto &column       = m_image.m_table[columnId];
        column.erase(column.begin() + rowId);
        RecalcPixelEnergy(columnId, rowId % column.size());
        RecalcPixelEnergy(columnId, (rowId + column.size() - 1) % column.size());
    }
    m_hEnergyPathTableIsValid = false;
    m_vEnergyPathTableIsValid = false;
}

void SeamCarver::RemoveVerticalSeam(const Seam &seam) {
    for (size_t rowId = 0; rowId < GetImageHeight(); ++rowId) {
        const size_t newWidth = GetImageWidth() - 1;
        for (size_t columnId = seam[rowId]; columnId < newWidth; ++columnId) {
            m_image.m_table[columnId][rowId] = m_image.m_table[columnId + 1][rowId];
        }
        RecalcPixelEnergy(seam[rowId] % newWidth, rowId);
        RecalcPixelEnergy((seam[rowId] + newWidth - 1) % newWidth, rowId);
    }
    m_image.m_table.pop_back();
    m_hEnergyPathTableIsValid = false;
    m_vEnergyPathTableIsValid = false;
}

void SeamCarver::RecalcPixelEnergy(const size_t columnId, const size_t rowId) const noexcept {
    Image::Pixel rowNext = m_image.m_table[columnId][(rowId + 1) % GetImageHeight()];
    Image::Pixel rowPrev = m_image.m_table[columnId][(rowId + GetImageHeight() - 1) % GetImageHeight()];
    Image::Pixel colNext = m_image.m_table[(columnId + 1) % GetImageWidth()][rowId];
    Image::Pixel colPrev = m_image.m_table[(columnId + GetImageWidth() - 1) % GetImageWidth()][rowId];
    double accum         = 0;
    for (const auto &[next, prev] : {std::make_pair(rowNext, rowPrev), std::make_pair(colNext, colPrev)}) {
        accum += (next.m_red - prev.m_red) * (next.m_red - prev.m_red) +
                 (next.m_green - prev.m_green) * (next.m_green - prev.m_green) +
                 (next.m_blue - prev.m_blue) * (next.m_blue - prev.m_blue);
    }
    m_energyTable[columnId][rowId] = std::sqrt(accum);
}

void SeamCarver::RecalcTableEnergy() const noexcept {
    for (size_t columnId = 0; columnId < GetImageWidth(); ++columnId) {
        for (size_t rowId = 0; rowId < GetImageHeight(); ++rowId) {
            RecalcPixelEnergy(columnId, rowId);
        }
    }
}

void SeamCarver::RecalcHorizontalEnergyPathTable() const noexcept {
    m_hEnergyPathTable[0] = m_energyTable[0];
    for (size_t columnId = 1; columnId < GetImageWidth(); ++columnId) {
        for (size_t rowId = 0; rowId < GetImageHeight(); ++rowId) {
            m_hEnergyPathTable[columnId][rowId] =
                m_energyTable[columnId][rowId] +
                std::min(std::min(m_hEnergyPathTable[columnId - 1][BoundaryDiff(rowId, 1, GetImageHeight())],
                                  m_hEnergyPathTable[columnId - 1][BoundaryDiff(rowId, -1, GetImageHeight())]),
                         m_hEnergyPathTable[columnId - 1][rowId]);
        }
    }
    m_hEnergyPathTableIsValid = true;
}

void SeamCarver::RecalcVerticalEnergyPathTable() const noexcept {
    for (size_t columnId = 0; columnId < GetImageWidth(); ++columnId) {
        m_vEnergyPathTable[columnId][0] = m_energyTable[columnId][0];
    }
    for (size_t rowId = 1; rowId < GetImageHeight(); ++rowId) {
        for (size_t columnId = 0; columnId < GetImageWidth(); ++columnId) {
            m_vEnergyPathTable[columnId][rowId] =
                m_energyTable[columnId][rowId] +
                std::min(std::min(m_vEnergyPathTable[BoundaryDiff(columnId, 1, GetImageWidth())][rowId - 1],
                                  m_vEnergyPathTable[BoundaryDiff(columnId, -1, GetImageWidth())][rowId - 1]),
                         m_vEnergyPathTable[columnId][rowId - 1]);
        }
    }
    m_vEnergyPathTableIsValid = true;
}
