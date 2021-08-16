#pragma once
#include <string>
namespace steg {
    std::string encodedFilename(const std::string& filename);

    std::string getContents(const std::string& filename);

    void saveFile(const std::string& filename, const std::string& data);

    namespace lsb {
        void encode(const std::string& filename, const std::string& text);

        std::string decode(const std::string& filename);

        constexpr auto endSig = "`89<2)7a¤";
    }
}