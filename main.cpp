#include <iostream>
#include <string>
#include <cstdint>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <dlfcn.h>

using namespace std;

// Объявления указателей на функции
using PermutationText = string (*)(const string&, const string&);
using PermutationFile = void (*)(const string&, const string&, const string&);
using MagicSquareText = string (*)(const string&, const string&);
using MagicSquareFile = void (*)(const string&, const string&, const string&);
using MatrixText = string (*)(const string&, const string&);
using MatrixFile = void (*)(const string&, const string&, const string&);
using GenerateKeyFunc = string (*)();
using TextFunc = string (*)(const string&, const string&);

uint64_t MenuChoice(uint64_t min, uint64_t max) {
    uint64_t choice;
    while (true) {
        cout << "Ваш выбор: ";
        cin >> choice;
        if (cin.fail()) {
            cout << "ОШИБКА! Введите число.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else if (choice < min || choice > max) {
            cout << "ОШИБКА! Введите число от " << min << " до " << max << ".\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return choice;
        }
    }
}

bool Authentication() {
    int attempts = 3;
    string password;
    bool successfulAuth = false;

    while (attempts > 0) {
        cout << "\nВведите пароль доступа: ";
        cin >> password;
        if (password == "123") {
            successfulAuth = true;
            break;
        } else {
            attempts--;
            cout << "Неправильный пароль. Попыток осталось: " << attempts << endl;
        }
    }

    if (!successfulAuth) {
        throw runtime_error("Доступ заблокирован!");
    }

    cout << "\nДоступ разрешён!\n" << endl;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return true;
}

string GetKeyUser(const string& cipherName) {
    string key;
    cout << "Введите ключ для " << cipherName << ": ";
    getline(cin, key);
    return key;
}

void SaveKeyFile(const string& cipherName, const string& key) {
    ofstream keyFile("generated_keys.txt", ios::app);
    if (keyFile.is_open()) {
        time_t now = time(nullptr);
        char timebuf[100];
        strftime(timebuf, sizeof(timebuf), "%d.%m.%Y %H:%M:%S", localtime(&now));
        
        keyFile << "[" << timebuf << "] " << cipherName << ": " << key << endl;
        keyFile.close();
        cout << "Ключ сохранен в файл generated_keys.txt" << endl;
    } else {
        cerr << "ОШИБКА: Не удалось сохранить ключ в файл." << endl;
    }
}

string GenerateKeyCipher(const string& cipherName) {
    void* handle = nullptr;
    GenerateKeyFunc pKey = nullptr;

    string libPath;
    string cipherDisplayName;
    
    string libExtension = ".so";
    
    if (cipherName == "permutation") {
        libPath = "./build/lib/libpermutation" + libExtension;
        cipherDisplayName = "Перестановка";
    } else if (cipherName == "magicsquare") {
        libPath = "./build/lib/libmagicsquare" + libExtension;
        cipherDisplayName = "Магический квадрат";
    } else if (cipherName == "matrix") {
        libPath = "./build/lib/libmatrix" + libExtension;
        cipherDisplayName = "Матричная шифровка";
    } else {
        cerr << "ОШИБКА: Неизвестный тип шифра: " << cipherName << endl;
        return "";
    }

    handle = dlopen(libPath.c_str(), RTLD_LAZY);

    if (!handle) {
        cerr << "ОШИБКА! Не удалось загрузить библиотеку: " << libPath << endl;
        return "";
    }

    // Получаем указатель на функцию
    string funcName;
    if (cipherName == "permutation") {
        funcName = "GeneratePermutationKey";
    } else if (cipherName == "magicsquare") {
        funcName = "GenerateMagicSquareKey";
    } else if (cipherName == "matrix") {
        funcName = "GenerateMatrixKey";
    }

    pKey = (GenerateKeyFunc)dlsym(handle, funcName.c_str());

    string generatedKey;
    if (pKey) {
        generatedKey = pKey();
        cout << "Сгенерированный ключ: " << generatedKey << endl;
        SaveKeyFile(cipherDisplayName, generatedKey);
    } else {
        cerr << "ОШИБКА! Не удалось найти функцию: " << funcName << endl;
    }

    if (handle) {
        dlclose(handle);
    }

    return generatedKey;
}

