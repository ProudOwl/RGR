#include <iostream>
#include <string>
#include <cstdint>
#include <limits>
#include <cstdlib>
#include <ctime>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

using std::string;
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::numeric_limits;
using std::streamsize;

using PermutationText = string (*)(const string&, const string&);
using PermutationFile = void (*)(const string&, const string&, const string&);
using MagicSquareText = string (*)(const string&, const string&);
using MagicSquareFile = void (*)(const string&, const string&, const string&);
using MatrixText = string (*)(const string&, const string&);
using MatrixFile = void (*)(const string&, const string&, const string&);
using GenerateKeyFunc = string (*)();

void WindowsConsole() {
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif
}

uint64_t MenuChoice(uint64_t min, uint64_t max) {
    uint64_t choice;
    while (true) {
        cout << u8">> ";
        cin >> choice;
        if (cin.fail()) {
            cout << u8"ОШИБКА! Введите число.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else if (choice < min || choice > max) {
            cout << u8"ОШИБКА! Введите число от '" << min << "' до '" << max << "'.\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return choice;
        }
    }
}

string getKeyFromUser(const string& cipherName) {
    string key;
    cout << u8"Введите ключ для " << cipherName << u8" (размер матрицы): ";
    getline(cin, key);
    return key;
}

string generateKeyForCipher(const string& cipherName) {
    void* handle = nullptr;
    GenerateKeyFunc generate_key_ptr = nullptr;

    if (cipherName == "permutation") {
#ifdef _WIN32
        handle = LoadLibraryA("permutation.dll");
#else
        handle = dlopen("./libpermutation.so", RTLD_LAZY);
#endif
        if (handle) {
#ifdef _WIN32
            generate_key_ptr = (GenerateKeyFunc)GetProcAddress((HMODULE)handle, "GeneratePermutationKey");
#else
            generate_key_ptr = (GenerateKeyFunc)dlsym(handle, "GeneratePermutationKey");
#endif
        }
    } else if (cipherName == "magicsquare") {
#ifdef _WIN32
        handle = LoadLibraryA("magicsquare.dll");
#else
        handle = dlopen("./libmagicsquare.so", RTLD_LAZY);
#endif
        if (handle) {
#ifdef _WIN32
            generate_key_ptr = (GenerateKeyFunc)GetProcAddress((HMODULE)handle, "GenerateMagicSquareKey");
#else
            generate_key_ptr = (GenerateKeyFunc)dlsym(handle, "GenerateMagicSquareKey");
#endif
        }
    } else if (cipherName == "matrix") {
#ifdef _WIN32
        handle = LoadLibraryA("matrix.dll");
#else
        handle = dlopen("./libmatrix.so", RTLD_LAZY);
#endif
        if (handle) {
#ifdef _WIN32
            generate_key_ptr = (GenerateKeyFunc)GetProcAddress((HMODULE)handle, "GenerateMatrixKey");
#else
            generate_key_ptr = (GenerateKeyFunc)dlsym(handle, "GenerateMatrixKey");
#endif
        }
    }

    string generatedKey;
    if (generate_key_ptr) {
        generatedKey = generate_key_ptr();
        cout << u8"Сгенерированный ключ: " << generatedKey << endl;
    } else {
        cerr << u8"ОШИБКА! Не удалось сгенерировать ключ.\n";
    }

    if (handle) {
#ifdef _WIN32
        FreeLibrary((HMODULE)handle);
#else
        dlclose(handle);
#endif
    }

    return generatedKey;
}

string getKeyForCipher(const string& cipherName) {
    cout << u8"\nВыберите способ ввода ключа:\n";
    cout << u8"1. Ввести ключ вручную\n";
    cout << u8"2. Сгенерировать ключ автоматически\n";
    
    uint64_t keyChoice = MenuChoice(1, 2);
    
    if (keyChoice == 1) {
        return getKeyFromUser(cipherName);
    } else {
        return generateKeyForCipher(cipherName);
    }
}

