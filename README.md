# Seam carving

This is my C++ assignment. The only code I wrote is in a `src/` directory.

## Description

[**Seam carving**](https://en.wikipedia.org/wiki/Seam_carving) is an algorithm which erases a part of an image while keeping important details, 
allowing to reduce it's size without loosing valuable information.

Before:

![](/data/text/wavebefore.png)

After:

![](/data/text/waveafter.png)

## Interface

```cpp
class SeamCarver
{
    using Seam = std::vector<size_t>;

public:
    SeamCarver(Image image);

    /**
     * Returns current image
     */
    const Image& GetImage() const;

    /**
     * Gets current image width
     */
    size_t GetImageWidth() const;

    /**
     * Gets current image height
     */
    size_t GetImageHeight() const;

    /**
     * Returns pixel energy
     * @param columnId column index (x)
     * @param rowId row index (y)
     */
    double GetPixelEnergy(size_t columnId, size_t rowId) const;

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
     * Removes sequence of pixes from the image
     */
    void RemoveVerticalSeam(const Seam& seam);

private:
    Image m_image;
};
```

## Testing

You can use both unit tests and python script located in a `scripts/` directory.

Пример использования:
1. Generating a `csv` file from an image:
   ```
   python scripts/img_to_csv.py data/tower.jpeg data/tower.csv
   ```
2. Launching *seam-carving* algorithm:
   ```
   ./seam-carving data/tower.csv data/tower_updated.csv    
   ```
3. Generating an image from a `csv` file:
   ```
   python scripts/csv_to_img.py data/tower_updated.csv data/tower_updated.jpeg 
   ```

`tower_updated.jpeg` image is a result of a *seam carving* algorithm.

To run python scripts you need python3 and following packages:
* imageio
* numpy
* pandas
* argparse
