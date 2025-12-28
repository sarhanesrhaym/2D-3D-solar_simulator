#include "Planet.h"
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <iomanip>

Planet::Planet(float semiMajorAxis, float eccentricity,
    const std::string& name, float mass, int texIdx,
    float diamKm, const std::string& fact, sf::Color col, int moons,
    float ringTilt, const std::string& comp, float temp, float grav,
    const std::string& disc, const std::vector<std::string>& chars,
    const sf::Font& font, sf::Texture* texture) {

    this->semiMajorAxis = semiMajorAxis;
    this->eccentricity = eccentricity;
    this->name = name;
    this->mass = mass;
    this->realDiameterKm = diamKm;
    this->fact = fact;
    this->color = col;
    this->numMoons = moons;
    this->ringTilt = ringTilt;
    this->composition = comp;
    this->temperature = temp;
    this->gravity = grav;
    this->discovery = disc;
    this->characteristics = chars;
    this->visible = true;
    this->planetTexture = texture;

    // Nouvelles propriétés réalistes basées sur le type de planète
    if (name == "Mercure") {
        density = 5.43f;
        rotationPeriod = 1407.6f;
        planetType = "Planète tellurique";
        orbitalPeriod = 88.0f;
    }
    else if (name == "Venus") {
        density = 5.24f;
        rotationPeriod = 5832.5f;
        planetType = "Planète tellurique";
        orbitalPeriod = 224.7f;
    }
    else if (name == "Terre") {
        density = 5.51f;
        rotationPeriod = 23.9f;
        planetType = "Planète tellurique";
        orbitalPeriod = 365.2f;
    }
    else if (name == "Mars") {
        density = 3.93f;
        rotationPeriod = 24.6f;
        planetType = "Planète tellurique";
        orbitalPeriod = 687.0f;
    }
    else if (name == "Jupiter") {
        density = 1.33f;
        rotationPeriod = 9.9f;
        planetType = "Géante gazeuse";
        orbitalPeriod = 4331.0f;
    }
    else if (name == "Saturne") {
        density = 0.69f;
        rotationPeriod = 10.7f;
        planetType = "Géante gazeuse";
        orbitalPeriod = 10747.0f;
    }
    else if (name == "Uranus") {
        density = 1.27f;
        rotationPeriod = 17.2f;
        planetType = "Géante de glaces";
        orbitalPeriod = 30589.0f;
    }
    else if (name == "Neptune") {
        density = 1.64f;
        rotationPeriod = 16.1f;
        planetType = "Géante de glaces";
        orbitalPeriod = 59800.0f;
    }
    else {
        density = 3.0f;
        rotationPeriod = 24.0f;
        planetType = "Planète";
        orbitalPeriod = 365.0f;
    }

    // Calcul de la vitesse angulaire
    const float G = 1e-3f;
    const float massSun = 1e6f;
    this->angularSpeed = std::sqrt(G * massSun / (semiMajorAxis * semiMajorAxis * semiMajorAxis));

    // Angle initial aléatoire
    this->angle = (std::rand() % 360) * 3.14159f / 180.f;

    // Position initiale
    float r = semiMajorAxis * (1 - eccentricity * eccentricity) / (1 + eccentricity * std::cos(angle));
    sf::Vector2f pos(r * std::cos(angle), r * std::sin(angle));

    // Configuration du shape
    float radius = 3.f + std::log2(semiMajorAxis / 60.f) * 3.f;
    shape = sf::CircleShape(radius);
    shape.setOrigin(radius, radius);
    shape.setPosition(pos);
    if (texture) {
        shape.setTexture(texture);
    }

    // Configuration du label
    label.setFont(font);
    label.setCharacterSize(13);
    label.setFillColor(sf::Color::White);
    label.setString(name);
    label.setStyle(sf::Text::Bold);
    label.setPosition(pos + sf::Vector2f(-25, -35));

    // Configuration du glow
    glow = sf::CircleShape(radius + 5.f);
    glow.setOrigin(radius + 5.f, radius + 5.f);
    glow.setFillColor(sf::Color(col.r, col.g, col.b, 60));
    glow.setPosition(pos);

    // Initialisation du trail
    trail = sf::VertexArray(sf::LineStrip);

    // Calcul de la vitesse initiale
    float v = std::sqrt(G * massSun * (2 / r - 1 / semiMajorAxis));
    velocity = sf::Vector2f(-v * std::sin(angle), v * std::cos(angle) * std::sqrt(1 - eccentricity * eccentricity));
}

