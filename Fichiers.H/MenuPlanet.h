#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class MenuPlanet {
public:
    MenuPlanet(float semiMajorAxis, sf::Color color, const sf::Vector2f& center,
        const std::string& name = "", float realRadius = 1.f, float density = 1.f);

    void update(float deltaTime, const sf::Vector2f& center);
    void draw(sf::RenderWindow& window);
    void drawOrbit(sf::RenderWindow& window, const sf::Vector2f& center);

    // Getters existants
    const std::string& getName() const;
    float getSemiMajorAxis() const;
    sf::Vector2f getPosition() const;

    //  Getters pour l'interaction avec les planètes
    float getRadius() const { return shape.getRadius(); }
    sf::Vector2f getScale() const { return shape.getScale(); }
    // Dans la section public, après les autres getters
    sf::Color getColor() const { return shape.getFillColor(); }

    //  Setters pour l'effet de survol
    void setScale(float scaleX, float scaleY) {
        shape.setScale(scaleX, scaleY);
        glow.setScale(scaleX * 1.2f, scaleY * 1.2f);
    }

    void setGlowScale(float scaleX, float scaleY) {
        glow.setScale(scaleX, scaleY);
    }

private:
    // Formes principales
    sf::CircleShape shape;
    sf::CircleShape glow;
    std::vector<sf::CircleShape> features;
    sf::ConvexShape rings;

    // Position et mouvement
    sf::Vector2f position;
    sf::Vector2f velocity;
    float angle;
    float angularSpeed;
    float semiMajorAxis;
    float eccentricity;

    // Propriétés planétaires
    std::string name;
    float realRadius;
    float density;
    float rotation;
    float rotationSpeed;
    float orbitTrailAlpha;

    // Méthodes privées
    void createPlanetDetails();
    void updatePlanetRotation(float deltaTime);
};