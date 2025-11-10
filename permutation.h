#pragma once
#include <string>

#define PERMUTATION_API

extern "C" {
    PERMUTATION_API std::string PermutationTextEncrypt(const std::string& text, const std::string& key);
    PERMUTATION_API std::string PermutationTextDecrypt(const std::string& text, const std::string& key);
    PERMUTATION_API void PermutationFileEncrypt(const std::string& inPath, const std::string& outPath, const std::string& key);
    PERMUTATION_API void PermutationFileDecrypt(const std::string& inPath, const std::string& outPath, const std::string& key);
    PERMUTATION_API std::string GeneratePermutationKey();
}