void MatrixMenu() {
    void* handle = nullptr;
    MatrixText encrypt_text_ptr = nullptr, decrypt_text_ptr = nullptr;
    MatrixFile encrypt_file_ptr = nullptr, decrypt_file_ptr = nullptr;

#ifdef _WIN32
    handle = LoadLibraryA("matrix.dll");
#else
    handle = dlopen("./libmatrix.so", RTLD_LAZY);
#endif

    if (!handle) {
        cerr << u8"ОШИБКА! Не удалось загрузить библиотеку matrix.\n";
        return;
    }

#ifdef _WIN32
    encrypt_text_ptr = (MatrixText)GetProcAddress((HMODULE)handle, "MatrixTextEncrypt");
    decrypt_text_ptr = (MatrixText)GetProcAddress((HMODULE)handle, "MatrixTextDecrypt");
    encrypt_file_ptr = (MatrixFile)GetProcAddress((HMODULE)handle, "MatrixFileEncrypt");
    decrypt_file_ptr = (MatrixFile)GetProcAddress((HMODULE)handle, "MatrixFileDecrypt");
#else
    encrypt_text_ptr = (MatrixText)dlsym(handle, "MatrixTextEncrypt");
    decrypt_text_ptr = (MatrixText)dlsym(handle, "MatrixTextDecrypt");
    encrypt_file_ptr = (MatrixFile)dlsym(handle, "MatrixFileEncrypt");
    decrypt_file_ptr = (MatrixFile)dlsym(handle, "MatrixFileDecrypt");
#endif

    if (!encrypt_text_ptr || !decrypt_text_ptr || !encrypt_file_ptr || !decrypt_file_ptr) {
        cerr << u8"ОШИБКА! Не удалось найти одну или несколько функций в библиотеке matrix.\n";
#ifdef _WIN32
        FreeLibrary((HMODULE)handle);
#else
        dlclose(handle);
#endif
        return;
    }

    string text, result, inPath, outPath, key;

    cout << u8"====Матричная шифровка====\n";
    cout << u8"1. Зашифровать текст.\n";
    cout << u8"2. Расшифровать текст.\n";
    cout << u8"3. Зашифровать файл.\n";
    cout << u8"4. Расшифровать файл.\n";
    cout << u8"5. В главное меню.\n";

    uint64_t choice = MenuChoice(1, 5);

    if (choice == 1 || choice == 2) {
        cout << u8"Введите текст для его обработки: ";
        getline(cin, text);
        
        key = getKeyForCipher("matrix");
        if (key.empty()) {
#ifdef _WIN32
            FreeLibrary((HMODULE)handle);
#else
            dlclose(handle);
#endif
            return;
        }
        
        try {
            if (choice == 1) {
                result = encrypt_text_ptr(text, key);
                cout << u8"Зашифрованный текст: " << result << endl;
            } else {
                result = decrypt_text_ptr(text, key);
                cout << u8"Расшифрованный текст: " << result << endl;
            }
        } catch (const std::exception& e) {
            cerr << u8"ОШИБКА! " << e.what() << endl;
        }
    } else if (choice == 3 || choice == 4) {
        cout << u8"Введите путь к исходному файлу: ";
        getline(cin, inPath);
        cout << u8"Введите путь к файлу, в который необходимо записать результат: ";
        getline(cin, outPath);
        
        key = getKeyForCipher("matrix");
        if (key.empty()) {
#ifdef _WIN32
            FreeLibrary((HMODULE)handle);
#else
            dlclose(handle);
#endif
            return;
        }
        
        try {
            if (choice == 3) {
                encrypt_file_ptr(inPath, outPath, key);
                cout << u8"Файл зашифрован.\n";
            } else {
                decrypt_file_ptr(inPath, outPath, key);
                cout << u8"Файл расшифрован.\n";
            }
        } catch (const std::exception& e) {
            cerr << u8"ОШИБКА! " << e.what() << endl;
        }
    }

#ifdef _WIN32
    FreeLibrary((HMODULE)handle);
#else
    dlclose(handle);
#endif
}

