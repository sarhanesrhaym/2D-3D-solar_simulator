// ===== GUIDE D'INTERACTION POUR SUIVRE LES PLANÈTES =====
// Ce fichier contient les fonctions pour afficher les contrôles intuitifs
// permettant aux utilisateurs de suivre les planètes (touches 1-8)

#include "UI.h"
#include <sstream>

// ===== PANNEAU PRINCIPAL - SÉLECTION DES PLANÈTES =====

/**
 * @brief Affiche un panneau intuitif pour sélectionner quelle planète suivre
 * Utilise les numéros 1-8 et les noms des planètes avec des icônes visuelles
 */
void drawPlanetFollowGuide(sf::RenderWindow& window, const sf::Font& font,
    const std::vector<Planet>& planets, int followedPlanetIndex, float time) {
    
    // ? NOUVEAU : Données des planètes avec leurs symboles
    struct PlanetSymbol {
   std::string number;      // "1", "2", "3"...
        std::string name;        // "Mercure", "Vénus"...
        sf::Color color;         // Couleur de la planète
        std::string shortName;   // "MER", "VEN" pour mobile
    };
    
    std::vector<PlanetSymbol> symbolData = {
        {"1", "Mercure", sf::Color(169, 169, 169), "MER"},
        {"2", "Vénus", sf::Color(255, 198, 73), "VEN"},
        {"3", "Terre", sf::Color(100, 149, 237), "TER"},
        {"4", "Mars", sf::Color(205, 92, 92), "MAR"},
    {"5", "Jupiter", sf::Color(218, 165, 32), "JUP"},
        {"6", "Saturne", sf::Color(238, 232, 170), "SAT"},
        {"7", "Uranus", sf::Color(175, 238, 238), "URA"},
        {"8", "Neptune", sf::Color(65, 105, 225), "NEP"}
  };
    
    // ===== PANNEAU PRINCIPAL =====
    float panelX = 20.f;
    float panelY = 100.f;
    float panelW = 280.f;
    float panelH = 300.f;
    
    // Fond du panneau avec style futuriste
    sf::RectangleShape panel(sf::Vector2f(panelW, panelH));
    panel.setPosition(panelX, panelY);
    panel.setFillColor(sf::Color(10, 20, 40, 240));
    panel.setOutlineThickness(3.f);
    panel.setOutlineColor(sf::Color(0, 200, 255, 200));
    window.draw(panel);
    
    // Ligne supérieure lumineuse
    sf::RectangleShape topLine(sf::Vector2f(panelW, 3.f));
    topLine.setPosition(panelX, panelY);
    topLine.setFillColor(sf::Color(0, 255, 255, 255));
    window.draw(topLine);
    
    // Titre du panneau
    sf::Text titleText("SUIVRE PLANETE", font, 14);
    titleText.setPosition(panelX + panelW / 2.f - 50.f, panelY + 8.f);
    titleText.setFillColor(sf::Color(100, 255, 255));
    titleText.setStyle(sf::Text::Bold);
    window.draw(titleText);
    
    // Sous-titre "Appuyez sur 1-8"
    sf::Text helpText("Appuyez 1-8", font, 12);
    helpText.setPosition(panelX + 10.f, panelY + 30.f);
    helpText.setFillColor(sf::Color(150, 200, 255));
    window.draw(helpText);
    
    // ===== GRILLE DES PLANÈTES (2x4) =====
    float cellW = (panelW - 20.f) / 2.f;
    float cellH = 32.f;
    float cellSpacingX = 5.f;
    float cellSpacingY = 3.f;
    float startX = panelX + 10.f;
    float startY = panelY + 50.f;
    
  for (size_t i = 0; i < symbolData.size(); ++i) {
        if (i >= planets.size()) break;
      
        // Position dans la grille (2 colonnes, 4 lignes)
        int row = i / 2;
        int col = i % 2;
        float x = startX + col * (cellW + cellSpacingX);
        float y = startY + row * (cellH + cellSpacingY);
        
        // Déterminer si cette planète est suivie
        bool isFollowed = (static_cast<int>(i) == followedPlanetIndex);
   
        // ===== FOND DE LA CELLULE =====
 sf::RectangleShape cellBg(sf::Vector2f(cellW, cellH));
      cellBg.setPosition(x, y);
   
        if (isFollowed) {
            // Couleur lumineuse si suivi
            float pulse = 0.7f + 0.3f * std::sin(time * 5.f);
            sf::Uint8 glowAlpha = static_cast<sf::Uint8>(150 + 100 * pulse);
        cellBg.setFillColor(sf::Color(symbolData[i].color.r, 
       symbolData[i].color.g,
           symbolData[i].color.b, 
        glowAlpha));
            cellBg.setOutlineThickness(2.f);
 cellBg.setOutlineColor(sf::Color(255, 255, 255, 255));
        } else {
  cellBg.setFillColor(sf::Color(30, 50, 80, 150));
          cellBg.setOutlineThickness(1.f);
            cellBg.setOutlineColor(symbolData[i].color);
  }
        window.draw(cellBg);
        
        // ===== NUMÉRO DE TOUCHE (GAUCHE) =====
 sf::Text numText(symbolData[i].number, font, 14);
 numText.setPosition(x + 4.f, y + 5.f);
    numText.setFillColor(isFollowed ? sf::Color::White : sf::Color(150, 150, 150));
        numText.setStyle(sf::Text::Bold);
   window.draw(numText);
     
        // ===== NOM DE LA PLANÈTE (DROITE) =====
        sf::Text nameText(symbolData[i].shortName, font, 11);
        nameText.setPosition(x + cellW - 30.f, y + 8.f);
        nameText.setFillColor(isFollowed ? sf::Color::White : sf::Color(180, 200, 220));
        nameText.setStyle(sf::Text::Bold);
        window.draw(nameText);
      
      // Icône de suivi animée (si suivi)
        if (isFollowed) {
            float pulse = 0.5f + 0.5f * std::sin(time * 6.f);
        sf::CircleShape indicator(3.f * pulse);
            indicator.setOrigin(indicator.getRadius(), indicator.getRadius());
            indicator.setPosition(x + cellW / 2.f, y + cellH - 5.f);
        indicator.setFillColor(sf::Color(0, 255, 100, 200));
            window.draw(indicator);
        }
    }
    
    // ===== LÉGENDE EN BAS =====
    sf::Text legendText("0 = Centre", font, 10);
    legendText.setPosition(panelX + 10.f, panelY + panelH - 20.f);
    legendText.setFillColor(sf::Color(100, 150, 200));
    window.draw(legendText);
}

