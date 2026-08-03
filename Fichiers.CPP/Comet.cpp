#include "Comet.h"
#include <cstdlib>
#include <cmath>

Comet::Comet(const sf::Vector2f& position) {
    this->position = position;
    this->lifetime = 10.f;
    this->active = true;

    // Configuration du shape
    shape = sf::CircleShape(2.f);
    shape.setOrigin(2.f, 2.f);
    shape.setPosition(position);
    shape.setFillColor(sf::Color(100, 200, 255));

    // Vélocité aléatoire
    velocity = sf::Vector2f(
        (std::rand() % 200 - 100) / 10.f,
        (std::rand() % 200 - 100) / 10.f
    );

    // Initialisation du trail
    trail = sf::VertexArray(sf::LineStrip);
}

void Comet::update(float deltaTime, const sf::Vector2f& sunCenter, float G, float massSun, bool showTrails) {
    if (!active) return;

    // Décrémenter le lifetime
    lifetime -= deltaTime;
    if (lifetime <= 0.f) {
        active = false;
        return;
    }

    // Calcul de l'attraction gravitationnelle vers le soleil
    sf::Vector2f toSun = sunCenter - position;
    float dist = std::sqrt(toSun.x * toSun.x + toSun.y * toSun.y);

    if (dist > 1.f) {
        sf::Vector2f accel = toSun / (dist * dist * dist) * (G * massSun);
        velocity += accel * deltaTime;
    }

    // Mise à jour de la position
    position += velocity * deltaTime;
    shape.setPosition(position);

    // Mise à jour du trail
    if (showTrails) {
        trail.append(sf::Vertex(position, sf::Color(100, 200, 255, 200)));
        if (trail.getVertexCount() > 100) {
            // Décalage des vertices pour maintenir une taille maximale
            for (size_t i = 0; i < trail.getVertexCount() - 1; ++i) {
                trail[i] = trail[i + 1];
            }
            trail.resize(trail.getVertexCount() - 1);
        }
    }
}

void Comet::draw(sf::RenderWindow& window, bool showTrails) {
    if (!active) return;

    if (showTrails) {
        window.draw(trail);
    }
    window.draw(shape);
}