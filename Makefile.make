# Makefile pour CosmoUIT

# Compilateur
CXX = g++

# Options de compilation
CXXFLAGS = -std=c++17 -Wall -Iinclude

# Bibliothèques SFML
LIBS = -lsfml-graphics -lsfml-window -lsfml-system

# Nom de l'exécutable
TARGET = CosmoUIT

# Dossiers
SRC_DIR = src
OBJ_DIR = obj

# Fichiers sources
SOURCES = $(SRC_DIR)/main.cpp \
          $(SRC_DIR)/Planet.cpp \
          $(SRC_DIR)/Comet.cpp \
          $(SRC_DIR)/Star.cpp \
          $(SRC_DIR)/MenuPlanet.cpp \
          $(SRC_DIR)/CosmicButton.cpp \
          $(SRC_DIR)/UI.cpp

# Fichiers objets
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Règle par défaut
all: $(TARGET)

# Créer l'exécutable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LIBS)
	@echo "Compilation terminée : $(TARGET)"

# Compiler les fichiers sources en objets
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyer les fichiers de compilation
clean:
	rm -rf $(OBJ_DIR) $(TARGET)
	@echo "Fichiers de compilation supprimés"

# Recompiler complètement
rebuild: clean all

# Exécuter le programme
run: $(TARGET)
	./$(TARGET)

# Aide
help:
	@echo "Commandes disponibles :"
	@echo "  make          - Compiler le projet"
	@echo "  make clean    - Supprimer les fichiers de compilation"
	@echo "  make rebuild  - Recompiler complètement"
	@echo "  make run      - Compiler et exécuter"
	@echo "  make help     - Afficher cette aide"

.PHONY: all clean rebuild run help