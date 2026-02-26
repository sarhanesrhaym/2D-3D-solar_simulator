#include "Moon.h"
#include <cmath>

Moon::Moon(const std::string& name, float orbitRadius, float size,
   sf::Color color, float orbitalSpeed, float initialAngle)
    : name(name), orbitRadius(orbitRadius), size(size), color(color),
      orbitalSpeed(orbitalSpeed), angle(initialAngle), visible(true),
      realDiameterKm(0.f), realDistanceKm(0.f), moonFont(nullptr) {
    
    // Configuration du shape
  shape = sf::CircleShape(size);
    shape.setOrigin(size, size);
    shape.setFillColor(color);
    
    // Configuration du glow
    glow = sf::CircleShape(size + 2.f);
    glow.setOrigin(size + 2.f, size + 2.f);
    glow.setFillColor(sf::Color(color.r, color.g, color.b, 40));
}

void Moon::update(float deltaTime, const sf::Vector2f& planetCenter) {
    if (!visible) return;
    
    // Mise à jour de l'angle orbital
    angle += orbitalSpeed * deltaTime;
  if (angle > 6.28318f) angle -= 6.28318f;
    
    // Calcul de la nouvelle position
    sf::Vector2f pos(
        planetCenter.x + orbitRadius * std::cos(angle),
        planetCenter.y + orbitRadius * std::sin(angle)
    );
    
    // Mise à jour des positions
    shape.setPosition(pos);
    glow.setPosition(pos);
}

void Moon::draw(sf::RenderWindow& window, bool showLabels) {
    if (!visible) return;
    
    // Dessiner le glow
    window.draw(glow);
    
  // Dessiner la lune
    window.draw(shape);
}

// ===== DONNÉES RÉALISTES DES LUNES =====
std::vector<MoonData> getMoonDataForPlanet(const std::string& planetName) {
    std::vector<MoonData> moons;
    
    if (planetName == "Terre") {
        moons.push_back({
        "Lune",  // Nom
            "Terre",          // Planète parente
            25.f,  // Rayon orbital (unités sim)
   3.5f,             // Taille visuelle
     sf::Color(200, 200, 210),  // Couleur grise
      2.5f,       // Vitesse orbitale
    3474.f,           // Diamètre réel km
        384400.f,  // Distance réelle km
   "Seul satellite naturel de la Terre"
        });
    }
    else if (planetName == "Mars") {
moons.push_back({
   "Phobos",
  "Mars",
            12.f,
    1.5f,
   sf::Color(150, 130, 110),
        5.0f,
         22.f,
       9376.f,
   "Plus grande lune de Mars, forme irreguliere"
        });
        moons.push_back({
            "Deimos",
            "Mars",
     18.f,
     1.0f,
  sf::Color(170, 150, 130),
     3.5f,
   12.f,
   23460.f,
       "Plus petite lune de Mars"
        });
    }
    else if (planetName == "Jupiter") {
        // Les 4 lunes galiléennes
      moons.push_back({
            "Io",
 "Jupiter",
      20.f,
  2.5f,
            sf::Color(255, 220, 100),  // Jaune volcanique
            4.0f,
            3643.f,
       421700.f,
   "Lune la plus volcanique du systeme solaire"
        });
        moons.push_back({
            "Europe",
            "Jupiter",
        28.f,
       2.3f,
sf::Color(200, 220, 255),  // Blanc-bleu glacé
            3.2f,
            3122.f,
            671034.f,
         "Ocean liquide sous la glace"
        });
        moons.push_back({
            "Ganymede",
          "Jupiter",
          38.f,
          3.0f,
            sf::Color(180, 170, 160),  // Gris-brun
            2.5f,
    5268.f,
     1070400.f,
         "Plus grande lune du systeme solaire"
        });
moons.push_back({
    "Callisto",
 "Jupiter",
            50.f,
            2.8f,
            sf::Color(140, 130, 120),  // Gris foncé
            1.8f,
            4821.f,
       1882700.f,
       "Surface la plus craterisee du systeme solaire"
        });
    }
    else if (planetName == "Saturne") {
   moons.push_back({
      "Titan",
      "Saturne",
         35.f,
          3.2f,
   sf::Color(230, 180, 100),  // Orange atmosphérique
       2.0f,
            5150.f,
     1221870.f,
            "Seule lune avec atmosphere dense"
        });
        moons.push_back({
"Encelade",
       "Saturne",
   18.f,
       1.8f,
            sf::Color(240, 245, 255),  // Blanc brillant
4.5f,
  504.f,
238020.f,
  "Geysers d'eau, ocean souterrain"
    });
   moons.push_back({
            "Mimas",
          "Saturne",
 14.f,
            1.5f,
            sf::Color(200, 200, 200),
   5.5f,
            396.f,
            185520.f,
            "Ressemble a l'Etoile de la Mort"
        });
        moons.push_back({
     "Rhea",
     "Saturne",
            28.f,
    2.0f,
       sf::Color(220, 220, 230),
     2.8f,
            1527.f,
    527040.f,
            "Deuxieme plus grande lune de Saturne"
        });
    }
    else if (planetName == "Uranus") {
    moons.push_back({
    "Titania",
    "Uranus",
22.f,
            2.0f,
            sf::Color(180, 190, 200),
   3.0f,
         1578.f,
            435910.f,
            "Plus grande lune d'Uranus"
        });
        moons.push_back({
    "Oberon",
            "Uranus",
            30.f,
            1.9f,
         sf::Color(160, 150, 140),
            2.5f,
       1523.f,
     583520.f,
            "Lune la plus eloignee d'Uranus"
     });
        moons.push_back({
     "Miranda",
      "Uranus",
  14.f,
  1.3f,
    sf::Color(200, 200, 210),
 4.0f,
    472.f,
         129390.f,
            "Surface chaotique et variee"
  });
    }
    else if (planetName == "Neptune") {
        moons.push_back({
       "Triton",
    "Neptune",
  25.f,
            2.5f,
    sf::Color(180, 200, 220),
     2.8f,
     2707.f,
            354760.f,
            "Orbite retrograde, capturee de Kuiper"
        });
      moons.push_back({
        "Protee",
     "Neptune",
         15.f,
     1.2f,
            sf::Color(150, 150, 160),
         4.5f,
   420.f,
     117647.f,
            "Forme irreguliere"
        });
    }
    
    return moons;
}
