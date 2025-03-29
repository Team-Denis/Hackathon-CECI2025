# Nom de l'exécutable
TARGET = Hackathon-CECI2025

# Compilateur
CXX = g++

# Options de compilation
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

# Répertoires
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include

# Fichiers sources et objets
SRCS = $(wildcard $(SRC_DIR)/**/*.cpp) $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# Règle par défaut
all: $(TARGET) clean_objs

# Création de l'exécutable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compilation des fichiers objets
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

clean_objs:
	rm -rf $(BUILD_DIR)/*.o

# Phony targets
.PHONY: all clean