void MagicSquareMenu() {
    void* handle = nullptr;
    MagicSquareText encrypt_text_ptr = nullptr, decrypt_text_ptr = nullptr;
    MagicSquareFile encrypt_file_ptr = nullptr, decrypt_file_ptr = nullptr;

#ifdef _WIN32
    handle = LoadLibraryA("magicsquare.dll");
#else
    handle = dlopen("./libmagicsquare.so", RTLD_LAZY);
#endif

    if (!handle) {
        cerr << u8"ОШИБКА! Не удалось загрузить библиотеку magicsquare.\n";
        return;
    }

#ifdef _WIN32
    encrypt_text_ptr = (MagicSquareText)GetProcAddress((HMODULE)handle, "MagicSquareTextEncrypt");
    decrypt_text_ptr = (MagicSquareText)GetProcAddress((HMODULE)handle, "MagicSquareTextDecrypt");
    encrypt_file_ptr = (MagicSquareFile)GetProcAddress((HMODULE)handle, "MagicSquareFileEncrypt");
    decrypt_file_ptr = (MagicSquareFile)GetProcAddress((HMODULE)handle, "MagicSquareFileDecrypt");
#else
    encrypt_text_ptr = (MagicSquareText)dlsym(handle, "MagicSquareTextEncrypt");
    decrypt_text_ptr = (MagicSquareText)dlsym(handle, "MagicSquareTextDecrypt");
    encrypt_file_ptr = (MagicSquareFile)dlsym(handle, "MagicSquareFileEncrypt");
    decrypt_file_ptr = (MagicSquareFile)dlsym(handle, "MagicSquareFileDecrypt");
#endif

    if (!encrypt_text_ptr || !decrypt_text_ptr || !encrypt_file_ptr || !decrypt_file_ptr) {
        cerr << u8"ОШИБКА! Не удалось найти одну или несколько функций в библиотеке magicsquare.\n";
#ifdef _WIN32
        FreeLibrary((HMODULE)handle);
#else
        dlclose(handle);
#endif
        return;
    }

    string text, result, inPath, outPath, key;

    cout << u8"====Магический квадрат====\n";
    cout << u8"1. Зашифровать текст.\n";
    cout << u8"2. Расшифровать текст.\n";
    cout << u8"3. Зашифровать файл.\n";
    cout << u8"4. Расшифровать файл.\n";
    cout << u8"5. В главное меню.\n";

    uint64_t choice = MenuChoice(1, 5);

    if (choice == 1 || choice == 2) {
        cout << u8"Введите текст для его обработки: ";
        getline(cin, text);
        
        key = getKeyForCipher("magicsquare");
        if (key.empty()) {
#ifdef _WIN32
            FreeLibrary((HMODULE)handle);
#else
            dlclose(handle);
#endif
            return;
        }
        
        try {
            if (choice == 1) result = encrypt_text_ptr(text, key);
            else result = decrypt_text_ptr(text, key);
            cout << u8"Результат: " << result << endl;
        } catch (const std::exception& e) {
            cerr << u8"ОШИБКА! " << e.what() << endl;
        }
    } else if (choice == 3 || choice == 4) {
        cout << u8"Введите путь к исходному файлу: ";
        getline(cin, inPath);
        cout << u8"Введите путь к файлу, в который необходимо записать результат: ";
        getline(cin, outPath);
        
        key = getKeyForCipher("magicsquare");
        if (key.empty()) {
#ifdef _WIN32
            FreeLibrary((HMODULE)handle);
#else
            dlclose(handle);
#endif
            return;
        }
        
        try {
            if (choice == 3) encrypt_file_ptr(inPath, outPath, key);
            else decrypt_file_ptr(inPath, outPath, key);
            cout << u8"Операция завершена.\n";
        } catch (const std::exception& e) {
            cerr << u8"ОШИБКА! " << e.what() << endl;
        }
    }

#ifdef _WIN32
    FreeLibrary((HMODULE)handle);
#else
    dlclose(handle);
#endif
}

