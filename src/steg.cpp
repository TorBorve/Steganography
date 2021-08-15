#include "steg.h"
#include "pngUtilities.h"
#include <png.h>
#include <assert.h>
#include <fstream>
#include <chrono>
#include <thread>

#define timeSince(prev) \
    std::cout << "Line: " << __LINE__ << ", Time: " << \
    (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - prevTime)).count() \
    << std::endl; \
    prev = std::chrono::system_clock::now();

namespace steg {
    std::string encodedFilename(const std::string& filename){
        return filename.substr(0, filename.size() - 4) + "_encoded.png";
    }

    std::string getContents(const std::string& filename){
        std::ifstream inFile{filename, std::ios::in | std::ios::binary};
        if (!inFile){
            throw std::runtime_error{"could not open file: " + filename};
        }
        std::string contents;
        inFile.seekg(0, std::ios::end);
        contents.resize(inFile.tellg());
        inFile.seekg(0, std::ios::beg);
        inFile.read(&contents[0], contents.size());
        return contents;

    }

    void saveText(const std::string& filename, const std::string& text){
        std::ofstream outFile{filename};
        if (!outFile){
            throw std::runtime_error{"could not create file: "  + filename};
        }
        outFile << text;
        return;
    }

    namespace lsb {
        bool bigEnough(uint32_t height, uint32_t byteWidth, const std::string& text){
            return height*byteWidth > text.size()*4;
        }

        void encodeChar(const char& ch, png_byte** rowPointers, uint32_t byteWidth, int& x, int& y){
            for (int i = 0; i < 4; i++){
                    const unsigned int shift = 6 - 2*i;
                    unsigned char keep = 0b00000011 << shift;
                    unsigned char data = (ch & keep) >> shift;
                    rowPointers[y][x] = (rowPointers[y][x] & 0b11111100) | data;
                    if (x < byteWidth - 1){
                        x++;
                    } else {
                        x = 0;
                        y++;
                    }
                }
        }

        void encode(png_byte** rowPointers, uint32_t height, uint32_t byteWidth, const std::string& text){
            if (!bigEnough(height, byteWidth, text)){
                throw std::runtime_error{"picture is not big enough to encode the entire text"};
            }
            int x = 0, y = 0;
            for (const char& ch : text){
                encodeChar(ch, rowPointers, byteWidth, x, y);
            }
            encodeChar('\0', rowPointers, byteWidth, x, y);
            return;
        }

        void encode(const std::string& filename, const std::string& text){
            FILE* fp; png_struct* pngPtr; png_info* infoPtr;
            png::open(filename, &fp, &pngPtr, &infoPtr);

            //get info
            uint32_t width, height;
            int bitDepth, colorType;
            png_get_IHDR(pngPtr, infoPtr, &width, &height, &bitDepth, &colorType, nullptr, nullptr, nullptr);
            assert(bitDepth == 8 && colorType == PNG_COLOR_TYPE_RGB);
            png_byte** rowPointers = png::makeRowPointers(height, png_get_rowbytes(pngPtr, infoPtr));
            png_read_image(pngPtr, rowPointers);

            // encode text into rowPointers
            encode(rowPointers, height, png_get_rowbytes(pngPtr, infoPtr), text);

            // close file
            png::close(&fp, &pngPtr, &infoPtr);
            // create new file for encoded picture
            png::create(encodedFilename(filename), width, height, bitDepth, colorType, &fp, &pngPtr, &infoPtr);
            png_write_image(pngPtr, rowPointers);
            png_write_end(pngPtr, nullptr);

            // clean up
            png::deleteRowPointers(height, rowPointers);
            png::close(&fp, &pngPtr, &infoPtr);

        }

        std::string decode(png_byte** rowPointers, uint32_t height, uint32_t byteWidth){
            std::string decodedText;
            char decodedChar;;
            int x = 0, y = 0;
            do {
                decodedChar = 0;
                for (int i = 0; i < 4; i++){
                    constexpr unsigned char keep = 0b00000011;
                    int shift = 6 - 2*i;
                    decodedChar = ((rowPointers[y][x] & keep) << shift) | decodedChar;
                    if (x < byteWidth - 1){
                        x++;
                    } else if (y == height - 1){
                        throw std::runtime_error{"could not find null terminating character in file"};
                    } else {
                        x = 0;
                        y++;
                    }
                } 
                decodedText += decodedChar;
            } while (decodedChar != '\0');
            decodedText.pop_back();
            return decodedText;
        }

        std::string decode(const std::string& filename){
            FILE* fp; png_struct* pngPtr; png_info* infoPtr;
            png::open(filename, &fp, &pngPtr, &infoPtr);
            uint32_t width, height;
            int bitDepth, colorType;
            png_get_IHDR(pngPtr, infoPtr, &width, &height, &bitDepth, &colorType, nullptr, nullptr, nullptr);
            assert(bitDepth == 8 && colorType == PNG_COLOR_TYPE_RGB);
            png_byte** rowPointers = png::makeRowPointers(height, png_get_rowbytes(pngPtr, infoPtr));
            png_read_image(pngPtr, rowPointers);
            std::string decodedText = decode(rowPointers, height, png_get_rowbytes(pngPtr, infoPtr));
            png::deleteRowPointers(height, rowPointers);
            png::close(&fp, &pngPtr, &infoPtr);
            return decodedText;
        }
    }

}