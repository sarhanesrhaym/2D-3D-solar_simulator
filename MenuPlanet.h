#pragma once
#include <SFML/Graphics.hpp>

class MenuPlanet {
public:
    MenuPlanet(float semiMajorAxis, sf::Color color, const sf::Vector2f& center);
    void update(float deltaTime, const sf::Vector2f& center);
    void draw(sf::RenderWindow& window);

private:
    sf::CircleShape shape;
    sf::Vector2f position;
    sf::Vector2f velocity;
    float angle;
    float angularSpeed;
    float semiMajorAxis;
    float eccentricity;
};

