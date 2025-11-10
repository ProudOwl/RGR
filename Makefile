# Настройки компилятора
CXX = g++
CXXFLAGS = -Wall -std=c++17 -fPIC
LDFLAGS = 

# Платформо-зависимые настройки
ifeq ($(OS),Windows_NT)
    LIB_EXT = .dll
    EXE_EXT = .exe
    LDFLAGS += -static-libgcc -static-libstdc++
else
    LIB_EXT = .so
    EXE_EXT = 
    LDFLAGS += -ldl
endif

# Каталоги
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
LIB_DIR = $(BUILD_DIR)/lib
BIN_DIR = $(BUILD_DIR)/bin

# Цели
TARGET = $(BIN_DIR)/cryptography$(EXE_EXT)
TARGET_LINK = ./cryptography$(EXE_EXT)
LIBS = $(LIB_DIR)/libpermutation$(LIB_EXT) $(LIB_DIR)/libmatrix$(LIB_EXT) $(LIB_DIR)/libmagicsquare$(LIB_EXT)

# Основная цель
all: prepare $(TARGET) $(LIBS) create_link
	@echo "========================================"
	@echo "Сборка завершена успешно!"
	@echo "Исполняемый файл: $(TARGET)"
	@echo "Для запуска: ./cryptography"
	@echo "========================================"

# Подготовка окружения
.PHONY: prepare
prepare:
	@mkdir -p $(BUILD_DIR) $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR)
	@echo "Созданы директории для сборки"

# Создание ссылки на исполняемый файл
.PHONY: create_link
create_link: $(TARGET)
	@echo "Создание ссылки для запуска..."
ifeq ($(OS),Windows_NT)
	@cp $(TARGET) $(TARGET_LINK)
	@echo "Создана копия: cryptography.exe"
else
	@ln -sf $(TARGET) $(TARGET_LINK)
	@echo "Создана символическая ссылка: ./cryptography"
endif

# Сборка основной программы
$(TARGET): $(OBJ_DIR)/cryptography.o
	@echo "Сборка основной программы..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Объектные файлы основной программы
$(OBJ_DIR)/cryptography.o: cryptography.cpp
	@echo "Компиляция cryptography.cpp..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Библиотека перестановки
$(LIB_DIR)/libpermutation$(LIB_EXT): permutation.cpp permutation.h
	@echo "Сборка библиотеки перестановки..."
	$(CXX) $(CXXFLAGS) -shared -o $@ $<

# Библиотека матричной шифровки
$(LIB_DIR)/libmatrix$(LIB_EXT): matrix.cpp matrix.h
	@echo "Сборка библиотеки матричной шифровки..."
	$(CXX) $(CXXFLAGS) -shared -o $@ $<

# Библиотека магического квадрата
$(LIB_DIR)/libmagicsquare$(LIB_EXT): magicsquare.cpp magicsquare.h
	@echo "Сборка библиотеки магического квадрата..."
	$(CXX) $(CXXFLAGS) -shared -o $@ $<

# Отладочная сборка
debug: CXXFLAGS += -g -DDEBUG
debug: all
	@echo "Собрана отладочная версия"

# Быстрая сборка (без оптимизаций)
fast: CXXFLAGS += -O0
fast: all
	@echo "Собрана быстрая версия (без оптимизаций)"

# Оптимизированная сборка
release: CXXFLAGS += -O3 -DNDEBUG
release: all
	@echo "Собрана оптимизированная версия"

# Показать информацию о собранных файлах
.PHONY: info
info:
	@echo "=== Информация о сборке ==="
	@echo "Платформа: $(if $(filter Windows_NT,$(OS)),Windows,Linux)"
	@echo "Исполняемый файл: $(TARGET)"
	@echo "Ссылка для запуска: $(TARGET_LINK)"
	@echo "Библиотеки:"
	@-ls -la $(LIB_DIR)/ 2>/dev/null || echo "Библиотеки не найдены"
	@echo "========================"

# Запуск программы
.PHONY: run
run: create_link
	@echo "Запуск программы..."
	./cryptography

# Очистка
.PHONY: clean
clean:
	@echo "Очистка проекта..."
	rm -rf $(BUILD_DIR)
	rm -f ./cryptography ./cryptography.exe
	@echo "Все сгенерированные файлы удалены"

# Помощь
.PHONY: help
help:
	@echo "Доступные команды:"
	@echo "  make all     - Полная сборка проекта"
	@echo "  make release - Оптимизированная сборка"
	@echo "  make debug   - Отладочная сборка"
	@echo "  make fast    - Быстрая сборка (без оптимизаций)"
	@echo "  make run     - Сборка и запуск программы"
	@echo "  make info    - Информация о собранных файлах"
	@echo "  make clean   - Очистка проекта"
	@echo "  make help    - Показать эту справку"

.DEFAULT_GOAL := all
