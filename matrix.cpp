#include "matrix.h"
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

int ParseMatrixSize(const string& key) {
    try {
        int size = stoi(key);
        if (size < 2) {
            throw invalid_argument("Размер матрицы должен быть не менее 2");
        }
        if (size > 20) {
            throw invalid_argument("Размер матрицы не должен превышать 20");
        }
        return size;
    } catch (const exception& e) {
        throw invalid_argument("Неверный формат размера матрицы");
    }
}

string GenerateMatrixKey() {
    srand(time(0));
    int size = 3 + rand() % 18;
    
    return to_string(size);
}

vector<pair<int, int>> GenerateSpiralOrder(int size) {
    vector<pair<int, int>> order;
    order.reserve(size * size);
    
    int center = size / 2;
    int x = center, y = center;
    if (size % 2 == 0) {
        x -= 1;
        y -= 1;
    }
    
    int dx[] = {0, 1, 0, -1};
    int dy[] = {1, 0, -1, 0};
    
    int direction = 0;
    int stepSize = 1;
    int stepCount = 0;
    
    order.push_back({x, y});
    
    while (order.size() < size * size) {
        for (int i = 0; i < stepSize && order.size() < size * size; i++) {
            x += dx[direction];
            y += dy[direction];
            
            if (x >= 0 && x < size && y >= 0 && y < size) {
                order.push_back({x, y});
            }
        }
        
        direction = (direction + 1) % 4;
        stepCount++;
        
        if (stepCount % 2 == 0) {
            stepSize++;
        }
    }
    
    return order;
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

vector<uint8_t> MatrixEncryptBinary(const vector<uint8_t>& data, int size) {
    int matrixSize = size * size;
    
    vector<uint8_t> paddedData = data;
    
    if (paddedData.size() < matrixSize) {
        paddedData.resize(matrixSize, 0);
    } else if (paddedData.size() % matrixSize != 0) {
        size_t padding = matrixSize - (paddedData.size() % matrixSize);
        paddedData.resize(paddedData.size() + padding, 0);
    }
    
    auto spiralOrder = GenerateSpiralOrder(size);
    vector<uint8_t> result(paddedData.size());
    
    for (size_t block = 0; block < paddedData.size(); block += matrixSize) {
        vector<vector<uint8_t>> matrix(size, vector<uint8_t>(size, 0));
        
        for (size_t i = 0; i < size; i++) {
            for (size_t j = 0; j < size; j++) {
                int dataIndex = block + i * size + j;
                if (dataIndex < paddedData.size()) {
                    matrix[i][j] = paddedData[dataIndex];
                }
            }
        }
        
        // Читаем по спирали
        for (size_t k = 0; k < matrixSize; k++) {
            auto [i, j] = spiralOrder[k];
            result[block + k] = matrix[i][j];
        }
    }
    
    return result;
}

vector<uint8_t> MatrixDecryptBinary(const vector<uint8_t>& encryptedData, int size) {
    int matrixSize = size * size;
    
    auto spiralOrder = GenerateSpiralOrder(size);
    vector<uint8_t> result(encryptedData.size());
    
    for (size_t block = 0; block < encryptedData.size(); block += matrixSize) {
        vector<vector<uint8_t>> matrix(size, vector<uint8_t>(size, 0));

        for (size_t k = 0; k < matrixSize; k++) {
            auto [i, j] = spiralOrder[k];
            matrix[i][j] = encryptedData[block + k];
        }
        
        for (size_t i = 0; i < size; i++) {
            for (size_t j = 0; j < size; j++) {
                result[block + i * size + j] = matrix[i][j];
            }
        }
    }
    
    while (!result.empty() && result.back() == 0) {
        result.pop_back();
    }
    
    return result;
}

string MatrixTextEncrypt(const string& text, const string& key) {
    int size = ParseMatrixSize(key);
    
    if (text.length() < size * size) {
        int minSize = static_cast<int>(ceil(sqrt(text.length())));
        if (minSize < size) {
            size = minSize;
        }
    }
    
    vector<string> characters = SplitUTF8(text);
    string result;
    
    int matrixSize = size * size;
    
    for (size_t i = 0; i < characters.size(); i += matrixSize) {
        size_t currentBlockSize = min(static_cast<size_t>(matrixSize), characters.size() - i);
        
        vector<string> block;
        for (size_t j = 0; j < currentBlockSize; ++j) {
            block.push_back(characters[i + j]);
        }
        
        while (block.size() < static_cast<size_t>(matrixSize)) {
            block.push_back(" ");
        }
        
        auto spiralOrder = GenerateSpiralOrder(size);
        
        vector<vector<string>> matrix(size, vector<string>(size, " "));
        for (int row = 0; row < size; row++) {
            for (int col = 0; col < size; col++) {
                size_t index = row * size + col;
                if (index < block.size()) {
                    matrix[row][col] = block[index];
                }
            }
        }
        
        for (int k = 0; k < matrixSize; k++) {
            auto [row, col] = spiralOrder[k];
            result += matrix[row][col];
        }
    }
    
    return result;
}

string MatrixTextDecrypt(const string& encryptedText, const string& key) {
    int size = ParseMatrixSize(key);
    
    vector<string> characters = SplitUTF8(encryptedText);
    string result;
    
    int matrixSize = size * size;
    
    auto spiralOrder = GenerateSpiralOrder(size);
    
    for (size_t i = 0; i < characters.size(); i += matrixSize) {
        vector<string> block;
        for (size_t j = 0; j < static_cast<size_t>(matrixSize) && (i + j) < characters.size(); ++j) {
            block.push_back(characters[i + j]);
        }
        
        vector<vector<string>> matrix(size, vector<string>(size, " "));
        for (int k = 0; k < matrixSize && k < block.size(); k++) {
            auto [row, col] = spiralOrder[k];
            matrix[row][col] = block[k];
        }
        
        for (int row = 0; row < size; row++) {
            for (int col = 0; col < size; col++) {
                result += matrix[row][col];
            }
        }
    }
    
    while (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    
    return result;
}

void MatrixFileEncrypt(const string& inPath, const string& outPath, const string& key) {
    int size = ParseMatrixSize(key);
    
    ifstream inputFile(inPath, ios::binary);
    if (!inputFile) {
        throw runtime_error("Не удалось открыть входной файл: " + inPath);
    }
    
    vector<uint8_t> content((istreambuf_iterator<char>(inputFile)),istreambuf_iterator<char>());
    inputFile.close();
    
    auto encrypted = MatrixEncryptBinary(content, size);
    
    ofstream outputFile(outPath, ios::binary);
    if (!outputFile) {
        throw runtime_error("Не удалось создать выходной файл: " + outPath);
    }
    outputFile.write(reinterpret_cast<const char*>(encrypted.data()), encrypted.size());
    outputFile.close();
}

void MatrixFileDecrypt(const string& inPath, const string& outPath, const string& key) {
    int size = ParseMatrixSize(key);
    
    ifstream inputFile(inPath, ios::binary);
    if (!inputFile) {
        throw runtime_error("Не удалось открыть входной файл: " + inPath);
    }
    
    vector<uint8_t> content((istreambuf_iterator<char>(inputFile)),istreambuf_iterator<char>());
    inputFile.close();
    
    auto decrypted = MatrixDecryptBinary(content, size);
    
    ofstream outputFile(outPath, ios::binary);
    if (!outputFile) {
        throw runtime_error("Не удалось создать выходной файл: " + outPath);
    }
    outputFile.write(reinterpret_cast<const char*>(decrypted.data()), decrypted.size());
    outputFile.close();
}
