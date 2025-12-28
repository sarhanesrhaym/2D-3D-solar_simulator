#include "MenuPlanet.h"
#include <cmath>
#include <cstdlib>

MenuPlanet::MenuPlanet(float semiMajorAxis, sf::Color color, const sf::Vector2f& center) {
    this->semiMajorAxis = semiMajorAxis;
    this->eccentricity = 0.1f + (std::rand() % 20) / 100.f;
    this->angle = (std::rand() % 360) * 3.14159f / 180.f;

    const float G = 1e-3f;
    const float massSun = 1e6f;
    this->angularSpeed = std::sqrt(G * massSun / (semiMajorAxis * semiMajorAxis * semiMajorAxis)) * 0.3f;

    float radius = 5.f + std::log2(semiMajorAxis / 60.f) * 2.f;
    shape = sf::CircleShape(radius);
    shape.setOrigin(radius, radius);
    shape.setFillColor(color);

    // Position initiale
    float r = semiMajorAxis * (1 - eccentricity * eccentricity) / (1 + eccentricity * std::cos(angle));
    position = sf::Vector2f(center.x + r * std::cos(angle), center.y + r * std::sin(angle));
    shape.setPosition(position);
}

void MenuPlanet::update(float deltaTime, const sf::Vector2f& center) {
    angle += angularSpeed * deltaTime;
    float r = semiMajorAxis * (1 - eccentricity * eccentricity) / (1 + eccentricity * std::cos(angle));
    position = sf::Vector2f(center.x + r * std::cos(angle), center.y + r * std::sin(angle));
    shape.setPosition(position);
}

void MenuPlanet::draw(sf::RenderWindow& window) {
    window.draw(shape);
}