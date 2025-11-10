#pragma once
#include <string>

#define MATRIX_API

extern "C" {
    MATRIX_API std::string MatrixTextEncrypt(const std::string& text, const std::string& key);
    MATRIX_API std::string MatrixTextDecrypt(const std::string& text, const std::string& key);
    MATRIX_API void MatrixFileEncrypt(const std::string& inPath, const std::string& outPath, const std::string& key);
    MATRIX_API void MatrixFileDecrypt(const std::string& inPath, const std::string& outPath, const std::string& key);
    MATRIX_API std::string GenerateMatrixKey();
}
