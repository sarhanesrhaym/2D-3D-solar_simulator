#pragma once
#include <SFML/Graphics.hpp>
#include <string>

/**
 * @brief Représente un satellite naturel (lune) orbitant autour d'une planète
 */
class Moon {
public:
    // Constructeur
    Moon(const std::string& name, float orbitRadius, float size, 
         sf::Color color, float orbitalSpeed, float initialAngle = 0.f);

    // Méthodes
    void update(float deltaTime, const sf::Vector2f& planetCenter);
    void draw(sf::RenderWindow& window, bool showLabels = false);

    // Getters
    const std::string& getName() const { return name; }
    sf::Vector2f getPosition() const { return shape.getPosition(); }
    float getOrbitRadius() const { return orbitRadius; }

    // Membres publics
    sf::CircleShape shape;
    sf::CircleShape glow;
    sf::Text label;
    std::string name;
    sf::Color color;
    float orbitRadius;      // Distance à la planète
    float orbitalSpeed;     // Vitesse de rotation autour de la planète
    float angle;            // Position angulaire actuelle
    float size;             // Rayon visuel
    bool visible;

    // Données réalistes
    float realDiameterKm;   // Diamètre réel en km
    float realDistanceKm; // Distance réelle à la planète en km
    std::string description;

private:
    const sf::Font* moonFont;
};

/**
 * @brief Données des lunes principales du système solaire
 */
struct MoonData {
    std::string name;
 std::string parentPlanet;
    float orbitRadius;      // En unités de simulation
    float size;   // Taille visuelle
    sf::Color color;
    float orbitalSpeed;     // Vitesse orbitale
    float realDiameterKm;
    float realDistanceKm;
    std::string description;
};

// Fonction pour obtenir les données des lunes par planète
std::vector<MoonData> getMoonDataForPlanet(const std::string& planetName);
