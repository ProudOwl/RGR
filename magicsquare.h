#pragma once
#include <string>

#define MAGICSQUARE_API

extern "C" {
    MAGICSQUARE_API std::string MagicSquareTextEncrypt(const std::string& text, const std::string& key);
    MAGICSQUARE_API std::string MagicSquareTextDecrypt(const std::string& text, const std::string& key);
    MAGICSQUARE_API void MagicSquareFileEncrypt(const std::string& inPath, const std::string& outPath, const std::string& key);
    MAGICSQUARE_API void MagicSquareFileDecrypt(const std::string& inPath, const std::string& outPath, const std::string& key);
    MAGICSQUARE_API std::string GenerateMagicSquareKey();
}
