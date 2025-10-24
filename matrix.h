#pragma once
#include <string>

#ifdef _WIN32
    #ifdef MATRIX_EXPORTS
        #define MATRIX_API __declspec(dllexport)
    #else
        #define MATRIX_API __declspec(dllimport)
    #endif
#else
    #define MATRIX_API
#endif

extern "C" {
    MATRIX_API std::string MatrixTextEncrypt(const std::string& text, const std::string& key);
    MATRIX_API std::string MatrixTextDecrypt(const std::string& text, const std::string& key);
    MATRIX_API void MatrixFileEncrypt(const std::string& inPath, const std::string& outPath, const std::string& key);
    MATRIX_API void MatrixFileDecrypt(const std::string& inPath, const std::string& outPath, const std::string& key);
    MATRIX_API std::string GenerateMatrixKey();
}
