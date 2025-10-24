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

std::vector<size_t> parseKey(const std::string& key) {
    std::vector<size_t> permutation;
    std::stringstream ss(key);
    std::string token;
    
    while (std::getline(ss, token, '-')) {
        try {
            int num = std::stoi(token);
            if (num <= 0) {
                throw std::invalid_argument("Ключ должен содержать только положительные числа");
            }
            permutation.push_back(static_cast<size_t>(num - 1)); // Convert to 0-based indexing
        } catch (const std::exception& e) {
            throw std::invalid_argument("Неверный формат ключа. Используйте формат: 3-1-4-2");
        }
    }
    
    // Validate permutation
    std::vector<size_t> sorted = permutation;
    std::sort(sorted.begin(), sorted.end());
    for (size_t i = 0; i < sorted.size(); ++i) {
        if (sorted[i] != i) {
            throw std::invalid_argument("Ключ должен быть перестановкой чисел от 1 до N");
        }
    }
    
    return permutation;
}

std::string GeneratePermutationKey() {
    // Генерация случайного размера блока от 3 до 8
    std::srand(std::time(0));
    int blockSize = 3 + std::rand() % 6; // от 3 до 8
    
    // Создание последовательности чисел от 1 до blockSize
    std::vector<int> numbers(blockSize);
    for (int i = 0; i < blockSize; ++i) {
        numbers[i] = i + 1;
    }
    
    // Перемешивание последовательности
    for (int i = blockSize - 1; i > 0; --i) {
        int j = std::rand() % (i + 1);
        std::swap(numbers[i], numbers[j]);
    }
    
    // Форматирование ключа
    std::stringstream keyStream;
    for (int i = 0; i < blockSize; ++i) {
        if (i > 0) keyStream << "-";
        keyStream << numbers[i];
    }
    
    return keyStream.str();
}

std::vector<uint8_t> encryptBinary(const std::vector<uint8_t>& data, const std::vector<size_t>& permutation) {
    size_t blockSize = permutation.size();
    std::vector<uint8_t> result;
    result.reserve(data.size());
    
    // Обрабатываем данные блоками
    for (size_t i = 0; i < data.size(); i += blockSize) {
        size_t current_block_size = std::min(blockSize, data.size() - i);
        
        // Создаем блок и применяем перестановку
        std::vector<uint8_t> block(blockSize, 0);
        
        // Копируем данные в блок
        for (size_t j = 0; j < current_block_size; ++j) {
            block[j] = data[i + j];
        }
        
        // Применяем перестановку
        for (size_t j = 0; j < blockSize; ++j) {
            result.push_back(block[permutation[j]]);
        }
    }
    
    return result;
}

std::vector<uint8_t> decryptBinary(const std::vector<uint8_t>& encrypted_data, const std::vector<size_t>& permutation) {
    size_t blockSize = permutation.size();
    std::vector<uint8_t> result;
    result.reserve(encrypted_data.size());
    
    // Создаем обратную перестановку
    std::vector<size_t> inverse(blockSize);
    for (size_t i = 0; i < blockSize; ++i) {
        inverse[permutation[i]] = i;
    }
    
    // Обрабатываем данные блоками
    for (size_t i = 0; i < encrypted_data.size(); i += blockSize) {
        size_t current_block_size = std::min(blockSize, encrypted_data.size() - i);
        
        // Создаем блок
        std::vector<uint8_t> block(blockSize, 0);
        
        // Копируем зашифрованные данные
        for (size_t j = 0; j < current_block_size; ++j) {
            block[j] = encrypted_data[i + j];
        }
        
        // Применяем обратную перестановку
        for (size_t j = 0; j < blockSize; ++j) {
            result.push_back(block[inverse[j]]);
        }
    }
    
    return result;
}

