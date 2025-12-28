#pragma once
#include <SFML/Graphics.hpp>
#include <string>

// Constantes physiques et graphiques
namespace Constants {
    // Physique
    const sf::Vector2f SUN_CENTER(0.f, 0.f);
    const float G = 1e-3f;
    const float MASS_SUN = 1e6f;
    const float PAN_SPEED = 400.f;

    // Interface utilisateur
    const float BTN_Y = 730.f;
    const float BTN_Y2 = 670.f;
    const float BTN_WIDTH = 140.f;
    const float BTN_HEIGHT = 50.f;
    const float BTN_SPACING = 10.f;

    // Menu
    const sf::Vector2f MENU_CENTER(600.f, 400.f);
    const float MENU_BTN_WIDTH = 300.f;
    const float MENU_BTN_HEIGHT = 60.f;
    const float MENU_BTN_SPACING = 80.f;

    // Chemins des textures
    const std::string FONT_PATH = "C:\\Windows\\Fonts\\arial.ttf";
    const std::string TEXTURE_PATH = "textures/";

    // Nouvelles constantes pour les améliorations visuelles
    const sf::Color NEBULA_COLOR_1(138, 43, 226, 25);  // Violet
    const sf::Color NEBULA_COLOR_2(30, 144, 255, 20);  // Bleu
    const sf::Vector2f NEBULA_1_POS(100.f, 50.f);
    const sf::Vector2f NEBULA_2_POS(700.f, 450.f);
}