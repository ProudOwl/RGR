#pragma once
#include <string>

#ifdef _WIN32
    #ifdef PERMUTATION_EXPORTS
        #define PERMUTATION_API __declspec(dllexport)
    #else
        #define PERMUTATION_API __declspec(dllimport)
    #endif
#else
    #define PERMUTATION_API
#endif

extern "C" {
    PERMUTATION_API std::string PermutationTextEncrypt(const std::string& text, const std::string& key);
    PERMUTATION_API std::string PermutationTextDecrypt(const std::string& text, const std::string& key);
    PERMUTATION_API void PermutationFileEncrypt(const std::string& inPath, const std::string& outPath, const std::string& key);
    PERMUTATION_API void PermutationFileDecrypt(const std::string& inPath, const std::string& outPath, const std::string& key);
    PERMUTATION_API std::string GeneratePermutationKey();
}