std::vector<uint8_t> encryptText(const std::vector<uint8_t>& data, const std::vector<size_t>& permutation) {
    size_t blockSize = permutation.size();
    std::vector<uint8_t> result;
    result.reserve(data.size());
    
    // Обрабатываем данные блоками
    for (size_t i = 0; i < data.size(); i += blockSize) {
        size_t current_block_size = std::min(blockSize, data.size() - i);
        
        // Создаем блок и применяем перестановку
        std::vector<uint8_t> block(blockSize, ' '); // Заполняем пробелами
        
        // Копируем данные в блок
        for (size_t j = 0; j < current_block_size; ++j) {
            block[j] = data[i + j];
        }
        
        // Применяем перестановку
        for (size_t j = 0; j < blockSize; ++j) {
            result.push_back(block[permutation[j]]);
        }
    }
    
    return result;
}

std::vector<uint8_t> decryptText(const std::vector<uint8_t>& encrypted_data, const std::vector<size_t>& permutation) {
    size_t blockSize = permutation.size();
    std::vector<uint8_t> result;
    result.reserve(encrypted_data.size());
    
    // Создаем обратную перестановку
    std::vector<size_t> inverse(blockSize);
    for (size_t i = 0; i < blockSize; ++i) {
        inverse[permutation[i]] = i;
    }
    
    // Обрабатываем данные блоками
    for (size_t i = 0; i < encrypted_data.size(); i += blockSize) {
        size_t current_block_size = std::min(blockSize, encrypted_data.size() - i);
        
        // Создаем блок
        std::vector<uint8_t> block(blockSize, ' ');
        
        // Копируем зашифрованные данные
        for (size_t j = 0; j < current_block_size; ++j) {
            block[j] = encrypted_data[i + j];
        }
        
        // Применяем обратную перестановку
        for (size_t j = 0; j < blockSize; ++j) {
            result.push_back(block[inverse[j]]);
        }
    }
    
    // Убираем лишние пробелы в конце для текста
    while (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    
    return result;
}

std::string PermutationTextEncrypt(const std::string& text, const std::string& key) {
    std::vector<size_t> permutation = parseKey(key);
    
    // Конвертируем текст в бинарные данные
    std::vector<uint8_t> text_data(text.begin(), text.end());
    
    // Шифруем с использованием пробелов для текста
    auto encrypted_data = encryptText(text_data, permutation);
    
    // Конвертируем в hex-строку вместо обычной строки
    return bytesToHex(encrypted_data);
}

std::string PermutationTextDecrypt(const std::string& hexText, const std::string& key) {
    std::vector<size_t> permutation = parseKey(key);
    
    // Конвертируем hex-строку обратно в бинарные данные
    auto encrypted_data = hexToBytes(hexText);
    
    // Дешифруем с обработкой пробелов для текста
    auto decrypted_data = decryptText(encrypted_data, permutation);
    
    // Конвертируем обратно в строку
    std::string result(decrypted_data.begin(), decrypted_data.end());
    return result;
}

void PermutationFileEncrypt(const std::string& inPath, const std::string& outPath, const std::string& key) {
    std::vector<size_t> permutation = parseKey(key);
    
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
    auto encrypted = encryptBinary(content, permutation);
    
    // Записываем результат как hex-строку
    std::ofstream outputFile(outPath);
    if (!outputFile) {
        throw std::runtime_error("Не удалось создать выходной файл: " + outPath);
    }
    outputFile << bytesToHex(encrypted);
    outputFile.close();
}

void PermutationFileDecrypt(const std::string& inPath, const std::string& outPath, const std::string& key) {
    std::vector<size_t> permutation = parseKey(key);
    
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
    auto decrypted = decryptBinary(content, permutation);
    
    // Записываем расшифрованные данные как бинарные
    std::ofstream outputFile(outPath, std::ios::binary);
    if (!outputFile) {
        throw std::runtime_error("Не удалось создать выходной файл: " + outPath);
    }
    outputFile.write(reinterpret_cast<const char*>(decrypted.data()), decrypted.size());
    outputFile.close();
}
