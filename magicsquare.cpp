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
#include <iomanip>

// Функция для преобразования байтов в hex-строку
std::string bytesToHex(const std::vector<uint8_t>& data) {
    std::stringstream ss;
    ss << std::hex << std::uppercase << std::setfill('0');
    for (size_t i = 0; i < data.size(); ++i) {
        ss << std::setw(2) << static_cast<int>(data[i]);
        if (i < data.size() - 1) {
            ss << " ";
        }
    }
    return ss.str();
}

// Функция для преобразования hex-строки обратно в байты
std::vector<uint8_t> hexToBytes(const std::string& hexStr) {
    std::vector<uint8_t> bytes;
    std::stringstream ss(hexStr);
    std::string hexByte;
    
    while (ss >> hexByte) {
        try {
            int byte = std::stoi(hexByte, nullptr, 16);
            bytes.push_back(static_cast<uint8_t>(byte));
        } catch (...) {
            throw std::invalid_argument("Неверный формат hex-строки");
        }
    }
    
    return bytes;
}

std::vector<std::vector<int>> generateMagicSquare(int size) {
    std::vector<std::vector<int>> square(size, std::vector<int>(size, 0));
    
    if (size % 2 == 1) {
        // Odd size magic square (Siamese method)
        int i = 0;
        int j = size / 2;
        
        for (int num = 1; num <= size * size; num++) {
            square[i][j] = num;
            
            int new_i = (i - 1 + size) % size;
            int new_j = (j + 1) % size;
            
            if (square[new_i][new_j] != 0) {
                i = (i + 1) % size;
            } else {
                i = new_i;
                j = new_j;
            }
        }
    } else {
        // Even size magic square (simple method)
        int num = 1;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                square[i][j] = num++;
            }
        }
    }
    
    return square;
}

int parseSize(const std::string& key) {
    try {
        int size = std::stoi(key);
        if (size < 2) {
            throw std::invalid_argument("Размер квадрата должен быть не менее 2");
        }
        if (size > 10) {
            throw std::invalid_argument("Размер квадрата не должен превышать 10");
        }
        return size;
    } catch (const std::exception& e) {
        throw std::invalid_argument("Неверный формат размера квадрата");
    }
}

std::string GenerateMagicSquareKey() {
    // Генерация случайного размера квадрата от 3 до 6
    std::srand(std::time(0));
    int size = 3 + std::rand() % 4; // от 3 до 6
    
    return std::to_string(size);
}

std::vector<uint8_t> encryptMagicSquareBinary(const std::vector<uint8_t>& data, int size) {
    int squareSize = size * size;
    
    // Создаем копию данных с дополнением
    std::vector<uint8_t> paddedData = data;
    size_t originalSize = data.size();
    
    if (paddedData.size() < squareSize) {
        paddedData.resize(squareSize, 0);
    } else if (paddedData.size() % squareSize != 0) {
        size_t padding = squareSize - (paddedData.size() % squareSize);
        paddedData.resize(paddedData.size() + padding, 0);
    }
    
    auto magicSquare = generateMagicSquare(size);
    std::vector<uint8_t> result(paddedData.size());
    
    // Create reading order from magic square
    std::vector<std::pair<int, int>> positions(squareSize);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            positions[magicSquare[i][j] - 1] = {i, j};
        }
    }
    
    // Encrypt each block using magic square order
    for (size_t block = 0; block < paddedData.size(); block += squareSize) {
        // Create temporary block
        std::vector<uint8_t> tempBlock(squareSize);
        
        // Fill temp block in original order
        for (int k = 0; k < squareSize; k++) {
            auto [i, j] = positions[k];
            tempBlock[k] = paddedData[block + i * size + j];
        }
        
        // Copy encrypted block to result
        for (int k = 0; k < squareSize; k++) {
            result[block + k] = tempBlock[k];
        }
    }
    
    return result;
}

std::vector<uint8_t> decryptMagicSquareBinary(const std::vector<uint8_t>& encrypted_data, int size) {
    int squareSize = size * size;
    
    if (encrypted_data.size() % squareSize != 0) {
        throw std::invalid_argument("Длина данных должна быть кратна " + std::to_string(squareSize));
    }
    
    auto magicSquare = generateMagicSquare(size);
    std::vector<uint8_t> result(encrypted_data.size());
    
    // Create reading order from magic square
    std::vector<std::pair<int, int>> positions(squareSize);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            positions[magicSquare[i][j] - 1] = {i, j};
        }
    }
    
    // Decrypt each block using magic square order
    for (size_t block = 0; block < encrypted_data.size(); block += squareSize) {
        // Create temporary block
        std::vector<uint8_t> tempBlock(squareSize);
        
        // Fill temp block in encrypted order
        for (int k = 0; k < squareSize; k++) {
            tempBlock[k] = encrypted_data[block + k];
        }
        
        // Restore original order
        for (int k = 0; k < squareSize; k++) {
            auto [i, j] = positions[k];
            result[block + i * size + j] = tempBlock[k];
        }
    }
    
    return result;
}

