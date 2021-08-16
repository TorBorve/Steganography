#include <iostream>
#include <png.h>
#include <chrono>
#include "steg.h"
#include "pngUtilities.h"

int main(){
    // How to encode and decode text
    {
        std::cout << "Encoding text\n";
        std::string text = steg::getContents("textFiles/cppWiki.txt");
        std::string filename = "pictures/nature.png";
        steg::lsb::encode(filename, text);
        std::cout << "Decoding text\n";
        std::string decodedText = steg::lsb::decode(steg::encodedFilename(filename));
        steg::saveFile("textFiles/decoded.txt", decodedText);
    }
    // How to encode and decode pictures
    {
        std::cout << "Encoding picture\n";
        std::string data = steg::getContents("pictures/mountain.png");
        std::string filename = "pictures/nature.png";
        steg::lsb::encode(filename, data);
        std::cout << "Decoding picture\n";
        std::string decodedData = steg::lsb::decode(steg::encodedFilename(filename));
        steg::saveFile("pictures/decoded.png", decodedData);
    }
    std::cout << "Finished!\n";
    return 0; 
}