#ifndef SEAMCARVER_HPP
#define SEAMCARVER_HPP

#include "Image.hpp"

class SeamCarver {
    using Seam  = std::vector<size_t>;
    using Line  = std::vector<double>;
    using Table = std::vector<Line>;

public:
    SeamCarver(Image image);

    /**
     * Returns current image
     */
    const Image& GetImage() const noexcept;

    /**
     * Gets current image width
     */
    size_t GetImageWidth() const noexcept;

    /**
     * Gets current image height
     */
    size_t GetImageHeight() const noexcept;

    /**
     * Returns pixel energy
     * @param columnId column index (x)
     * @param rowId row index (y)
     */
    double GetPixelEnergy(size_t columnId, size_t rowId) const noexcept;

    /**
     * Returns sequence of pixel row indexes (y)
     * (x indexes are [0:W-1])
     */
    Seam FindHorizontalSeam() const;

    /**
     * Returns sequence of pixel column indexes (x)
     * (y indexes are [0:H-1])
     */
    Seam FindVerticalSeam() const;

    /**
     * Removes sequence of pixels from the image
     */
    void RemoveHorizontalSeam(const Seam& seam);

    /**
     * Removes sequence of pixels from the image
     */
    void RemoveVerticalSeam(const Seam& seam);

private:
    Image m_image;
    mutable Table m_energyTable;
    mutable Table m_vEnergyPathTable;
    mutable Table m_hEnergyPathTable;

    mutable bool m_vEnergyPathTableIsValid{};
    mutable bool m_hEnergyPathTableIsValid{};

    void RecalcHorizontalEnergyPathTable() const noexcept;
    void RecalcVerticalEnergyPathTable() const noexcept;

    void RecalcPixelEnergy(size_t columnId, size_t rowId) const noexcept;
    void RecalcTableEnergy() const noexcept;
};

#endif  // SEAMCARVER_HPP