bool DisplayResult() {
    cout << "\nВывести результат на экран?\n";
    cout << "1. Да\n";
    cout << "2. Нет\n";
    
    uint64_t choice = MenuChoice(1, 2);
    return (choice == 1);
}

void SaveToFile(const string& content, const string& operationType) {
    cout << "\nСохранить результат в файл?\n";
    cout << "1. Да\n";
    cout << "2. Нет\n";
    
    uint64_t choice = MenuChoice(1, 2);
    if (choice == 1) {
        string filename;
        cout << "Введите имя файла для сохранения " << operationType << ": ";
        getline(cin, filename);
        
        if (filename.empty()) {
            cout << "Имя файла не может быть пустым. Отмена сохранения.\n";
            return;
        }
        
        ofstream file(filename);
        if (file.is_open()) {
            file << content;
            file.close();
            cout << "Результат успешно сохранен в файл: " << filename << endl;
        } else {
            cerr << "ОШИБКА: Не удалось создать или записать в файл " << filename << endl;
        }
    }
}

void KeyGeneratorMenu() {
    cout << "\nГЕНЕРАТОР КЛЮЧЕЙ\n";
    cout << "Выберите тип шифра для генерации ключа:\n";
    cout << "1. Перестановка\n";
    cout << "2. Матричная шифровка\n";
    cout << "3. Магический квадрат\n";
    cout << "4. Назад в главное меню\n";

    uint64_t choice = MenuChoice(1, 4);
    
    string cipherName;
    
    switch (choice) {
        case 1:
            cipherName = "permutation";
            break;
        case 2:
            cipherName = "matrix";
            break;
        case 3:
            cipherName = "magicsquare";
            break;
        case 4:
            return;
    }
    
    string key = GenerateKeyCipher(cipherName);
    if (!key.empty()) {
        cout << "Ключ успешно сгенерирован!\n";
    }
}

