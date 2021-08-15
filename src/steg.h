#pragma once
#include <string>
namespace steg {
    std::string encodedFilename(const std::string& filename);

    std::string getContents(const std::string& filename);

    void saveText(const std::string& filename, const std::string& text);

    namespace lsb {
        void encode(const std::string& filename, const std::string& text);

        std::string decode(const std::string& filename);
    }
}