#include "magicsquare.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <random>
#include <ctime>
#include <stdexcept>

using namespace std;

vector<vector<int>> GenerateMagicSquare(int n) {
    vector<vector<int>> square(n, vector<int>(n, 0));

    int x = 0, y = n / 2;
    for (int num = 1; num <= n * n; num++) {
        square[x][y] = num;
        int nextX = (x + n - 1) % n;
        int nextY = (y + 1) % n;
        if (square[nextX][nextY] != 0) {
            nextX = (x + 1) % n;
            nextY = y;
        }
        x = nextX;
        y = nextY;
    }
    
    return square;
}

int ParseSize(const string& key) {
    try {
        int size = stoi(key);
        if (size < 3) {
            throw invalid_argument("Размер квадрата должен быть не менее 3");
        }
        if (size > 10 || size % 2 == 0) {
            throw invalid_argument("Размер квадрата должен быть нечётным числом от 3 до 9");
        }
        return size;
    } catch (const exception& e) {
        throw invalid_argument("Неверный формат размера квадрата");
    }
}

string GenerateMagicSquareKey() {
    srand(time(0));
    int sizes[] = {3, 5, 7, 9};
    int size = sizes[rand() % 4];
    
    return to_string(size);
}

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

vector<uint8_t> MagicSquareEncryptBinary(const vector<uint8_t>& data, int size) {
    int squareSize = size * size;
    
    vector<uint8_t> paddedData = data;
    
    if (paddedData.size() < squareSize) {
        paddedData.resize(squareSize, 0);
    } else if (paddedData.size() % squareSize != 0) {
        size_t padding = squareSize - (paddedData.size() % squareSize);
        paddedData.resize(paddedData.size() + padding, 0);
    }
    
    auto magicSquare = GenerateMagicSquare(size);
    vector<uint8_t> result(paddedData.size());
    
    vector<pair<int, int>> positions(squareSize);
    for (size_t i = 0; i < size; i++) {
        for (size_t j = 0; j < size; j++) {
            positions[magicSquare[i][j] - 1] = {i, j};
        }
    }
    
    for (int block = 0; block < paddedData.size(); block += squareSize) {
        vector<vector<uint8_t>> tempMatrix(size, vector<uint8_t>(size, 0));
        
        for (size_t k = 0; k < squareSize; k++) {
            auto [i, j] = positions[k];
            tempMatrix[i][j] = paddedData[block + k];
        }
        
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                result[block + i * size + j] = tempMatrix[i][j];
            }
        }
    }
    
    return result;
}

vector<uint8_t> MagicSquareDecryptBinary(const vector<uint8_t>& encryptedData, int size) {
    int squareSize = size * size;
    vector<uint8_t> result(encryptedData.size());
    
    auto magicSquare = GenerateMagicSquare(size);
    
    vector<pair<int, int>> positions(squareSize);
    for (size_t i = 0; i < size; i++) {
        for (size_t j = 0; j < size; j++) {
            positions[magicSquare[i][j] - 1] = {i, j};
        }
    }
    
    for (int block = 0; block < encryptedData.size(); block += squareSize) {
        vector<vector<uint8_t>> tempMatrix(size, vector<uint8_t>(size, 0));
        
        for (size_t i = 0; i < size; i++) {
            for (size_t j = 0; j < size; j++) {
                tempMatrix[i][j] = encryptedData[block + i * size + j];
            }
        }
        
        for (int k = 0; k < squareSize; k++) {
            auto [i, j] = positions[k];
            result[block + k] = tempMatrix[i][j];
        }
    }
    
    while (!result.empty() && result.back() == 0) {
        result.pop_back();
    }
    
    return result;
}

string MagicSquareTextEncrypt(const string& text, const string& key) {
    int size = ParseSize(key);
    
    vector<string> characters = SplitUTF8(text);
    string result;
    
    int squareSize = size * size;
    
    for (size_t i = 0; i < characters.size(); i += squareSize) {
        int currentBlockSize = min(static_cast<size_t>(squareSize), characters.size() - i);
        
        vector<string> block;
        for (size_t j = 0; j < currentBlockSize; ++j) {
            block.push_back(characters[i + j]);
        }
        
        while (block.size() < static_cast<size_t>(squareSize)) {
            block.push_back(" ");
        }
        
        auto magicSquare = GenerateMagicSquare(size);
        
        vector<pair<int, int>> positions(squareSize);
        for (size_t i = 0; i < size; i++) {
            for (size_t j = 0; j < size; j++) {
                positions[magicSquare[i][j] - 1] = {i, j};
            }
        }
        
        vector<vector<string>> tempMatrix(size, vector<string>(size, " "));
        for (size_t k = 0; k < squareSize; k++) {
            auto [row, col] = positions[k];
            tempMatrix[row][col] = block[k];
        }
        
        for (size_t row = 0; row < size; row++) {
            for (size_t col = 0; col < size; col++) {
                result += tempMatrix[row][col];
            }
        }
    }
    
    return result;
}

string MagicSquareTextDecrypt(const string& encryptedText, const string& key) {
    int size = ParseSize(key);
    
    vector<string> characters = SplitUTF8(encryptedText);
    string result;
    
    int squareSize = size * size;
    
    auto magicSquare = GenerateMagicSquare(size);
    
    vector<pair<int, int>> positions(squareSize);
    for (size_t i = 0; i < size; i++) {
        for (size_t j = 0; j < size; j++) {
            positions[magicSquare[i][j] - 1] = {i, j};
        }
    }
    
    for (size_t i = 0; i < characters.size(); i += squareSize) {
        vector<string> block;
        for (size_t j = 0; j < static_cast<size_t>(squareSize) && (i + j) < characters.size(); ++j) {
            block.push_back(characters[i + j]);
        }
        
        vector<vector<string>> tempMatrix(size, vector<string>(size, " "));
        for (int row = 0; row < size; row++) {
            for (int col = 0; col < size; col++) {
                size_t index = i + row * size + col;
                if (index < characters.size()) {
                    tempMatrix[row][col] = characters[index];
                }
            }
        }
        
        for (int k = 0; k < squareSize; k++) {
            auto [row, col] = positions[k];
            result += tempMatrix[row][col];
        }
    }
    
    while (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    
    return result;
}

void MagicSquareFileEncrypt(const string& inPath, const string& outPath, const string& key) {
    int size = ParseSize(key);
    
    ifstream inputFile(inPath, ios::binary);
    if (!inputFile) {
        throw runtime_error("Не удалось открыть входной файл: " + inPath);
    }
    
    vector<uint8_t> content((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    inputFile.close();
    
    auto encrypted = MagicSquareEncryptBinary(content, size);
    
    ofstream outputFile(outPath, ios::binary);
    if (!outputFile) {
        throw runtime_error("Не удалось создать выходной файл: " + outPath);
    }
    outputFile.write(reinterpret_cast<const char*>(encrypted.data()), encrypted.size());
    outputFile.close();
}

void MagicSquareFileDecrypt(const string& inPath, const string& outPath, const string& key) {
    int size = ParseSize(key);
    
    ifstream inputFile(inPath, ios::binary);
    if (!inputFile) {
        throw runtime_error("Не удалось открыть входной файл: " + inPath);
    }
    
    vector<uint8_t> content((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    inputFile.close();
    
    auto decrypted = MagicSquareDecryptBinary(content, size);
    
    ofstream outputFile(outPath, ios::binary);
    if (!outputFile) {
        throw runtime_error("Не удалось создать выходной файл: " + outPath);
    }
    outputFile.write(reinterpret_cast<const char*>(decrypted.data()), decrypted.size());
    outputFile.close();
}
