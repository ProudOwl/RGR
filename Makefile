# Настройки компилятора
CXX = g++
CXXFLAGS = -Wall -std=c++17 -fPIC
LDFLAGS = 

LIB_EXT = .so
LDFLAGS += -ldl

# Каталоги
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
LIB_DIR = $(BUILD_DIR)/lib
BIN_DIR = $(BUILD_DIR)/bin

# Цели
TARGET = $(BIN_DIR)/main
TARGET_LINK = cryptography
LIBS = $(LIB_DIR)/libpermutation$(LIB_EXT) $(LIB_DIR)/libmatrix$(LIB_EXT) $(LIB_DIR)/libmagicsquare$(LIB_EXT)

# Основная цель
all: prepare $(TARGET) $(LIBS) create_link
	@echo "========================================"
	@echo "Сборка завершена успешно!"
	@echo "Исполняемый файл: $(TARGET)"
	@echo "Для запуска: ./$(TARGET_LINK)"
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
	@ln -sf $(TARGET) $(TARGET_LINK)
	@echo "Создана символическая ссылка: ./$(TARGET_LINK)"

# Сборка основной программы
$(TARGET): $(OBJ_DIR)/cryptography.o
	@echo "Сборка основной программы..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Объектные файлы основной программы
$(OBJ_DIR)/cryptography.o: main.cpp
	@echo "Компиляция main.cpp..."
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

# Показать информацию о собранных файлах
.PHONY: info
info:
	@echo "=== Информация о сборке ==="
	@echo "Исполняемый файл: $(TARGET)"
	@echo "Ссылка для запуска: $(TARGET_LINK)"
	@echo "Библиотеки:"
	@-ls -la $(LIB_DIR)/ 2>/dev/null || echo "Библиотеки не найдены"
	@echo "========================"

# Очистка
.PHONY: clean
clean:
	@echo "Очистка проекта..."
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET_LINK)
	@echo "Все сгенерированные файлы удалены"

# Помощь
.PHONY: help
help:
	@echo "Доступные команды:"
	@echo "  make all     - Полная сборка проекта"
	@echo "  make clean   - Очистка проекта"
	@echo "  make info    - Показать информацию о сборке"
	@echo "  make help    - Показать эту справку"

.DEFAULT_GOAL := all