std::vector<std::pair<std::string, std::string>> Planet::getFormattedInfo() const {
    std::vector<std::pair<std::string, std::string>> info;

    // Formatage simple et clair
    std::string massStr = std::to_string(static_cast<int>(mass * 1e6)) + " × 10²³ kg";
    std::string diamStr = std::to_string(static_cast<int>(realDiameterKm)) + " km";
    std::string tempStr = std::to_string(static_cast<int>(temperature)) + " °C";
    std::string gravStr = std::to_string(static_cast<int>(gravity)) + " m/s²";
    std::string densStr = std::to_string(static_cast<int>(density)) + " g/cm³";

    // Période orbitale SIMPLE
    std::string orbitStr = std::to_string(static_cast<int>(orbitalPeriod)) + " j";

    // Période de rotation SIMPLE  
    std::string rotationStr = std::to_string(static_cast<int>(rotationPeriod)) + " h";

    info.push_back({ "Type", planetType });
    info.push_back({ "Diamètre", diamStr });
    info.push_back({ "Masse", massStr });
    info.push_back({ "Température", tempStr });
    info.push_back({ "Gravité", gravStr });
    info.push_back({ "Densité", densStr });
    info.push_back({ "Satellites", std::to_string(numMoons) });
    info.push_back({ "Orbite", orbitStr });
    info.push_back({ "Rotation", rotationStr });
    info.push_back({ "Découverte", discovery });

    return info;
}

void Planet::update(float deltaTime, const sf::Vector2f& sunCenter, float G, float massSun, bool showTrails) {
    if (!visible) return;

    // Mise à jour de l'angle
    angle += angularSpeed * deltaTime;

    // Calcul de la nouvelle position
    float r = semiMajorAxis * (1 - eccentricity * eccentricity) / (1 + eccentricity * std::cos(angle));
    sf::Vector2f pos(sunCenter.x + r * std::cos(angle), sunCenter.y + r * std::sin(angle));

    // Mise à jour des positions
    shape.setPosition(pos);
    glow.setPosition(pos);
    label.setPosition(pos + sf::Vector2f(-25, -35));

    // Mise à jour du trail
    if (showTrails) {
        trail.append(sf::Vertex(pos, color));
        if (trail.getVertexCount() > 400) {
            // Décalage des vertices pour maintenir une taille maximale
            for (size_t i = 0; i < trail.getVertexCount() - 1; ++i) {
                trail[i] = trail[i + 1];
            }
            trail.resize(trail.getVertexCount() - 1);
        }
    }
}

void Planet::draw(sf::RenderWindow& window, bool showTrails, bool showLabels) {
    if (!visible) return;

    if (showTrails) {
        window.draw(trail);
    }
    window.draw(glow);

    // Utiliser la texture si disponible
    if (planetTexture) {
        shape.setTexture(planetTexture);
    }

    window.draw(shape);
    if (showLabels) {
        window.draw(label);
    }
}

void Planet::drawRings(sf::RenderWindow& window, const sf::Texture& ringTexture) {
    if (name != "Saturne") return;

    float radius = shape.getRadius();
    sf::Vector2f pos = shape.getPosition();

    sf::Sprite ring(ringTexture);
    ring.setOrigin(ringTexture.getSize().x / 2.f, ringTexture.getSize().y / 2.f);
    ring.setPosition(pos);
    ring.setRotation(ringTilt);
    float ringScale = radius * 2.8f / ringTexture.getSize().x;
    ring.setScale(ringScale, ringScale * 0.1f);
    ring.setColor(sf::Color(255, 255, 255, 200));

    window.draw(ring);
}

void Planet::clearTrail() {
    trail.clear();
}

const sf::Vector2f& Planet::getPosition() const {
    return shape.getPosition();
}