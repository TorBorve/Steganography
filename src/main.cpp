#include <iostream>
#include <png.h>
#include "steg.h"
#include "pngUtilities.h"

int main(){
    std::string text = steg::getContents("textFiles/cppWiki.txt");
    std::string filename = "pictures/nature.png";
    steg::lsb::encode(filename, text);
    std::string decodedText = steg::lsb::decode(steg::encodedFilename(filename));
    steg::saveText("textFiles/decoded.txt", decodedText);
    // std::cout << "Decoded text:\n" << decodedText << std::endl;
    return 0; 
}