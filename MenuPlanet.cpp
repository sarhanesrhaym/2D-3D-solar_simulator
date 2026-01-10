#include "MenuPlanet.h"
#include <cmath>

MenuPlanet::MenuPlanet(float semiMajorAxis, sf::Color color, const sf::Vector2f& center,
    const std::string& name, float realRadius, float density)
    : semiMajorAxis(semiMajorAxis), name(name), realRadius(realRadius), density(density),
    angle(0.f), eccentricity(0.f), rotation(0.f), rotationSpeed(0.f), orbitTrailAlpha(100.f) {

    // Calculer la vitesse angulaire basée sur la distance (lois de Kepler simplifiées)
    angularSpeed = 50.f / semiMajorAxis;

    // Angle initial aléatoire
    angle = static_cast<float>(std::rand() % 360) * 3.14159f / 180.f;

    // Calculer la taille visuelle basée sur le rayon réel (échelle logarithmique)
    float visualRadius = 5.f + std::log(realRadius + 1.f) * 4.f;

    // Créer la forme principale
    shape.setRadius(visualRadius);
    shape.setOrigin(visualRadius, visualRadius);
    shape.setFillColor(color);

    // Créer l'effet de lueur
    float glowRadius = visualRadius * 1.5f;
    glow.setRadius(glowRadius);
    glow.setOrigin(glowRadius, glowRadius);
    glow.setFillColor(sf::Color(color.r, color.g, color.b, 50));

    // Position initiale
    position = sf::Vector2f(
        center.x + semiMajorAxis * std::cos(angle),
        center.y + semiMajorAxis * std::sin(angle)
    );

    shape.setPosition(position);
    glow.setPosition(position);

    // Vitesse de rotation propre
    rotationSpeed = 20.f + static_cast<float>(std::rand() % 40);

    createPlanetDetails();
}

void MenuPlanet::update(float deltaTime, const sf::Vector2f& center) {
    // Mise à jour de l'angle orbital
    angle += angularSpeed * deltaTime;
    if (angle > 6.28318f) angle -= 6.28318f;

    // Calcul de la nouvelle position
    position = sf::Vector2f(
        center.x + semiMajorAxis * std::cos(angle),
        center.y + semiMajorAxis * std::sin(angle)
    );

    shape.setPosition(position);
    glow.setPosition(position);

    // Rotation propre de la planète
    rotation += rotationSpeed * deltaTime;
    shape.setRotation(rotation);

    updatePlanetRotation(deltaTime);
}