// ===== PANNEAU RAPIDE - RACCOURCIS CLAVIER =====

/**
 * @brief Affiche un petit panneau des raccourcis clavier en simulation
 * Placé dans le coin supérieur droit pour informer les utilisateurs
 */
void drawKeyboardHints(sf::RenderWindow& window, const sf::Font& font, float time) {
    
    float panelX = window.getSize().x - 220.f;
    float panelY = 20.f;
    float panelW = 200.f;
    float panelH = 120.f;
 
    // Fond semi-transparent
    sf::RectangleShape hintPanel(sf::Vector2f(panelW, panelH));
    hintPanel.setPosition(panelX, panelY);
    hintPanel.setFillColor(sf::Color(10, 15, 35, 220));
    hintPanel.setOutlineThickness(2.f);
    
    // Couleur de l'outline qui pulse
    float outlineAlpha = 150.f + 100.f * std::sin(time * 2.f);
    hintPanel.setOutlineColor(sf::Color(100, 200, 255, static_cast<sf::Uint8>(outlineAlpha)));
    window.draw(hintPanel);
    
    // Titre
    sf::Text titleHint("RACCOURCIS", font, 12);
  titleHint.setPosition(panelX + 15.f, panelY + 5.f);
    titleHint.setFillColor(sf::Color(100, 255, 200));
    titleHint.setStyle(sf::Text::Bold);
    window.draw(titleHint);
    
    // Aide rapide
    sf::Text hint1("1-8: Suivre", font, 10);
    hint1.setPosition(panelX + 10.f, panelY + 25.f);
    hint1.setFillColor(sf::Color(200, 220, 255));
    window.draw(hint1);
    
    sf::Text hint2("0: Vue sol.", font, 10);
    hint2.setPosition(panelX + 10.f, panelY + 40.f);
    hint2.setFillColor(sf::Color(200, 220, 255));
    window.draw(hint2);
    
    sf::Text hint3("F: Mode suivi", font, 10);
    hint3.setPosition(panelX + 10.f, panelY + 55.f);
    hint3.setFillColor(sf::Color(200, 220, 255));
    window.draw(hint3);
    
  sf::Text hint4("TAB: Menu", font, 10);
    hint4.setPosition(panelX + 10.f, panelY + 70.f);
    hint4.setFillColor(sf::Color(200, 220, 255));
    window.draw(hint4);
    
    sf::Text hint5("R: Reset", font, 10);
    hint5.setPosition(panelX + 10.f, panelY + 85.f);
    hint5.setFillColor(sf::Color(200, 220, 255));
    window.draw(hint5);
}

// ===== INDICATEUR VISUEL DES TOUCHES DISPONIBLES =====

/**
 * @brief Affiche un indicateur visuel montrant les touches numérotées disponibles
 * lors du survol avec la souris sur les planètes en simulation
 */
void drawNumberKeyIndicator(sf::RenderWindow& window, const sf::Font& font,
    const std::vector<Planet>& planets, const sf::View& worldView, const sf::View& uiView) {
    
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel, worldView);
    
    // Vérifier si la souris est sur une planète
    for (size_t i = 0; i < planets.size(); ++i) {
        if (planets[i].shape.getGlobalBounds().contains(mouseWorld)) {
   // Afficher un petit badge avec le numéro
        sf::Vector2f uiMousePos = window.mapPixelToCoords(mousePixel, uiView);
  
            sf::RectangleShape badge(sf::Vector2f(40.f, 40.f));
         badge.setPosition(uiMousePos.x + 10.f, uiMousePos.y + 10.f);
     badge.setFillColor(planets[i].color);
            badge.setOutlineThickness(2.f);
badge.setOutlineColor(sf::Color::White);
            window.draw(badge);
            
      // Numéro
    sf::Text numText(std::to_string(i + 1), font, 24);
      numText.setPosition(uiMousePos.x + 20.f, uiMousePos.y + 15.f);
    numText.setFillColor(sf::Color::White);
            numText.setStyle(sf::Text::Bold);
     window.draw(numText);
       
   // Texte "Touche X"
       sf::Text keyText("Touche " + std::to_string(i + 1), font, 10);
          keyText.setPosition(uiMousePos.x + 55.f, uiMousePos.y + 12.f);
     keyText.setFillColor(sf::Color::White);
   window.draw(keyText);
        
    break;
  }
    }
}
