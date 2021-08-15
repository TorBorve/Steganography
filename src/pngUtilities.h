#pragma once
#include <png.h>
#include <iostream>

namespace png {

    void open(const std::string& filename, FILE** fp, png_struct** pngPtr, png_info** infoPtr);

    void create(const std::string& filename, size_t width, size_t height, 
                    int bitDepth, int colorType, FILE** fp, png_struct** pngPtr, png_info** infoPtr);

    png_byte** makeRowPointers(uint32_t height, uint32_t bytePrRow);

    void deleteRowPointers(uint32_t height, png_byte** rowPointers);

    void close(FILE** fp, png_struct** pngPtr, png_info** infoPtr);
}