std::vector<uint8_t> encryptMagicSquareText(const std::vector<uint8_t>& data, int size) {
    int squareSize = size * size;
    
    // Создаем копию данных с дополнением ПРОБЕЛАМИ для текста
    std::vector<uint8_t> paddedData = data;
    
    if (paddedData.size() < squareSize) {
        // Дополняем пробелами до полной матрицы
        paddedData.resize(squareSize, ' '); // Пробел для текста
    } else if (paddedData.size() % squareSize != 0) {
        // Дополняем пробелами до кратного размера матрицы
        size_t padding = squareSize - (paddedData.size() % squareSize);
        paddedData.resize(paddedData.size() + padding, ' ');
    }
    
    auto magicSquare = generateMagicSquare(size);
    std::vector<uint8_t> result(paddedData.size());
    
    // Create reading order from magic square
    std::vector<std::pair<int, int>> positions(squareSize);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            positions[magicSquare[i][j] - 1] = {i, j};
        }
    }
    
    // Encrypt each block using magic square order
    for (size_t block = 0; block < paddedData.size(); block += squareSize) {
        // Create temporary block
        std::vector<uint8_t> tempBlock(squareSize);
        
        // Fill temp block in original order
        for (int k = 0; k < squareSize; k++) {
            auto [i, j] = positions[k];
            tempBlock[k] = paddedData[block + i * size + j];
        }
        
        // Copy encrypted block to result
        for (int k = 0; k < squareSize; k++) {
            result[block + k] = tempBlock[k];
        }
    }
    
    return result;
}

std::vector<uint8_t> decryptMagicSquareText(const std::vector<uint8_t>& encrypted_data, int size) {
    int squareSize = size * size;
    
    if (encrypted_data.size() % squareSize != 0) {
        throw std::invalid_argument("Длина данных должна быть кратна " + std::to_string(squareSize));
    }
    
    auto magicSquare = generateMagicSquare(size);
    std::vector<uint8_t> result(encrypted_data.size());
    
    // Create reading order from magic square
    std::vector<std::pair<int, int>> positions(squareSize);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            positions[magicSquare[i][j] - 1] = {i, j};
        }
    }
    
    // Decrypt each block using magic square order
    for (size_t block = 0; block < encrypted_data.size(); block += squareSize) {
        // Create temporary block
        std::vector<uint8_t> tempBlock(squareSize);
        
        // Fill temp block in encrypted order
        for (int k = 0; k < squareSize; k++) {
            tempBlock[k] = encrypted_data[block + k];
        }
        
        // Restore original order
        for (int k = 0; k < squareSize; k++) {
            auto [i, j] = positions[k];
            result[block + i * size + j] = tempBlock[k];
        }
    }
    
    // Убираем лишние пробелы в конце для текста
    while (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    
    return result;
}

std::string MagicSquareTextEncrypt(const std::string& text, const std::string& key) {
    int size = parseSize(key);
    
    // Конвертируем текст в бинарные данные
    std::vector<uint8_t> text_data(text.begin(), text.end());
    
    // Шифруем с использованием пробелов для текста
    auto encrypted_data = encryptMagicSquareText(text_data, size);
    
    // Конвертируем в hex-строку вместо обычной строки
    return bytesToHex(encrypted_data);
}

std::string MagicSquareTextDecrypt(const std::string& hexText, const std::string& key) {
    int size = parseSize(key);
    
    // Конвертируем hex-строку обратно в бинарные данные
    auto encrypted_data = hexToBytes(hexText);
    
    // Дешифруем с обработкой пробелов для текста
    auto decrypted_data = decryptMagicSquareText(encrypted_data, size);
    
    // Конвертируем обратно в строку
    std::string result(decrypted_data.begin(), decrypted_data.end());
    return result;
}

void MagicSquareFileEncrypt(const std::string& inPath, const std::string& outPath, const std::string& key) {
    int size = parseSize(key);
    
    std::ifstream inputFile(inPath, std::ios::binary);
    if (!inputFile) {
        throw std::runtime_error("Не удалось открыть входной файл: " + inPath);
    }
    
    // Читаем все данные как бинарные
    std::vector<uint8_t> content(
        (std::istreambuf_iterator<char>(inputFile)),
        std::istreambuf_iterator<char>()
    );
    inputFile.close();
    
    // Для ВСЕХ файлов используем бинарное шифрование
    auto encrypted = encryptMagicSquareBinary(content, size);
    
    // Записываем результат как hex-строку
    std::ofstream outputFile(outPath);
    if (!outputFile) {
        throw std::runtime_error("Не удалось создать выходной файл: " + outPath);
    }
    outputFile << bytesToHex(encrypted);
    outputFile.close();
}

void MagicSquareFileDecrypt(const std::string& inPath, const std::string& outPath, const std::string& key) {
    int size = parseSize(key);
    
    std::ifstream inputFile(inPath);
    if (!inputFile) {
        throw std::runtime_error("Не удалось открыть входной файл: " + inPath);
    }
    
    // Читаем hex-строку из файла
    std::string hexContent;
    std::getline(inputFile, hexContent);
    inputFile.close();
    
    // Преобразуем hex-строку в бинарные данные
    auto content = hexToBytes(hexContent);
    
    // Дешифруем
    auto decrypted = decryptMagicSquareBinary(content, size);
    
    // Записываем расшифрованные данные как бинарные
    std::ofstream outputFile(outPath, std::ios::binary);
    if (!outputFile) {
        throw std::runtime_error("Не удалось создать выходной файл: " + outPath);
    }
    outputFile.write(reinterpret_cast<const char*>(decrypted.data()), decrypted.size());
    outputFile.close();
}
