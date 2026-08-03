#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <sstream>
#include "Planet.h"

/**
 * @brief Classe pour gérer l'affichage du panneau d'aide du suivi planétaire
 */
class PlanetFollowHelper {
public:
    PlanetFollowHelper();
    
    void show();
    void hide();
    void toggle();
    bool isVisible() const;
    
    void draw(sf::RenderWindow& window, const sf::Font& font, 
           const std::vector<Planet>& planets, float time);
    
    bool handleClick(const sf::Vector2f& mousePos);
  
private:
    bool visible;
    float animationTime;
};