void MenuPlanet::draw(sf::RenderWindow& window) {
    // Récupérer la couleur de base
    sf::Color baseColor = shape.getFillColor();
    float radius = shape.getRadius();
    sf::Vector2f scale = shape.getScale();
    float scaledRadius = radius * scale.x;

    // 1. Ombre pour effet de profondeur (derrière la planète)
    sf::CircleShape shadow(radius * 1.05f);
    shadow.setOrigin(radius * 1.05f, radius * 1.05f);
    shadow.setPosition(position.x + 2.f * scale.x, position.y + 2.f * scale.y);
    shadow.setScale(scale);
    shadow.setFillColor(sf::Color(0, 0, 0, 60));
    window.draw(shadow);

    // 2. Lueur externe (glow)
    sf::CircleShape outerGlow(radius * 1.3f);
    outerGlow.setOrigin(radius * 1.3f, radius * 1.3f);
    outerGlow.setPosition(position);
    outerGlow.setScale(scale);
    outerGlow.setFillColor(sf::Color(baseColor.r, baseColor.g, baseColor.b, 25));
    window.draw(outerGlow);

    // 3. Corps principal de la planète (base sombre)
    sf::CircleShape planetBase(radius);
    planetBase.setOrigin(radius, radius);
    planetBase.setPosition(position);
    planetBase.setScale(scale);
    // Couleur légèrement plus sombre pour la base
    planetBase.setFillColor(sf::Color(
        static_cast<sf::Uint8>(baseColor.r * 0.7f),
        static_cast<sf::Uint8>(baseColor.g * 0.7f),
        static_cast<sf::Uint8>(baseColor.b * 0.7f)
    ));
    window.draw(planetBase);

    // 4. Zone éclairée (effet 3D - décalée vers haut-gauche)
    sf::CircleShape lightZone(radius * 0.9f);
    lightZone.setOrigin(radius * 0.9f, radius * 0.9f);
    lightZone.setPosition(position.x - scaledRadius * 0.1f, position.y - scaledRadius * 0.1f);
    lightZone.setScale(scale);
    lightZone.setFillColor(baseColor);
    window.draw(lightZone);

    // 5. Zone très éclairée (plus proche de la lumière)
    sf::CircleShape brightZone(radius * 0.7f);
    brightZone.setOrigin(radius * 0.7f, radius * 0.7f);
    brightZone.setPosition(position.x - scaledRadius * 0.15f, position.y - scaledRadius * 0.15f);
    brightZone.setScale(scale);
    // Couleur plus claire
    brightZone.setFillColor(sf::Color(
        static_cast<sf::Uint8>(std::min(255, static_cast<int>(baseColor.r * 1.2f))),
        static_cast<sf::Uint8>(std::min(255, static_cast<int>(baseColor.g * 1.2f))),
        static_cast<sf::Uint8>(std::min(255, static_cast<int>(baseColor.b * 1.2f)))
    ));
    window.draw(brightZone);

    // 6. Point de lumière principal (reflet spéculaire)
    sf::CircleShape highlight(radius * 0.25f);
    highlight.setOrigin(radius * 0.25f, radius * 0.25f);
    highlight.setPosition(position.x - scaledRadius * 0.35f, position.y - scaledRadius * 0.35f);
    highlight.setScale(scale);
    highlight.setFillColor(sf::Color(255, 255, 255, 180));
    window.draw(highlight);

    // 7. Petit reflet secondaire
    sf::CircleShape highlight2(radius * 0.1f);
    highlight2.setOrigin(radius * 0.1f, radius * 0.1f);
    highlight2.setPosition(position.x - scaledRadius * 0.2f, position.y - scaledRadius * 0.5f);
    highlight2.setScale(scale);
    highlight2.setFillColor(sf::Color(255, 255, 255, 120));
    window.draw(highlight2);

    // 8. Bord subtil pour définir la forme
    sf::CircleShape edge(radius);
    edge.setOrigin(radius, radius);
    edge.setPosition(position);
    edge.setScale(scale);
    edge.setFillColor(sf::Color::Transparent);
    edge.setOutlineThickness(1.f);
    edge.setOutlineColor(sf::Color(
        static_cast<sf::Uint8>(baseColor.r * 0.5f),
        static_cast<sf::Uint8>(baseColor.g * 0.5f),
        static_cast<sf::Uint8>(baseColor.b * 0.5f),
        100
    ));
    window.draw(edge);
}

void MenuPlanet::drawOrbit(sf::RenderWindow& window, const sf::Vector2f& center) {
    sf::CircleShape orbit(semiMajorAxis);
    orbit.setOrigin(semiMajorAxis, semiMajorAxis);
    orbit.setPosition(center);
    orbit.setFillColor(sf::Color::Transparent);
    orbit.setOutlineThickness(1.f);
    orbit.setOutlineColor(sf::Color(100, 100, 150, 40));
    window.draw(orbit);
}

// ✅ FONCTIONS GETTERS - ASSUREZ-VOUS QU'ELLES SONT PRÉSENTES
const std::string& MenuPlanet::getName() const {
    return name;
}

float MenuPlanet::getSemiMajorAxis() const {
    return semiMajorAxis;
}

sf::Vector2f MenuPlanet::getPosition() const {
    return position;
}

void MenuPlanet::createPlanetDetails() {
    // Créer quelques détails de surface (cratères, taches)
    int numFeatures = std::rand() % 3;
    for (int i = 0; i < numFeatures; ++i) {
        float featureRadius = shape.getRadius() * 0.2f;
        sf::CircleShape feature(featureRadius);
        feature.setOrigin(featureRadius, featureRadius);
        feature.setFillColor(sf::Color(0, 0, 0, 30));
        features.push_back(feature);
    }
}

void MenuPlanet::updatePlanetRotation(float deltaTime) {
    // Mise à jour des détails de surface avec la rotation
    for (size_t i = 0; i < features.size(); ++i) {
        float featureAngle = rotation + static_cast<float>(i) * 1.5f;
        float offset = shape.getRadius() * 0.3f;
        features[i].setPosition(
            position.x + offset * std::cos(featureAngle * 0.0174533f),
            position.y + offset * std::sin(featureAngle * 0.0174533f)
        );
    }
}