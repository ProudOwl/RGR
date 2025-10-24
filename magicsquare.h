#pragma once
#include <string>

#ifdef _WIN32
    #ifdef MAGICSQUARE_EXPORTS
        #define MAGICSQUARE_API __declspec(dllexport)
    #else
        #define MAGICSQUARE_API __declspec(dllimport)
    #endif
#else
    #define MAGICSQUARE_API
#endif

extern "C" {
    MAGICSQUARE_API std::string MagicSquareTextEncrypt(const std::string& text, const std::string& key);
    MAGICSQUARE_API std::string MagicSquareTextDecrypt(const std::string& text, const std::string& key);
    MAGICSQUARE_API void MagicSquareFileEncrypt(const std::string& inPath, const std::string& outPath, const std::string& key);
    MAGICSQUARE_API void MagicSquareFileDecrypt(const std::string& inPath, const std::string& outPath, const std::string& key);
    MAGICSQUARE_API std::string GenerateMagicSquareKey();
}
