#include "permutation.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <ctime>
#include <stdexcept>

using namespace std;

vector<string> SplitUTF8(const string& str) {
    vector<string> characters;
    
    for (size_t i = 0; i < str.length();) {
        unsigned char c = str[i];
        size_t charLen = 1;
        
        if ((c & 0x80) == 0) {
            charLen = 1;
        } else if ((c & 0xE0) == 0xC0) {
            charLen = 2;
        } else if ((c & 0xF0) == 0xE0) {
            charLen = 3;
        } else if ((c & 0xF8) == 0xF0) {
            charLen = 4;
        }
        
        characters.push_back(str.substr(i, charLen));
        i += charLen;
    }
    
    return characters;
}

vector<size_t> ParseKey(const string& key) {
    vector<size_t> permutation;
    stringstream ss(key);
    string token;
    
    while (getline(ss, token, '-')) {
        try {
            int num = stoi(token);
            if (num <= 0) {
                throw invalid_argument("Ключ должен содержать только положительные числа");
            }
            permutation.push_back(static_cast<size_t>(num - 1));
        } catch (const exception& e) {
            throw invalid_argument("Неверный формат ключа. Используйте формат: 3-1-4-2");
        }
    }
    
    vector<size_t> sorted = permutation;
    sort(sorted.begin(), sorted.end());
    for (size_t i = 0; i < sorted.size(); ++i) {
        if (sorted[i] != i) {
            throw invalid_argument("Ключ должен быть перестановкой чисел от 1 до N");
        }
    }
    
    return permutation;
}

string GeneratePermutationKey() {
    srand(time(0));
    int blockSize = 3 + rand() % 6;
    
    vector<int> numbers(blockSize);
    for (size_t i = 0; i < blockSize; ++i) {
        numbers[i] = i + 1;
    }
    
    for (size_t i = blockSize - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        swap(numbers[i], numbers[j]);
    }
    
    stringstream keyStream;
    for (size_t i = 0; i < blockSize; ++i) {
        if (i > 0) keyStream << "-";
        keyStream << numbers[i];
    }
    
    return keyStream.str();
}

vector<uint8_t> ProcessBinaryData(const vector<uint8_t>& data, const vector<size_t>& permutation, bool encrypt) {
    size_t blockSize = permutation.size();
    if (data.empty() || blockSize == 0) {
        return vector<uint8_t>();
    }
    
    vector<size_t> currentPermutation(blockSize);
    if (encrypt) {
        currentPermutation = permutation;
    } else {
        for (size_t i = 0; i < blockSize; ++i) {
            currentPermutation[permutation[i]] = i;
        }
    }
    
    vector<uint8_t> result;
    result.reserve(data.size() + blockSize);
    
    for (size_t i = 0; i < data.size(); i += blockSize) {
        size_t currentBlockSize = min(blockSize, data.size() - i);
        
        vector<uint8_t> block(blockSize, 0);
        for (size_t j = 0; j < currentBlockSize; ++j) {
            block[j] = data[i + j];
        }
        
        vector<uint8_t> processedBlock(blockSize);
        for (size_t j = 0; j < blockSize; ++j) {
            processedBlock[currentPermutation[j]] = block[j];
        }
        
        result.insert(result.end(), processedBlock.begin(), processedBlock.end());
    }
    
    if (!encrypt) {
        while (!result.empty() && result.back() == 0) {
            result.pop_back();
        }
    }
    
    return result;
}

string PermutationTextEncrypt(const string& text, const string& key) {
    vector<size_t> permutation = ParseKey(key);
    
    if (text.empty()) {
        return "";
    }
    
    vector<string> characters = SplitUTF8(text);
    string result;
    
    size_t blockSize = permutation.size();
    
    for (size_t i = 0; i < characters.size(); i += blockSize) {
        size_t currentBlockSize = min(blockSize, characters.size() - i);
        
        vector<string> block;
        for (size_t j = 0; j < currentBlockSize; ++j) {
            block.push_back(characters[i + j]);
        }
        
        while (block.size() < blockSize) {
            block.push_back(" ");
        }
        
        vector<string> encryptedBlock(blockSize);
        for (size_t j = 0; j < blockSize; ++j) {
            encryptedBlock[permutation[j]] = block[j];
        }
        
        for (const auto& ch : encryptedBlock) {
            result += ch;
        }
    }
    
    return result;
}

string PermutationTextDecrypt(const string& encryptedText, const string& key) {
    vector<size_t> permutation = ParseKey(key);
    
    if (encryptedText.empty()) {
        return "";
    }
    
    vector<string> characters = SplitUTF8(encryptedText);
    
    size_t blockSize = permutation.size();
    vector<size_t> inverse(blockSize);
    for (size_t i = 0; i < blockSize; ++i) {
        inverse[permutation[i]] = i;
    }
    
    string result;
    
    for (size_t i = 0; i < characters.size(); i += blockSize) {
        vector<string> block;
        for (size_t j = 0; j < blockSize && (i + j) < characters.size(); ++j) {
            block.push_back(characters[i + j]);
        }
        
        vector<string> decryptedBlock(blockSize);
        for (size_t j = 0; j < blockSize; ++j) {
            decryptedBlock[inverse[j]] = block[j];
        }
        
        for (const auto& ch : decryptedBlock) {
            result += ch;
        }
    }
    
    while (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    
    return result;
}

void PermutationFileEncrypt(const string& inPath, const string& outPath, const string& key) {
    vector<size_t> permutation = ParseKey(key);
    
    ifstream inputFile(inPath, ios::binary);
    if (!inputFile) {
        throw runtime_error("Не удалось открыть входной файл: " + inPath);
    }
    
    vector<uint8_t> content((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    inputFile.close();
    
    auto encrypted = ProcessBinaryData(content, permutation, true);
    
    ofstream outputFile(outPath, ios::binary);
    if (!outputFile) {
        throw runtime_error("Не удалось создать выходной файл: " + outPath);
    }
    outputFile.write(reinterpret_cast<const char*>(encrypted.data()), encrypted.size());
    outputFile.close();
}

void PermutationFileDecrypt(const string& inPath, const string& outPath, const string& key) {
    vector<size_t> permutation = ParseKey(key);
    
    ifstream inputFile(inPath, ios::binary);
    if (!inputFile) {
        throw runtime_error("Не удалось открыть входной файл: " + inPath);
    }
    
    vector<uint8_t> content((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    inputFile.close();
    
    auto decrypted = ProcessBinaryData(content, permutation, false);
    
    ofstream outputFile(outPath, ios::binary);
    if (!outputFile) {
        throw runtime_error("Не удалось создать выходной файл: " + outPath);
    }
    outputFile.write(reinterpret_cast<const char*>(decrypted.data()), decrypted.size());
    outputFile.close();
}
