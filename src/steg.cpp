#include "steg.h"
#include "pngUtilities.h"
#include <png.h>
#include <assert.h>
#include <fstream>
#include <chrono>
#include <thread>
#include <string.h>

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

    void saveFile(const std::string& filename, const std::string& data){
        std::ofstream outFile{filename, std::ios::out | std::ios::binary};
        if (!outFile){
            throw std::runtime_error{"could not create file: " + filename};
        }
        outFile.write(&data[0], data.size());
    }

    namespace lsb {
        bool bigEnough(uint32_t height, uint32_t byteWidth, const std::string& text){
            return height*byteWidth > (text.size() + strlen(endSig))*4;
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
            for (int i = 0; i < strlen(endSig); i++){
                encodeChar(endSig[i], rowPointers, byteWidth, x, y);
            }        
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
            // png_set_compression_level(pngPtr, 0);
            // png_set_filter(pngPtr, 0, 0);
            png_write_image(pngPtr, rowPointers);
            png_write_end(pngPtr, nullptr);

            // clean up
            png::deleteRowPointers(height, rowPointers);
            png::close(&fp, &pngPtr, &infoPtr);

        }

        char decodeChar(png_byte** rowPointers, uint32_t height, uint32_t byteWidth, int& x, int& y){
            char decodedChar = 0;
            for (int i = 0; i < 4; i++){
                constexpr unsigned char keep = 0b00000011;
                int shift = 6 - 2*i;
                decodedChar = ((rowPointers[y][x] & keep) << shift) | decodedChar;
                if (x < byteWidth - 1){
                    x++;
                } else if (y == height - 1){
                    throw std::runtime_error{"could not find end signature in file"};
                } else {
                    x = 0;
                    y++;
                }
            }
            return decodedChar;
        }

        std::string decode(png_byte** rowPointers, uint32_t height, uint32_t byteWidth){
            std::string decodedText;
            std::string temp;
            int x = 0, y = 0;
            for (int indEndSig = 0; indEndSig < strlen(endSig);){
                char decodedChar = decodeChar(rowPointers, height, byteWidth, x, y);
                temp += decodedChar;
                if (endSig[indEndSig] == decodedChar){
                    indEndSig++;
                } else {
                    decodedText += temp;
                    temp.clear();
                    indEndSig = 0;
                }
            }
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