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

int parseMatrixSize(const std::string& key) {
    try {
        int size = std::stoi(key);
        if (size < 2) {
            throw std::invalid_argument("Размер матрицы должен быть не менее 2");
        }
        if (size > 20) {
            throw std::invalid_argument("Размер матрицы не должен превышать 20");
        }
        return size;
    } catch (const std::exception& e) {
        throw std::invalid_argument("Неверный формат размера матрицы");
    }
}

std::string GenerateMatrixKey() {
    // Генерация случайного размера матрицы от 3 до 8
    std::srand(std::time(0));
    int size = 3 + std::rand() % 6; // от 3 до 8
    
    return std::to_string(size);
}

// Функция для получения координат спирального обхода из центра
std::vector<std::pair<int, int>> generateSpiralOrder(int size) {
    std::vector<std::pair<int, int>> order;
    order.reserve(size * size);
    
    int center = size / 2;
    int x = center, y = center;
    
    // Направления: вправо, вниз, влево, вверх
    int dx[] = {1, 0, -1, 0};
    int dy[] = {0, 1, 0, -1};
    
    int direction = 0; // начинаем двигаться вправо
    int stepSize = 1;
    int stepCount = 0;
    
    order.push_back({x, y});
    
    while (order.size() < size * size) {
        for (int i = 0; i < stepSize && order.size() < size * size; i++) {
            x += dx[direction];
            y += dy[direction];
            
            // Проверяем границы
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

std::vector<uint8_t> encryptMatrixBinary(const std::vector<uint8_t>& data, int size) {
    int matrixSize = size * size;
    
    // Создаем копию данных
    std::vector<uint8_t> paddedData = data;
    size_t originalSize = data.size();
    
    // Дополняем до полной матрицы нулями (для бинарных данных)
    if (paddedData.size() < matrixSize) {
        paddedData.resize(matrixSize, 0);
    } else if (paddedData.size() % matrixSize != 0) {
        size_t padding = matrixSize - (paddedData.size() % matrixSize);
        paddedData.resize(paddedData.size() + padding, 0);
    }
    
    auto spiralOrder = generateSpiralOrder(size);
    std::vector<uint8_t> result(paddedData.size());
    
    // Шифруем каждый блок
    for (size_t block = 0; block < paddedData.size(); block += matrixSize) {
        // Создаем матрицу для текущего блока
        std::vector<std::vector<uint8_t>> matrix(size, std::vector<uint8_t>(size, 0));
        
        // Записываем данные в матрицу по строкам
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                size_t dataIndex = block + i * size + j;
                if (dataIndex < paddedData.size()) {
                    matrix[i][j] = paddedData[dataIndex];
                }
            }
        }
        
        // Читаем данные из матрицы по спирали
        for (int k = 0; k < matrixSize; k++) {
            auto [i, j] = spiralOrder[k];
            result[block + k] = matrix[i][j];
        }
    }
    
    return result;
}

std::vector<uint8_t> decryptMatrixBinary(const std::vector<uint8_t>& encrypted_data, int size) {
    int matrixSize = size * size;
    
    if (encrypted_data.size() % matrixSize != 0) {
        throw std::invalid_argument("Длина данных должна быть кратна " + std::to_string(matrixSize));
    }
    
    auto spiralOrder = generateSpiralOrder(size);
    std::vector<uint8_t> result(encrypted_data.size());
    
    // Дешифруем каждый блок
    for (size_t block = 0; block < encrypted_data.size(); block += matrixSize) {
        // Создаем матрицу для текущего блока
        std::vector<std::vector<uint8_t>> matrix(size, std::vector<uint8_t>(size, 0));
        
        // Записываем зашифрованные данные в матрицу по спирали
        for (int k = 0; k < matrixSize; k++) {
            auto [i, j] = spiralOrder[k];
            matrix[i][j] = encrypted_data[block + k];
        }
        
        // Читаем данные из матрицы по строкам
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                result[block + i * size + j] = matrix[i][j];
            }
        }
    }
    
    return result;
}