// Функция для обработки текстовых файлов
void ProcessTextFile(const string& inPath, const string& outPath, const string& key, bool encrypt, const string& cipherType, void* handle, TextFunc pEncrypt, TextFunc pDecrypt) {
    ifstream inputFile(inPath);
    if (!inputFile) {
        throw runtime_error("Не удалось открыть входной файл: " + inPath);
    }
    
    string content((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    inputFile.close();
    
    string result;
    if (encrypt) {
        result = pEncrypt(content, key);
        cout << "Текстовый файл зашифрован.\n";
    } else {
        result = pDecrypt(content, key);
        cout << "Текстовый файл расшифрован.\n";
    }
    
    ofstream outputFile(outPath);
    if (!outputFile) {
        throw runtime_error("Не удалось создать выходной файл: " + outPath);
    }
    outputFile << result;
    outputFile.close();
    
    if (DisplayResult()) {
        cout << "Содержимое файла:\n" << result << endl;
    }
}

void MatrixMenu() {
    void* handle = nullptr;
    MatrixText pEncryptText = nullptr, pDecryptText = nullptr;
    MatrixFile pEncryptFile = nullptr, pDecryptFile = nullptr;

    handle = dlopen("./build/lib/libmatrix.so", RTLD_LAZY);

    if (!handle) {
        cerr << "ОШИБКА! Не удалось загрузить библиотеку matrix.\n";
        return;
    }

    pEncryptText = (MatrixText)dlsym(handle, "MatrixTextEncrypt");
    pDecryptText = (MatrixText)dlsym(handle, "MatrixTextDecrypt");
    pEncryptFile = (MatrixFile)dlsym(handle, "MatrixFileEncrypt");
    pDecryptFile = (MatrixFile)dlsym(handle, "MatrixFileDecrypt");

    if (!pEncryptText || !pDecryptText || !pEncryptFile || !pDecryptFile) {
        cerr << "ОШИБКА! Не удалось найти одну или несколько функций в библиотеке matrix.\n";
        dlclose(handle);
        return;
    }

    string text, result, inPath, outPath, key;

    cout << "\nМАТРИЧНАЯ ШИФРОВКА\n";
    cout << "1. Зашифровать текст.\n";
    cout << "2. Расшифровать текст.\n";
    cout << "3. Зашифровать файл (бинарный).\n";
    cout << "4. Расшифровать файл (бинарный).\n";
    cout << "5. Зашифровать текстовый файл.\n";
    cout << "6. Расшифровать текстовый файл.\n";
    cout << "7. В главное меню.\n";

    uint64_t choice = MenuChoice(1, 7);

    if (choice == 1 || choice == 2) {
        // Обработка текста
        cout << "Введите текст для его обработки: ";
        getline(cin, text);
        
        key = GetKeyUser("матричной шифровки");
        if (key.empty()) {
            dlclose(handle);
            return;
        }
        
        try {
            if (choice == 1) {
                result = pEncryptText(text, key);
                cout << "Зашифрованный текст: " << result << endl;
                SaveToFile(result, "зашифрованного текста");
            } else {
                result = pDecryptText(text, key);
                cout << "Расшифрованный текст: " << result << endl;
                SaveToFile(result, "расшифрованного текста");
            }
        } catch (const exception& e) {
            cerr << "ОШИБКА! " << e.what() << endl;
        }
    } else if (choice == 3 || choice == 4) {
        // Обработка бинарных файлов
        cout << "Введите путь к исходному файлу: ";
        getline(cin, inPath);
        cout << "Введите путь к файлу, в который необходимо записать результат: ";
        getline(cin, outPath);
        
        key = GetKeyUser("матричной шифровки");
        if (key.empty()) {
            dlclose(handle);
            return;
        }
        
        try {
            if (choice == 3) {
                pEncryptFile(inPath, outPath, key);
                cout << "Бинарный файл зашифрован.\n";
            } else {
                pDecryptFile(inPath, outPath, key);
                cout << "Бинарный файл расшифрован.\n";
            }
            
            if (DisplayResult()) {
                ifstream resultFile(outPath, ios::binary);
                if (resultFile) {
                    string content((istreambuf_iterator<char>(resultFile)), istreambuf_iterator<char>());
                    cout << "Содержимое файла:\n" << content << endl;
                    resultFile.close();
                }
            }
        } catch (const exception& e) {
            cerr << "ОШИБКА! " << e.what() << endl;
        }
    } else if (choice == 5 || choice == 6) {
        // Обработка текстовых файлов
        cout << "Введите путь к исходному файлу: ";
        getline(cin, inPath);
        cout << "Введите путь к файлу, в который необходимо записать результат: ";
        getline(cin, outPath);
        
        key = GetKeyUser("матричной шифровки");
        if (key.empty()) {
            dlclose(handle);
            return;
        }
        
        try {
            ProcessTextFile(inPath, outPath, key, (choice == 5), "матричной шифровки", 
                          handle, pEncryptText, pDecryptText);
        } catch (const exception& e) {
            cerr << "ОШИБКА! " << e.what() << endl;
        }
    }

    dlclose(handle);
}

void MagicSquareMenu() {
    void* handle = nullptr;
    MagicSquareText pEncryptText = nullptr, pDecryptText = nullptr;
    MagicSquareFile pEncryptFile = nullptr, pDecryptFile = nullptr;

    handle = dlopen("./build/lib/libmagicsquare.so", RTLD_LAZY);

    if (!handle) {
        cerr << "ОШИБКА! Не удалось загрузить библиотеку magicsquare.\n";
        return;
    }

    pEncryptText = (MagicSquareText)dlsym(handle, "MagicSquareTextEncrypt");
    pDecryptText = (MagicSquareText)dlsym(handle, "MagicSquareTextDecrypt");
    pEncryptFile = (MagicSquareFile)dlsym(handle, "MagicSquareFileEncrypt");
    pDecryptFile = (MagicSquareFile)dlsym(handle, "MagicSquareFileDecrypt");

    if (!pEncryptText || !pDecryptText || !pEncryptFile || !pDecryptFile) {
        cerr << "ОШИБКА! Не удалось найти одну или несколько функций в библиотеке magicsquare.\n";
        dlclose(handle);
        return;
    }

    string text, result, inPath, outPath, key;

    cout << "\nШИФРОВАНИЕ МАГИЧЕСКИМ КВАДРАТОМ\n";
    cout << "1. Зашифровать текст.\n";
    cout << "2. Расшифровать текст.\n";
    cout << "3. Зашифровать файл (бинарный).\n";
    cout << "4. Расшифровать файл (бинарный).\n";
    cout << "5. Зашифровать текстовый файл.\n";
    cout << "6. Расшифровать текстовый файл.\n";
    cout << "7. В главное меню.\n";

    uint64_t choice = MenuChoice(1, 7);

    if (choice == 1 || choice == 2) {
        // Обработка текста
        cout << "Введите текст для его обработки: ";
        getline(cin, text);
        
        key = GetKeyUser("магического квадрата");
        if (key.empty()) {
            dlclose(handle);
            return;
        }
        
        try {
            if (choice == 1) {
                result = pEncryptText(text, key);
                cout << "Зашифрованный текст: " << result << endl;
                SaveToFile(result, "зашифрованного текста");
            } else {
                result = pDecryptText(text, key);
                cout << "Расшифрованный текст: " << result << endl;
                SaveToFile(result, "расшифрованного текста");
            }
        } catch (const exception& e) {
            cerr << "ОШИБКА! " << e.what() << endl;
        }
    } else if (choice == 3 || choice == 4) {
        // Обработка бинарных файлов
        cout << "Введите путь к исходному файлу: ";
        getline(cin, inPath);
        cout << "Введите путь к файлу, в который необходимо записать результат: ";
        getline(cin, outPath);
        
        key = GetKeyUser("магического квадрата");
        if (key.empty()) {
            dlclose(handle);
            return;
        }
        
        try {
            if (choice == 3) {
                pEncryptFile(inPath, outPath, key);
                cout << "Бинарный файл зашифрован.\n";
            } else {
                pDecryptFile(inPath, outPath, key);
                cout << "Бинарный файл расшифрован.\n";
            }
            
            if (DisplayResult()) {
                ifstream resultFile(outPath, ios::binary);
                if (resultFile) {
                    string content((istreambuf_iterator<char>(resultFile)), istreambuf_iterator<char>());
                    cout << "Содержимое файла:\n" << content << endl;
                    resultFile.close();
                }
            }
        } catch (const exception& e) {
            cerr << "ОШИБКА! " << e.what() << endl;
        }
    } else if (choice == 5 || choice == 6) {
        // Обработка текстовых файлов
        cout << "Введите путь к исходному файлу: ";
        getline(cin, inPath);
        cout << "Введите путь к файлу, в который необходимо записать результат: ";
        getline(cin, outPath);
        
        key = GetKeyUser("магического квадрата");
        if (key.empty()) {
            dlclose(handle);
            return;
        }
        
        try {
            ProcessTextFile(inPath, outPath, key, (choice == 5), "магического квадрата", 
                          handle, pEncryptText, pDecryptText);
        } catch (const exception& e) {
            cerr << "ОШИБКА! " << e.what() << endl;
        }
    }

    dlclose(handle);
}

void PermutationMenu() {
    void* handle = nullptr;
    PermutationText pEncryptText = nullptr, pDecryptText = nullptr;
    PermutationFile pEncryptFile = nullptr, pDecryptFile = nullptr;

    handle = dlopen("./build/lib/libpermutation.so", RTLD_LAZY);

    if (!handle) {
        cerr << "ОШИБКА! Не удалось загрузить библиотеку permutation.\n";
        return;
    }

    pEncryptText = (PermutationText)dlsym(handle, "PermutationTextEncrypt");
    pDecryptText = (PermutationText)dlsym(handle, "PermutationTextDecrypt");
    pEncryptFile = (PermutationFile)dlsym(handle, "PermutationFileEncrypt");
    pDecryptFile = (PermutationFile)dlsym(handle, "PermutationFileDecrypt");

    if (!pEncryptText || !pDecryptText || !pEncryptFile || !pDecryptFile) {
        cerr << "ОШИБКА! Не удалось найти одну или несколько функций в библиотеке permutation.\n";
        dlclose(handle);
        return;
    }

    string text, result, inPath, outPath, key;

    cout << "\nШИФР ПЕРЕСТАНОВКИ\n";
    cout << "1. Зашифровать текст.\n";
    cout << "2. Расшифровать текст.\n";
    cout << "3. Зашифровать файл (бинарный).\n";
    cout << "4. Расшифровать файл (бинарный).\n";
    cout << "5. Зашифровать текстовый файл.\n";
    cout << "6. Расшифровать текстовый файл.\n";
    cout << "7. В главное меню.\n";

    uint64_t choice = MenuChoice(1, 7);

    if (choice == 1 || choice == 2) {
        // Обработка текста
        cout << "Введите текст для его обработки: ";
        getline(cin, text);
        
        key = GetKeyUser("шифра перестановки");
        if (key.empty()) {
            dlclose(handle);
            return;
        }
        
        try {
            if (choice == 1) {
                result = pEncryptText(text, key);
                cout << "Зашифрованный текст: " << result << endl;
                SaveToFile(result, "зашифрованного текста");
            } else {
                result = pDecryptText(text, key);
                cout << "Расшифрованный текст: " << result << endl;
                SaveToFile(result, "расшифрованного текста");
            }
        } catch (const exception& e) {
            cerr << "ОШИБКА! " << e.what() << endl;
        }
    } else if (choice == 3 || choice == 4) {
        // Обработка бинарных файлов
        cout << "Введите путь к исходному файлу: ";
        getline(cin, inPath);
        cout << "Введите путь к файлу, в который необходимо записать результат: ";
        getline(cin, outPath);
        
        key = GetKeyUser("шифра перестановки");
        if (key.empty()) {
            dlclose(handle);
            return;
        }
        
        try {
            if (choice == 3) {
                pEncryptFile(inPath, outPath, key);
                cout << "Бинарный файл зашифрован.\n";
            } else {
                pDecryptFile(inPath, outPath, key);
                cout << "Бинарный файл расшифрован.\n";
            }
            
            if (DisplayResult()) {
                ifstream resultFile(outPath, ios::binary);
                if (resultFile) {
                    string content((istreambuf_iterator<char>(resultFile)), istreambuf_iterator<char>());
                    cout << "Содержимое файла:\n" << content << endl;
                    resultFile.close();
                }
            }
        } catch (const exception& e) {
            cerr << "ОШИБКА! " << e.what() << endl;
        }
    } else if (choice == 5 || choice == 6) {
        // Обработка текстовых файлов
        cout << "Введите путь к исходному файлу: ";
        getline(cin, inPath);
        cout << "Введите путь к файлу, в который необходимо записать результат: ";
        getline(cin, outPath);
        
        key = GetKeyUser("шифра перестановки");
        if (key.empty()) {
            dlclose(handle);
            return;
        }
        
        try {
            ProcessTextFile(inPath, outPath, key, (choice == 5), "шифра перестановки", 
                          handle, pEncryptText, pDecryptText);
        } catch (const exception& e) {
            cerr << "ОШИБКА! " << e.what() << endl;
        }
    }

    dlclose(handle);
}

int main() {
    srand(time(0));

    try {
        cout << "ПРОГРАММА ШИФРОВАНИЯ/ДЕШИФРОВАНИЯ\n";
        
        Authentication();

        while (true) {
            cout << "\nГЛАВНОЕ МЕНЮ\n";
            cout << "Выберите метод шифрования (1-3) или действие:\n";
            cout << "1. Перестановка\n";
            cout << "2. Матричная шифровка\n";
            cout << "3. Магический квадрат\n";
            cout << "4. Переход к генератору ключей\n";
            cout << "5. Выход\n";

            uint64_t choice = MenuChoice(1, 5);
            switch (choice) {
                case 1:
                    PermutationMenu();
                    break;
                case 2:
                    MatrixMenu();
                    break;
                case 3:
                    MagicSquareMenu();
                    break;
                case 4:
                    KeyGeneratorMenu();
                    break;
                case 5:
                    cout << "Выход из программы.\n";
                    return 0;
            }
        }

    } catch (const exception& e) {
        cerr << "ОШИБКА: " << e.what() << endl;
        cerr << "Нажмите Enter для выхода из программы." << endl;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        return 1;
    }

    return 0;
}
