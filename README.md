# CosmoUIT - Simulateur de Système Solaire 2D

Projet universitaire développé à l'Université Ibn Tofail - Faculté des Sciences

## 📁 Structure du Projet

```
CosmoUIT/
├── include/
│   ├── AppState.h          # États de l'application
│   ├── Constants.h         # Constantes physiques et graphiques
│   ├── Planet.h            # Classe Planet
│   ├── Comet.h             # Classe Comet
│   ├── Star.h              # Classe Star (étoiles du fond)
│   ├── MenuPlanet.h        # Planètes du menu principal
│   ├── CosmicButton.h      # Boutons interactifs
│   └── UI.h                # Fonctions d'interface utilisateur
├── src/
│   ├── main.cpp            # Point d'entrée du programme
│   ├── Planet.cpp          # Implémentation de Planet
│   ├── Comet.cpp           # Implémentation de Comet
│   ├── Star.cpp            # Implémentation de Star
│   ├── MenuPlanet.cpp      # Implémentation de MenuPlanet
│   ├── CosmicButton.cpp    # Implémentation de CosmicButton
│   └── UI.cpp              # Implémentation des fonctions UI
├── textures/
│   ├── mercury.jpg
│   ├── venus.jpg
│   ├── earth.jpg
│   ├── mars.jpg
│   ├── jupiter.jpg
│   ├── saturn.jpg
│   ├── uranus.jpg
│   ├── neptune.jpg
│   ├── sun.jpg
│   └── saturn_ring.png
├── CMakeLists.txt          # Configuration CMake
└── README.md               # Ce fichier

```

## 🔧 Prérequis

- **C++17** ou supérieur
- **SFML 2.5+** (Simple and Fast Multimedia Library)
- **CMake 3.10+** (optionnel, pour la compilation)
- Un compilateur C++ (GCC, Clang, MSVC)

## 📥 Installation de SFML

### Windows
```bash
# Télécharger depuis https://www.sfml-dev.org/download.php
# Ou avec vcpkg:
vcpkg install sfml
```

### Linux (Ubuntu/Debian)
```bash
sudo apt-get install libsfml-dev
```

### macOS
```bash
brew install sfml
```

## 🚀 Compilation

### Méthode 1 : Avec CMake (Recommandé)

```bash
# Créer un dossier de build
mkdir build
cd build

# Configurer CMake
cmake ..

# Compiler
cmake --build .

# Exécuter
./CosmoUIT
```

### Méthode 2 : Avec g++ directement

```bash
# Compiler tous les fichiers
g++ -std=c++17 -Iinclude \
    src/main.cpp \
    src/Planet.cpp \
    src/Comet.cpp \
    src/Star.cpp \
    src/MenuPlanet.cpp \
    src/CosmicButton.cpp \
    src/UI.cpp \
    -o CosmoUIT \
    -lsfml-graphics -lsfml-window -lsfml-system

# Exécuter
./CosmoUIT
```

### Méthode 3 : Visual Studio (Windows)

1. Créer un nouveau projet C++ "Application console"
2. Ajouter tous les fichiers `.h` et `.cpp`
3. Configurer les propriétés du projet :
   - **C/C++ → Général → Répertoires Include supplémentaires** : Ajouter le chemin vers SFML/include
   - **Éditeur de liens → Général → Répertoires de bibliothèques supplémentaires** : Ajouter SFML/lib
   - **Éditeur de liens → Entrée → Dépendances supplémentaires** : Ajouter `sfml-graphics.lib sfml-window.lib sfml-system.lib`
4. Compiler et exécuter

## 🎮 Contrôles

### Menu Principal
- Clic sur les boutons pour naviguer

### Simulation
- **Flèches directionnelles** : Déplacer la caméra
- **Molette souris** : Zoom in/out
- **Clic gauche sur planète** : Afficher les détails
- **Clic gauche dans l'espace** : Créer une comète
- **Espace** : Pause/Reprendre
- **R** : Réinitialiser la caméra
- **Échap** : Retour au menu principal

### Boutons Interface
- **>> / <<** : Accélérer/Ralentir le temps
- **|| ** : Pause
- **+ / -** : Zoom
- **O** : Afficher/Masquer les orbites
- **~** : Afficher/Masquer les traînées
- **T** : Afficher/Masquer les labels
- **i** : Afficher/Masquer les statistiques
- **#** : Afficher/Masquer la grille
- **C** : Effacer les traînées
- **R** : Reset caméra
- **M** : Retour au menu

## ✨ Fonctionnalités

- ✅ Simulation réaliste des orbites planétaires basée sur la mécanique céleste
- ✅ 8 planètes du système solaire avec leurs caractéristiques réelles
- ✅ Système de comètes interactif avec attraction gravitationnelle
- ✅ Visualisation des trajectoires orbitales
- ✅ Zoom et déplacement libre de la caméra
- ✅ Contrôle de la vitesse de simulation (x0.125 à x64)
- ✅ Informations détaillées pour chaque planète
- ✅ Interface utilisateur intuitive avec fond étoilé animé
- ✅ Anneaux de Saturne avec texture
- ✅ Panneau de statistiques

## 📚 Architecture du Code

### Classes Principales

- **Planet** : Gère les propriétés physiques et graphiques d'une planète
- **Comet** : Représente une comète avec physique gravitationnelle
- **Star** : Étoiles du fond avec effet de scintillement
- **MenuPlanet** : Planètes simplifiées pour le menu principal
- **CosmicButton** : Boutons interactifs avec effets hover

### Namespaces

- **Constants** : Contient toutes les constantes physiques et graphiques

### Fonctions UI (UI.cpp)

- `createStarfield()` : Génère le champ d'étoiles
- `createMenuSolarSystem()` : Crée le système solaire du menu
- `drawMainMenu()` : Affiche le menu principal
- `drawSimulation()` : Affiche la simulation
- `drawPlanetDetails()` : Affiche les détails d'une planète
- `drawPresentation()` : Affiche la présentation du projet
- `drawTeamInfo()` : Affiche les informations de l'équipe

## 🐛 Dépannage

### Erreur : "Police non trouvée"
Modifiez le chemin dans `Constants.h` :
```cpp
const std::string FONT_PATH = "C:\\Windows\\Fonts\\arial.ttf"; // Windows
// ou
const std::string FONT_PATH = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"; // Linux
```

### Erreur : "Textures manquantes"
Assurez-vous que le dossier `textures/` est présent et contient toutes les images nécessaires.

### Erreur de compilation SFML
Vérifiez que SFML est correctement installé et que les chemins sont configurés.

## 📝 Licence

Projet académique - Université Ibn Tofail - 2024-2025

## 👥 Équipe

- [Nom Étudiant 1]
- [Nom Étudiant 2]
- [Nom Étudiant 3]
- [Nom Étudiant 4]

**Encadrant :** [Nom du Professeur]

## 🙏 Remerciements

- Université Ibn Tofail - Faculté des Sciences
- SFML Community
- NASA pour les données planétaires