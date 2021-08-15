#include "pngUtilities.h"

namespace png {

    void open(const std::string& filename, FILE** fp, png_struct** pngPtr, png_info** infoPtr){
        *fp = fopen(filename.c_str(), "rb");
        if (!*fp){
            throw std::runtime_error{"could not open file"};
        }
        // check if it is a png file
        unsigned char sig[8];
        fread(sig, 1, 8, *fp);
        if(!png_check_sig(sig, 8)){
            throw std::runtime_error{filename + " is not a png file"};
        }
        *pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (!*pngPtr){
            throw std::runtime_error{"could not create write struct"};
        }
        *infoPtr = png_create_info_struct(*pngPtr);
        if (!infoPtr){
            throw std::runtime_error{"could not create info struct"};
        }
        if (setjmp(png_jmpbuf(*pngPtr))){
            throw std::runtime_error{"error from setjmp"};
        }
        png_init_io(*pngPtr, *fp);
        png_set_sig_bytes(*pngPtr, 8);
        png_read_info(*pngPtr, *infoPtr);
        if (setjmp(png_jmpbuf(*pngPtr))){
            throw std::runtime_error{"error from setjmp"};
        }
    }

    void create(const std::string& filename, size_t width, size_t height, 
                    int bitDepth, int colorType, FILE** fp, png_struct** pngPtr, png_info** infoPtr){
        *fp = fopen(filename.c_str(), "wb");
        if (!*fp){
            throw std::runtime_error{"could not open file: " + filename};
        }
        *pngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (!*pngPtr){
            throw std::runtime_error{"could not create write struct"};
        }
        *infoPtr = png_create_info_struct(*pngPtr);
        if (!*infoPtr){
            throw std::runtime_error{"could not create info struct"};
        }
        if (setjmp(png_jmpbuf(*pngPtr))){
            throw std::runtime_error{"jumpbuf error createPng"};
        }
        png_init_io(*pngPtr, *fp);
        if (setjmp(png_jmpbuf(*pngPtr))){
            throw std::runtime_error{"jumpbuf error createPng"};
        }
        png_set_IHDR(*pngPtr, *infoPtr, width, height, bitDepth, colorType, PNG_INTERLACE_NONE, 
                    PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
        png_write_info(*pngPtr, *infoPtr);
        if (setjmp(png_jmpbuf(*pngPtr))){
            throw std::runtime_error{"jumpbuf error createPng"};
        }
    }

    png_byte** makeRowPointers(uint32_t height, uint32_t bytePrRow){
        png_byte** rowPointers = new png_byte*[height];
        for (int i = 0; i < height; i++){
            rowPointers[i] = (png_byte*) malloc(bytePrRow);
        }
        return rowPointers;
    }

    void deleteRowPointers(uint32_t height, png_byte** rowPointers){
        for (int i = 0; i < height; i++){
            delete[] rowPointers[i];
        }
        delete[] rowPointers;
        rowPointers = nullptr;
    }

    void close(FILE** fp, png_struct** pngPtr, png_info** infoPtr){
        if (!*fp || !*pngPtr || !*infoPtr){
            throw std::runtime_error{"can not close file"};
        }
        png_destroy_read_struct(pngPtr, infoPtr, nullptr);
        png_destroy_info_struct(*pngPtr, infoPtr);
        fclose(*fp);
        *fp = nullptr;
        *pngPtr = nullptr;
        *infoPtr = nullptr;
    }
}