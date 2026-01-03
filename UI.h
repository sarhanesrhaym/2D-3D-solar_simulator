#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "CosmicButton.h"
#include "Star.h"
#include "MenuPlanet.h"
#include "Planet.h"

// Fonctions pour créer les éléments du menu
void createStarfield(std::vector<Star>& stars);
void createMenuSolarSystem(std::vector<MenuPlanet>& menuPlanets, const sf::Vector2f& center);

// Fonctions de mise à jour
void updateStars(std::vector<Star>& stars, float time);
void updateMenuSolarSystem(std::vector<MenuPlanet>& menuPlanets, float deltaTime, const sf::Vector2f& center);

// Fonctions de dessin des différents écrans
void drawMainMenu(sf::RenderWindow& window, const sf::Font& font,
    std::vector<CosmicButton>& menuButtons,
    std::vector<Star>& stars,
    std::vector<MenuPlanet>& menuPlanets,
    const sf::Vector2f& center);

void drawPresentation(sf::RenderWindow& window, const sf::Font& font,
    std::vector<CosmicButton>& backButtons,
    std::vector<Star>& stars, float time);

void drawTeamInfo(sf::RenderWindow& window, const sf::Font& font,
    std::vector<CosmicButton>& backButtons,
    std::vector<Star>& stars, float time);

void drawPlanetDetails(sf::RenderWindow& window, const sf::Font& font,
    std::vector<CosmicButton>& backButtons,
    std::vector<Star>& stars, float time,
    const Planet& planet);

void drawSimulation(sf::RenderWindow& window, const sf::Font& font,
    sf::View& worldView, sf::View& uiView,
    std::vector<Star>& stars, float time,
    const sf::CircleShape& sun, const sf::CircleShape& sunGlow,
    std::vector<Planet>& planets,
    std::vector<class Comet>& comets,
    std::vector<CosmicButton>& simButtons,
    const sf::Text& infoText, const sf::Text& speedText,
    bool showOrbits, bool showTrails, bool showLabels,
    bool showStats, bool showGrid,
    const sf::RectangleShape& statsPanel,
    const sf::Text& statsTitle,
    const sf::Texture& saturnRingTexture);
// ===== NOUVELLES DÉCLARATIONS POUR LE FOND FUTURISTE =====

/**
 * @brief Structure contenant la palette de couleurs futuriste
 */
struct FuturisticPalette {
    sf::Color deepPurple;      // #6a11cb
    sf::Color electricBlue;    // #2575fc
    sf::Color luminousPink;    // #ff5e62
    sf::Color solarYellow;     // #ffd166
    sf::Color crispWhite;      // #f8f9fa

    FuturisticPalette();
};

// Fonctions pour générer et afficher le fond futuriste
void initializeFuturisticBackground(sf::RenderWindow& window);
void drawFuturisticBackground(sf::RenderWindow& window, float phase);
// Ajouter cette déclaration avec les autres fonctions
void drawPlanetStructure(sf::RenderWindow& window, const sf::Font& font,
    std::vector<CosmicButton>& backButtons,
    std::vector<Star>& stars, float time);