void PermutationMenu() {
    void* handle = nullptr;
    PermutationText encrypt_text_ptr = nullptr, decrypt_text_ptr = nullptr;
    PermutationFile encrypt_file_ptr = nullptr, decrypt_file_ptr = nullptr;

#ifdef _WIN32
    handle = LoadLibraryA("permutation.dll");
#else
    handle = dlopen("./libpermutation.so", RTLD_LAZY);
#endif

    if (!handle) {
        cerr << u8"ОШИБКА! Не удалось загрузить библиотеку permutation.\n";
        return;
    }

#ifdef _WIN32
    encrypt_text_ptr = (PermutationText)GetProcAddress((HMODULE)handle, "PermutationTextEncrypt");
    decrypt_text_ptr = (PermutationText)GetProcAddress((HMODULE)handle, "PermutationTextDecrypt");
    encrypt_file_ptr = (PermutationFile)GetProcAddress((HMODULE)handle, "PermutationFileEncrypt");
    decrypt_file_ptr = (PermutationFile)GetProcAddress((HMODULE)handle, "PermutationFileDecrypt");
#else
    encrypt_text_ptr = (PermutationText)dlsym(handle, "PermutationTextEncrypt");
    decrypt_text_ptr = (PermutationText)dlsym(handle, "PermutationTextDecrypt");
    encrypt_file_ptr = (PermutationFile)dlsym(handle, "PermutationFileEncrypt");
    decrypt_file_ptr = (PermutationFile)dlsym(handle, "PermutationFileDecrypt");
#endif

    if (!encrypt_text_ptr || !decrypt_text_ptr || !encrypt_file_ptr || !decrypt_file_ptr) {
        cerr << u8"ОШИБКА! Не удалось найти одну или несколько функций в библиотеке permutation.\n";
#ifdef _WIN32
        FreeLibrary((HMODULE)handle);
#else
        dlclose(handle);
#endif
        return;
    }

    string text, result, inPath, outPath, key;

    cout << u8"====Шифр Перестановки====\n";
    cout << u8"1. Зашифровать текст.\n";
    cout << u8"2. Расшифровать текст.\n";
    cout << u8"3. Зашифровать файл.\n";
    cout << u8"4. Расшифровать файл.\n";
    cout << u8"5. В главное меню.\n";

    uint64_t choice = MenuChoice(1, 5);

    if (choice == 1 || choice == 2) {
        cout << u8"Введите текст для его обработки: ";
        getline(cin, text);
        
        key = getKeyForCipher("permutation");
        if (key.empty()) {
#ifdef _WIN32
            FreeLibrary((HMODULE)handle);
#else
            dlclose(handle);
#endif
            return;
        }
        
        try {
            if (choice == 1) result = encrypt_text_ptr(text, key);
            else result = decrypt_text_ptr(text, key);
            cout << u8"Результат: " << result << endl;
        } catch (const std::exception& e) {
            cerr << u8"ОШИБКА! " << e.what() << endl;
        }
    } else if (choice == 3 || choice == 4) {
        cout << u8"Введите путь к исходному файлу: ";
        getline(cin, inPath);
        cout << u8"Введите путь к файлу, в который необходимо записать результат: ";
        getline(cin, outPath);
        
        key = getKeyForCipher("permutation");
        if (key.empty()) {
#ifdef _WIN32
            FreeLibrary((HMODULE)handle);
#else
            dlclose(handle);
#endif
            return;
        }
        
        try {
            if (choice == 3) encrypt_file_ptr(inPath, outPath, key);
            else decrypt_file_ptr(inPath, outPath, key);
            cout << u8"Операция завершена.\n";
        } catch (const std::exception& e) {
            cerr << u8"ОШИБКА! " << e.what() << endl;
        }
    }

#ifdef _WIN32
    FreeLibrary((HMODULE)handle);
#else
    dlclose(handle);
#endif
}

void GenerateKeyMenu() {
    cout << u8"====Генерация ключей====\n";
    cout << u8"1. Сгенерировать ключ для перестановки\n";
    cout << u8"2. Сгенерировать ключ для магического квадрата\n";
    cout << u8"3. Сгенерировать ключ для матричной шифровки\n";
    cout << u8"4. Назад в главное меню\n";
    
    uint64_t choice = MenuChoice(1, 4);
    
    if (choice == 1) {
        generateKeyForCipher("permutation");
    } else if (choice == 2) {
        generateKeyForCipher("magicsquare");
    } else if (choice == 3) {
        generateKeyForCipher("matrix");
    }
}

int main() {
    WindowsConsole();
    std::srand(std::time(0)); // Инициализация генератора случайных чисел

    while (true) {
        cout << u8"====ПРОГРАММА ШИФРОВАНИЯ/ДЕШИФРОВАНИЯ====\n";
        cout << u8"Выберите метод шифрования или действие:\n";
        cout << u8"1. Перестановка\n";
        cout << u8"2. Матричная шифровка\n";
        cout << u8"3. Магический квадрат\n";
        cout << u8"4. Сгенерировать ключ\n";
        cout << u8"5. Выход.\n";

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
                GenerateKeyMenu();
                break;
            case 5:
                cout << u8"Выход из программы.\n";
                return 0;
        }
    }

    return 0;
}
