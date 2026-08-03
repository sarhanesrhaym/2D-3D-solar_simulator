#pragma once
#include <SFML/Graphics.hpp>

class Comet {
public:
    // Constructeur
    Comet(const sf::Vector2f& position);

    // Méthodes
    void update(float deltaTime, const sf::Vector2f& sunCenter, float G, float massSun, bool showTrails);
    void draw(sf::RenderWindow& window, bool showTrails);
    bool isActive() const { return active; }

    // Membres publics
    sf::CircleShape shape;
    sf::VertexArray trail;
    sf::Vector2f velocity;
    sf::Vector2f position;
    float lifetime;
    bool active;
};