// Специальные функции для работы с текстом (с пробелами)
std::vector<uint8_t> encryptMatrixText(const std::vector<uint8_t>& data, int size) {
    int matrixSize = size * size;
    
    // Создаем копию данных с дополнением ПРОБЕЛАМИ для текста
    std::vector<uint8_t> paddedData = data;
    
    if (paddedData.size() < matrixSize) {
        // Дополняем пробелами до полной матрицы
        paddedData.resize(matrixSize, ' '); // Пробел для текста
    } else if (paddedData.size() % matrixSize != 0) {
        // Дополняем пробелами до кратного размера матрицы
        size_t padding = matrixSize - (paddedData.size() % matrixSize);
        paddedData.resize(paddedData.size() + padding, ' ');
    }
    
    auto spiralOrder = generateSpiralOrder(size);
    std::vector<uint8_t> result(paddedData.size());
    
    // Шифруем каждый блок
    for (size_t block = 0; block < paddedData.size(); block += matrixSize) {
        // Создаем матрицу для текущего блока
        std::vector<std::vector<uint8_t>> matrix(size, std::vector<uint8_t>(size, ' '));
        
        // Записываем данные в матрицу по строкам
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                size_t dataIndex = block + i * size + j;
                if (dataIndex < paddedData.size()) {
                    matrix[i][j] = paddedData[dataIndex];
                }
            }
        }
        
        // Читаем данные из матрицы по спирали
        for (int k = 0; k < matrixSize; k++) {
            auto [i, j] = spiralOrder[k];
            result[block + k] = matrix[i][j];
        }
    }
    
    return result;
}

std::vector<uint8_t> decryptMatrixText(const std::vector<uint8_t>& encrypted_data, int size) {
    int matrixSize = size * size;
    
    if (encrypted_data.size() % matrixSize != 0) {
        throw std::invalid_argument("Длина данных должна быть кратна " + std::to_string(matrixSize));
    }
    
    auto spiralOrder = generateSpiralOrder(size);
    std::vector<uint8_t> result(encrypted_data.size());
    
    // Дешифруем каждый блок
    for (size_t block = 0; block < encrypted_data.size(); block += matrixSize) {
        // Создаем матрицу для текущего блока
        std::vector<std::vector<uint8_t>> matrix(size, std::vector<uint8_t>(size, ' '));
        
        // Записываем зашифрованные данные в матрицу по спирали
        for (int k = 0; k < matrixSize; k++) {
            auto [i, j] = spiralOrder[k];
            matrix[i][j] = encrypted_data[block + k];
        }
        
        // Читаем данные из матрицы по строкам
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                result[block + i * size + j] = matrix[i][j];
            }
        }
    }
    
    // Убираем лишние пробелы в конце для текста
    while (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    
    return result;
}

std::string MatrixTextEncrypt(const std::string& text, const std::string& key) {
    int size = parseMatrixSize(key);
    
    // Для коротких текстов используем меньший размер матрицы
    if (text.length() < size * size) {
        // Автоматически подбираем минимальный подходящий размер
        int minSize = static_cast<int>(std::ceil(std::sqrt(text.length())));
        if (minSize < size) {
            size = minSize;
        }
    }
    
    // Конвертируем текст в бинарные данные
    std::vector<uint8_t> text_data(text.begin(), text.end());
    
    // Шифруем с использованием пробелов для текста
    auto encrypted_data = encryptMatrixText(text_data, size);
    
    // Конвертируем в hex-строку вместо обычной строки
    return bytesToHex(encrypted_data);
}

std::string MatrixTextDecrypt(const std::string& hexText, const std::string& key) {
    int size = parseMatrixSize(key);
    
    // Конвертируем hex-строку обратно в бинарные данные
    auto encrypted_data = hexToBytes(hexText);
    
    // Дешифруем с обработкой пробелов для текста
    auto decrypted_data = decryptMatrixText(encrypted_data, size);
    
    // Конвертируем обратно в строку
    std::string result(decrypted_data.begin(), decrypted_data.end());
    return result;
}

void MatrixFileEncrypt(const std::string& inPath, const std::string& outPath, const std::string& key) {
    int size = parseMatrixSize(key);
    
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
    auto encrypted = encryptMatrixBinary(content, size);
    
    // Записываем результат как hex-строку
    std::ofstream outputFile(outPath);
    if (!outputFile) {
        throw std::runtime_error("Не удалось создать выходной файл: " + outPath);
    }
    outputFile << bytesToHex(encrypted);
    outputFile.close();
}

void MatrixFileDecrypt(const std::string& inPath, const std::string& outPath, const std::string& key) {
    int size = parseMatrixSize(key);
    
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
    auto decrypted = decryptMatrixBinary(content, size);
    
    // Записываем расшифрованные данные как бинарные
    std::ofstream outputFile(outPath, std::ios::binary);
    if (!outputFile) {
        throw std::runtime_error("Не удалось создать выходной файл: " + outPath);
    }
    outputFile.write(reinterpret_cast<const char*>(decrypted.data()), decrypted.size());
    outputFile.close();
}
