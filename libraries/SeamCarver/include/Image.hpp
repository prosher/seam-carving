#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <iostream>
#include <vector>

struct Image {
    struct Pixel {
        Pixel(int red, int green, int blue);

        int m_red;
        int m_green;
        int m_blue;
    };

    Image(std::vector<std::vector<Pixel>> table);

    Pixel GetPixel(size_t columnId, size_t rowId) const noexcept;

    std::vector<std::vector<Pixel>> m_table;
};

#endif  // IMAGE_HPP