#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Moon.h"

class Planet {
public:
    // Constructeur
    Planet(float semiMajorAxis, float eccentricity,
        const std::string& name, float mass, int texIdx,
        float diamKm, const std::string& fact, sf::Color col, int moons,
        float ringTilt, const std::string& comp, float temp, float grav,
        const std::string& disc, const std::vector<std::string>& chars,
        const sf::Font& font, sf::Texture* texture);

    // Méthodes
    void update(float deltaTime, const sf::Vector2f& sunCenter, float G, float massSun, bool showTrails);
    void draw(sf::RenderWindow& window, bool showTrails, bool showLabels);
    void drawRings(sf::RenderWindow& window, const sf::Texture& ringTexture);
    void drawMoons(sf::RenderWindow& window, bool showLabels);
    void clearTrail();
    
    // Gestion des lunes
    void initializeMoons();
    void updateMoons(float deltaTime);
    const std::vector<Moon>& getMoons() const { return moons; }
    bool hasMoons() const { return !moons.empty(); }

    // Getters
    const sf::Vector2f& getPosition() const;
    const std::string& getName() const { return name; }
    sf::Color getColor() const { return color; }
    bool hasTexture() const { return planetTexture != nullptr; }
    sf::Texture* getTexture() const { return planetTexture; }

    // Nouvelle méthode pour obtenir les infos formatées
    std::vector<std::pair<std::string, std::string>> getFormattedInfo() const;

    // Membres publics (pour simplifier l'accès)
    sf::CircleShape shape;
    sf::Text label;
    sf::VertexArray trail;
    std::string name;
    sf::Color color;
    float mass;
    sf::Vector2f velocity;
    sf::CircleShape glow;
    float semiMajorAxis;
    float eccentricity;
    float angle;
    float angularSpeed;
    float realDiameterKm;
    std::string fact;
    float ringTilt;
    bool visible;
    float orbitalPeriod;
    int numMoons;

    // Propriétés détaillées
    std::string composition;
    float temperature;
    float gravity;
    std::string discovery;
    std::vector<std::string> characteristics;

    // Nouvelles propriétés pour plus de réalisme
    float density;
    float rotationPeriod;
    std::string planetType;
    
    // Système de lunes
    std::vector<Moon> moons;
 bool moonsInitialized;

private:
  sf::Texture* planetTexture;
};