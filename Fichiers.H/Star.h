#pragma once
#include <SFML/Graphics.hpp>

class Star {
public:
    Star();
    void update(float time);
    void draw(sf::RenderWindow& window);

private:
    sf::CircleShape shape;
    float twinkleSpeed;
    float baseAlpha;
};

