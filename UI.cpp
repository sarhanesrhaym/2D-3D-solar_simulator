#include "UI.h"
#include "Comet.h"
#include "Constants.h"
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <iostream>
#include <algorithm>
#include "Moon.h"
#include "SpaceMission.h"
#include "EducationalMode.h"  

// ===== ÉTOILES FILANTES =====
struct ShootingStar {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float lifetime;
    float maxLifetime;
    sf::VertexArray trail;

    ShootingStar() : trail(sf::LineStrip, 20), lifetime(1.5f), maxLifetime(1.5f) {}
};

static std::vector<ShootingStar> shootingStars;
static float shootingStarTimer = 0.f;

// ===== FONCTION MISE À JOUR ÉTOILES FILANTES =====
static void updateShootingStars(float deltaTime, unsigned int width, unsigned int height) {
    shootingStarTimer += deltaTime;

    if (shootingStarTimer > 4.f + static_cast<float>(std::rand() % 30) / 10.f) {
        shootingStarTimer = 0.f;

        ShootingStar star;
        //  Cast explicite pour éviter overflow
        star.position = sf::Vector2f(
            static_cast<float>(std::rand() % std::max(1u, width)),
            static_cast<float>(std::rand() % std::max(1u, height / 3u))
        );
        star.velocity = sf::Vector2f(
            350.f + static_cast<float>(std::rand() % 200),
            120.f + static_cast<float>(std::rand() % 100)
        );
        star.lifetime = star.maxLifetime;
        shootingStars.push_back(star);
    }

    // Mettre à jour les étoiles filantes
    for (auto& star : shootingStars) {
        star.position += star.velocity * deltaTime;
        star.lifetime -= deltaTime;

        // Mettre à jour la traînée
        for (int i = 19; i > 0; --i) {
            star.trail[i].position = star.trail[i - 1].position;
            float alpha = 255.f * (1.f - static_cast<float>(i) / 20.f) * (star.lifetime / star.maxLifetime);
            star.trail[i].color = sf::Color(255, 255, 255, static_cast<sf::Uint8>(std::max(0.f, alpha)));
        }
        star.trail[0].position = star.position;
        float headAlpha = 255.f * (star.lifetime / star.maxLifetime);
        star.trail[0].color = sf::Color(255, 255, 255, static_cast<sf::Uint8>(std::max(0.f, headAlpha)));
    }

    // Nettoyer les étoiles mortes
    shootingStars.erase(
        std::remove_if(shootingStars.begin(), shootingStars.end(),
            [](const ShootingStar& s) { return s.lifetime <= 0.f; }),
        shootingStars.end()
    );
}

// ===== FONCTION DESSIN ETOILES FILANTES =====
static void drawShootingStars(sf::RenderWindow& window) {
    for (const auto& star : shootingStars) {
        window.draw(star.trail);

        // Point lumineux à la tête
        sf::CircleShape head(2.5f);
        head.setOrigin(2.5f, 2.5f);
        head.setPosition(star.position);
        float alpha = 255.f * (star.lifetime / star.maxLifetime);
        head.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(std::max(0.f, alpha))));
        window.draw(head);
    }
}
// ===== EFFET PARALLAXE =====
static sf::Vector2f mouseParallaxOffset(0.f, 0.f);

static void updateParallaxEffect(const sf::Vector2f& mousePos, const sf::Vector2u& windowSize) {
    float centerX = static_cast<float>(windowSize.x) / 2.f;
    float centerY = static_cast<float>(windowSize.y) / 2.f;

    // Calculer le décalage cible basé sur la position de la souris
    float targetX = (mousePos.x - centerX) / centerX * 20.f;
    float targetY = (mousePos.y - centerY) / centerY * 15.f;

    // Interpolation douce pour un mouvement fluide (lerp)
    mouseParallaxOffset.x += (targetX - mouseParallaxOffset.x) * 0.08f;
    mouseParallaxOffset.y += (targetY - mouseParallaxOffset.y) * 0.08f;
}

// Getter pour accéder à l'offset depuis d'autres fonctions
static sf::Vector2f getParallaxOffset() {
    return mouseParallaxOffset;
}
// ===== PLANETES INTERACTIVES AVEC TOOLTIP =====
struct PlanetTooltip {
    std::string name;
    sf::Vector2f position;
    float alpha;
    bool visible;
    int hoveredIndex;

    PlanetTooltip() : name(""), position(0.f, 0.f), alpha(0.f),
        visible(false), hoveredIndex(-1) {
    }
};

static PlanetTooltip planetTooltip;


// Mise à jour de l'interaction avec les planètes
static void updatePlanetInteraction(std::vector<MenuPlanet>& menuPlanets,
    const sf::Vector2f& mousePos,
    float deltaTime) {
    bool foundHover = false;

    for (size_t i = 0; i < menuPlanets.size(); ++i) {
        auto& mp = menuPlanets[i];
        sf::Vector2f planetPos = mp.getPosition();

        //  les getters 
        float baseRadius = mp.getRadius();
        float currentScale = mp.getScale().x;
        float effectiveRadius = baseRadius * currentScale;

        // Calculer la distance entre la souris et le centre de la planète
        float distance = std::sqrt(
            std::pow(mousePos.x - planetPos.x, 2) +
            std::pow(mousePos.y - planetPos.y, 2)
        );

        // Vérifier si la souris survole la planète
        if (distance < effectiveRadius + 12.f) {
            foundHover = true;
            planetTooltip.hoveredIndex = static_cast<int>(i);
            planetTooltip.name = mp.getName();
            planetTooltip.position = planetPos;
            planetTooltip.visible = true;

            //  Utiliser setScale au lieu d'accéder directement
            float targetScale = 1.5f;
            float newScale = currentScale + (targetScale - currentScale) * 0.15f;
            mp.setScale(newScale, newScale);

            // Augmenter l'alpha du tooltip
            planetTooltip.alpha = std::min(255.f, planetTooltip.alpha + 800.f * deltaTime);
        }
        else {
            // Réduire progressivement l'échelle si pas survolée
            float currentScaleVal = mp.getScale().x;
            if (currentScaleVal > 1.f) {
                float newScale = currentScaleVal + (1.f - currentScaleVal) * 0.12f;
                if (newScale < 1.01f) newScale = 1.f;
                mp.setScale(newScale, newScale);
            }
        }
    }

    // Si aucune planète n'est survolée, faire disparaître le tooltip
    if (!foundHover) {
        planetTooltip.alpha = std::max(0.f, planetTooltip.alpha - 600.f * deltaTime);
        if (planetTooltip.alpha <= 0.f) {
            planetTooltip.visible = false;
            planetTooltip.hoveredIndex = -1;
        }
    }
}

// Dessiner le tooltip de la planète
static void drawPlanetTooltip(sf::RenderWindow& window, const sf::Font& font) {
    if (!planetTooltip.visible || planetTooltip.alpha <= 5.f) return;

    sf::Uint8 alpha = static_cast<sf::Uint8>(std::min(255.f, planetTooltip.alpha));

    // Position du tooltip au-dessus de la planète
    float tooltipY = planetTooltip.position.y - 50.f;

    // Fond du tooltip avec bordure arrondie (simulée avec rectangle)
    float textWidth = static_cast<float>(planetTooltip.name.length()) * 10.f + 30.f;
    sf::RectangleShape tooltipBg(sf::Vector2f(textWidth, 32.f));
    tooltipBg.setOrigin(textWidth / 2.f, 16.f);
    tooltipBg.setPosition(planetTooltip.position.x, tooltipY);
    tooltipBg.setFillColor(sf::Color(15, 20, 40, static_cast<sf::Uint8>(alpha * 0.9f)));
    tooltipBg.setOutlineThickness(2.f);
    tooltipBg.setOutlineColor(sf::Color(100, 200, 255, alpha));
    window.draw(tooltipBg);

    // Texte du nom de la planète
    sf::Text tooltipText(planetTooltip.name, font, 16);
    sf::FloatRect textBounds = tooltipText.getLocalBounds();
    tooltipText.setOrigin(textBounds.width / 2.f, textBounds.height / 2.f);
    tooltipText.setPosition(planetTooltip.position.x, tooltipY - 2.f);
    tooltipText.setFillColor(sf::Color(255, 255, 255, alpha));
    tooltipText.setStyle(sf::Text::Bold);
    window.draw(tooltipText);

    // Petite flèche pointant vers la planète
    sf::ConvexShape arrow;
    arrow.setPointCount(3);
    arrow.setPoint(0, sf::Vector2f(0.f, 0.f));
    arrow.setPoint(1, sf::Vector2f(-8.f, -12.f));
    arrow.setPoint(2, sf::Vector2f(8.f, -12.f));
    arrow.setPosition(planetTooltip.position.x, tooltipY + 16.f);
    arrow.setFillColor(sf::Color(100, 200, 255, alpha));
    window.draw(arrow);

    // Effet de lueur autour de la planète survolée
    sf::CircleShape hoverGlow(25.f);
    hoverGlow.setOrigin(25.f, 25.f);
    hoverGlow.setPosition(planetTooltip.position);
    hoverGlow.setFillColor(sf::Color(100, 200, 255, static_cast<sf::Uint8>(alpha * 0.15f)));
    window.draw(hoverGlow);
}

// Structure pour les particules de couleur
struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    sf::Color color;
    float lifetime;
    float maxLifetime;
    float size;
    float rotation;
    float rotationSpeed;

    // Constructeur pour initialiser tous les membres
    Particle() : position(0.f, 0.f), velocity(0.f, 0.f), acceleration(0.f, 0.f),
        color(255, 255, 255, 255), lifetime(0.f), maxLifetime(0.f),
        size(0.f), rotation(0.f), rotationSpeed(0.f) {
    }
};

struct LightTrail {
    sf::VertexArray trail;
    float lifetime;
    float maxLifetime;
    sf::Color color;

    // Constructeur
    LightTrail() : trail(sf::LineStrip, 2), lifetime(0.f),
        maxLifetime(0.f), color(255, 255, 255, 255) {
    }
};

// Variables globales pour les particules - VARIABLES CRITIQUES DÉCLARÉES ICI
static std::vector<Particle> menuParticles;
static std::vector<LightTrail> lightTrails;  
static float particleEmissionTimer = 0.f;

// ===== ASTRONAUTE, VAISSEAU NASA ET MÉTÉORITES =====
static sf::Texture astronautTexture;
static sf::Texture spacecraftTexture;
static sf::Texture meteoriteTexture;
static bool spaceElementsLoaded = false;

// Structure pour l'astronaute flottant
struct FloatingAstronaut {
    sf::Sprite sprite;
    sf::Vector2f position;
    float floatPhase;
    bool loaded;

    FloatingAstronaut() : position(100.f, 550.f), floatPhase(0.f), loaded(false) {}

    void update(float deltaTime) {
        floatPhase += deltaTime * 1.5f;
        float floatX = 8.f * std::sin(floatPhase * 0.8f);
        float floatY = 12.f * std::sin(floatPhase);
        sprite.setPosition(position.x + floatX, position.y + floatY);
        sprite.setRotation(5.f * std::sin(floatPhase * 0.5f));
    }

    void draw(sf::RenderWindow& window) {
        if (loaded) {
            window.draw(sprite);
        }
    }
};

// Structure pour le vaisseau NASA
struct NASASpacecraft {
    sf::Sprite sprite;
    sf::Vector2f position;
    sf::Vector2f velocity;
    float phase;
    bool loaded;

    NASASpacecraft() : position(-100.f, 120.f), velocity(50.f, 0.f),
        phase(0.f), loaded(false) {
    }

    void update(float deltaTime, unsigned int windowWidth) {
        phase += deltaTime;
        position.x += velocity.x * deltaTime;

        if (position.x > static_cast<float>(windowWidth) + 100.f) {
            position.x = -150.f;
            position.y = 80.f + static_cast<float>(std::rand() % 150);
        }

        float waveY = 10.f * std::sin(phase * 2.f);
        sprite.setPosition(position.x, position.y + waveY);
        sprite.setRotation(3.f * std::sin(phase * 1.5f));
    }

    void draw(sf::RenderWindow& window) {
        if (loaded) {
            window.draw(sprite);
        }
    }
};

// Structure pour les météorites
struct Meteorite {
    sf::Sprite sprite;
    sf::Vector2f position;
    sf::Vector2f velocity;
    float rotation;
    float rotationSpeed;
    float scale;
    bool active;
    sf::VertexArray trail;

    Meteorite() : position(0.f, 0.f), velocity(0.f, 0.f), rotation(0.f),
        rotationSpeed(0.f), scale(1.f), active(false),
        trail(sf::LineStrip, 10) {
    }

    void spawn(unsigned int windowWidth, unsigned int windowHeight) {
        int side = std::rand() % 3;
        if (side == 0) {
            //  Cast explicite
            position.x = static_cast<float>(std::rand() % std::max(1u, windowWidth));
            position.y = -50.f;
            velocity.x = static_cast<float>((std::rand() % 80) - 40);  
            velocity.y = 100.f + static_cast<float>(std::rand() % 80);
        }
        else if (side == 1) {
            position.x = -50.f;
            position.y = static_cast<float>(std::rand() % std::max(1u, windowHeight / 2u));
            velocity.x = 80.f + static_cast<float>(std::rand() % 60);
            velocity.y = 50.f + static_cast<float>(std::rand() % 40);
        }
        else {
            position.x = static_cast<float>(windowWidth) + 50.f;
            position.y = static_cast<float>(std::rand() % std::max(1u, windowHeight / 2u));
            velocity.x = -(80.f + static_cast<float>(std::rand() % 60));
            velocity.y = 50.f + static_cast<float>(std::rand() % 40);
        }

        rotation = static_cast<float>(std::rand() % 360);
        rotationSpeed = static_cast<float>((std::rand() % 300) - 150);  
        scale = 0.05f + static_cast<float>(std::rand() % 100) / 1000.f;

        //  TAILLE RÉDUITE : entre 0.05 et 0.15 (au lieu de 0.2 à 0.87)
        scale = 0.05f + static_cast<float>(std::rand() % 100) / 1000.f;
        active = true;

        for (int i = 0; i < 10; ++i) {
            trail[i].position = position;
            trail[i].color = sf::Color(255, 150, 50, 0);
        }
    }

    void update(float deltaTime, unsigned int windowWidth, unsigned int windowHeight) {
        if (!active) return;

        position += velocity * deltaTime;
        rotation += rotationSpeed * deltaTime;

        sprite.setPosition(position);
        sprite.setRotation(rotation);
        sprite.setScale(scale, scale);

        for (int i = 9; i > 0; --i) {
            trail[i].position = trail[i - 1].position;
            float alpha = 150.f * (1.f - static_cast<float>(i) / 10.f);
            trail[i].color = sf::Color(255, 100 + i * 10, 50,
                static_cast<sf::Uint8>(std::max(0.f, alpha)));
        }
        trail[0].position = position;
        trail[0].color = sf::Color(255, 180, 80, 180);

        if (position.x < -100.f || position.x > windowWidth + 100.f ||
            position.y > windowHeight + 100.f) {
            active = false;
        }
    }

    void draw(sf::RenderWindow& window, bool textureLoaded) {
        if (!active) return;

        window.draw(trail);

        if (textureLoaded) {
            window.draw(sprite);
        }
        else {
            //  Fallback avec taille réduite
            sf::CircleShape meteor(8.f * scale * 10.f); // Ajusté pour le fallback
            meteor.setOrigin(8.f * scale * 10.f, 8.f * scale * 10.f);
            meteor.setPosition(position);
            meteor.setFillColor(sf::Color(139, 90, 43));
            meteor.setOutlineThickness(1.f);
            meteor.setOutlineColor(sf::Color(180, 130, 80));
            window.draw(meteor);
        }
    }
};


// Instances globales
static FloatingAstronaut menuAstronaut;
static NASASpacecraft menuSpacecraft;
static std::vector<Meteorite> menuMeteorites(5);
static float meteoriteSpawnTimer = 0.f;

// Fonction d'initialisation des éléments spatiaux
static void initializeSpaceElements() {
    if (spaceElementsLoaded) return;

    // Charger texture astronaute
    if (astronautTexture.loadFromFile("textures/astronaut.png")) {
        astronautTexture.setSmooth(true);
        menuAstronaut.sprite.setTexture(astronautTexture);
        sf::FloatRect bounds = menuAstronaut.sprite.getLocalBounds();
        float scale = 100.f / std::max(bounds.width, bounds.height);
        menuAstronaut.sprite.setScale(scale, scale);
        menuAstronaut.sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        menuAstronaut.position = sf::Vector2f(100.f, 580.f);
        menuAstronaut.loaded = true;
        std::cout << "Astronaut texture loaded successfully!" << std::endl;
    }
    else {
        std::cerr << "Failed to load astronaut.png" << std::endl;
    }

    // Charger texture vaisseau
    if (spacecraftTexture.loadFromFile("textures/spacecraft.png")) {
        spacecraftTexture.setSmooth(true);
        menuSpacecraft.sprite.setTexture(spacecraftTexture);
        sf::FloatRect bounds = menuSpacecraft.sprite.getLocalBounds();
        float scale = 70.f / std::max(bounds.width, bounds.height);
        menuSpacecraft.sprite.setScale(scale, scale);
        menuSpacecraft.sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        menuSpacecraft.loaded = true;
        std::cout << "Spacecraft texture loaded successfully!" << std::endl;
    }
    else {
        std::cerr << "Failed to load spacecraft.png" << std::endl;
    }

    // Charger texture météorite -  TAILLE RÉDUITE
    if (meteoriteTexture.loadFromFile("textures/meteorite.png")) {
        meteoriteTexture.setSmooth(true);
        for (auto& m : menuMeteorites) {
            m.sprite.setTexture(meteoriteTexture);
            sf::FloatRect bounds = m.sprite.getLocalBounds();
            m.sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
            //  Échelle initiale petite 
            m.sprite.setScale(0.1f, 0.1f);
        }
        std::cout << "Meteorite texture loaded successfully!" << std::endl;
    }
    else {
        std::cerr << "Failed to load meteorite.png" << std::endl;
    }

    spaceElementsLoaded = true;
}

// Fonction de mise à jour des éléments spatiaux
static void updateSpaceElements(float deltaTime, unsigned int windowWidth, unsigned int windowHeight) {
    initializeSpaceElements();

    menuAstronaut.update(deltaTime);
    menuSpacecraft.update(deltaTime, windowWidth);

    meteoriteSpawnTimer += deltaTime;
    if (meteoriteSpawnTimer > 2.5f + static_cast<float>(std::rand() % 30) / 10.f) {
        meteoriteSpawnTimer = 0.f;

        for (auto& m : menuMeteorites) {
            if (!m.active) {
                m.spawn(windowWidth, windowHeight);
                break;
            }
        }
    }

    for (auto& m : menuMeteorites) {
        m.update(deltaTime, windowWidth, windowHeight);
    }
}

// Fonction de dessin des éléments spatiaux
static void drawSpaceElements(sf::RenderWindow& window) {
    // Météorites (arrière-plan)
    bool meteorTextureOK = meteoriteTexture.getSize().x > 0;
    for (auto& m : menuMeteorites) {
        m.draw(window, meteorTextureOK);
    }

    // Vaisseau
    menuSpacecraft.draw(window);

    // Astronaute (premier plan)
    menuAstronaut.draw(window);
}
// ===== PALETTE DE COULEURS FUTURISTE =====

FuturisticPalette::FuturisticPalette()
    : deepPurple(8, 15, 35),           // Bleu nuit profond (au lieu de violet)
    electricBlue(30, 100, 180),        // Bleu spatial réaliste
    luminousPink(50, 80, 120),         // Bleu-gris pour nébuleuses (au lieu de rose)
    solarYellow(255, 200, 50),         // Jaune solaire réaliste
    crispWhite(255, 255, 255)          // Blanc pur pour étoiles
{
}

// Variables globales pour le fond futuriste
static FuturisticPalette cosmoColors;
static sf::RenderTexture backgroundTexture;
static bool backgroundInitialized = false;

/*
 * @brief Génère un dégradé diagonal lisse du violet au bleu
 */
static void generateGradientBackground(sf::RenderTexture& texture, const FuturisticPalette& palette,
    unsigned int width, unsigned int height) {
    texture.clear(palette.deepPurple);

    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            float t = (static_cast<float>(x) / width + static_cast<float>(y) / height) / 2.f;
            t = std::max(0.f, std::min(1.f, t));

            sf::Color color(
                static_cast<sf::Uint8>(palette.deepPurple.r * (1.f - t) + palette.electricBlue.r * t),
                static_cast<sf::Uint8>(palette.deepPurple.g * (1.f - t) + palette.electricBlue.g * t),
                static_cast<sf::Uint8>(palette.deepPurple.b * (1.f - t) + palette.electricBlue.b * t)
            );

            sf::CircleShape pixel(0.5f);
            pixel.setPosition(static_cast<float>(x), static_cast<float>(y));
            pixel.setFillColor(color);
            texture.draw(pixel);
        }
    }
}

/**
 * @brief Ajoute des nébuleuses vaporeu...ses animées au fond
 */
static void addNebulasToBackground(sf::RenderTexture& texture, const FuturisticPalette& palette,
    float phase, unsigned int width, unsigned int height) {
    sf::CircleShape nebula1(350.f);
    nebula1.setPosition(-50.f, -100.f);
    float alpha1 = static_cast<sf::Uint8>(40 + 35 * std::sin(phase));
    nebula1.setFillColor(sf::Color(palette.deepPurple.r, palette.deepPurple.g,
        palette.deepPurple.b, alpha1));
    texture.draw(nebula1);

    sf::CircleShape nebula2(320.f);
    nebula2.setPosition(static_cast<float>(width) - 200.f, static_cast<float>(height) - 150.f);
    float alpha2 = static_cast<sf::Uint8>(35 + 30 * std::sin(phase + 1.57f));
    nebula2.setFillColor(sf::Color(palette.electricBlue.r, palette.electricBlue.g,
        palette.electricBlue.b, alpha2));
    texture.draw(nebula2);

    sf::CircleShape nebula3(300.f);
    nebula3.setPosition(static_cast<float>(width) - 100.f, static_cast<float>(height) / 2.f - 150.f);
    float alpha3 = static_cast<sf::Uint8>(30 + 25 * std::sin(phase + 3.14f));
    nebula3.setFillColor(sf::Color(palette.luminousPink.r, palette.luminousPink.g,
        palette.luminousPink.b, alpha3));
    texture.draw(nebula3);
}

/**
 * @brief Ajoute des points lumineux (étoiles/particules) au fond
 */
static void addLuminousPointsToBackground(sf::RenderTexture& texture, const FuturisticPalette& palette,
    unsigned int width, unsigned int height) {
    std::srand(42);

    for (int i = 0; i < 150; ++i) {
        float x = static_cast<float>(std::rand() % width);
        float y = static_cast<float>(std::rand() % height);
        float size = 0.5f + static_cast<float>(std::rand() % 3);

        sf::CircleShape point(size);
        point.setPosition(x, y);

        int colorChoice = std::rand() % 100;
        if (colorChoice < 70) {
            point.setFillColor(palette.crispWhite);
        }
        else if (colorChoice < 90) {
            point.setFillColor(palette.solarYellow);
        }
        else {
            point.setFillColor(palette.luminousPink);
        }

        texture.draw(point);
    }
}

/**
 * @brief Ajoute des traînées lumineuses courbes (orbites/flux d'énergie)
 */
static void addEnergyTracesToBackground(sf::RenderTexture& texture, const FuturisticPalette& palette,
    unsigned int width, unsigned int height) {
    sf::VertexArray trace1(sf::LineStrip, 50);
    for (int i = 0; i < 50; ++i) {
        float t = static_cast<float>(i) / 50.f;
        float x = t * width * 0.8f;
        float y = 100.f + t * height * 0.6f + 50.f * std::sin(t * 6.28f);

        sf::Uint8 alpha = static_cast<sf::Uint8>(150 * (1.f - t));
        trace1[i].position = sf::Vector2f(x, y);
        trace1[i].color = sf::Color(palette.luminousPink.r, palette.luminousPink.g,
            palette.luminousPink.b, alpha);
    }
    texture.draw(trace1);

    sf::VertexArray trace2(sf::LineStrip, 50);
    for (int i = 0; i < 50; ++i) {
        float t = static_cast<float>(i) / 50.f;
        float x = width - t * width * 0.7f;
        float y = height - 100.f - t * height * 0.5f + 40.f * std::cos(t * 6.28f);

        sf::Uint8 alpha = static_cast<sf::Uint8>(120 * (1.f - t));
        trace2[i].position = sf::Vector2f(x, y);
        trace2[i].color = sf::Color(palette.electricBlue.r, palette.electricBlue.g,
            palette.electricBlue.b, alpha);
    }
    texture.draw(trace2);
}

/**
 * @brief Ajoute un noyau solaire énergétique stylisé au centre
 */
static void addSolarCoreToBackground(sf::RenderTexture& texture, const FuturisticPalette& palette,
    unsigned int width, unsigned int height, float phase) {
    sf::Vector2f center(width / 2.f, height / 2.f);

    sf::CircleShape auraOuter(120.f);
    auraOuter.setOrigin(120.f, 120.f);
    auraOuter.setPosition(center);
    float auraAlpha1 = static_cast<sf::Uint8>(60 + 40 * std::sin(phase));
    auraOuter.setFillColor(sf::Color(palette.luminousPink.r, palette.luminousPink.g,
        palette.luminousPink.b, auraAlpha1));
    texture.draw(auraOuter);

    sf::CircleShape auraMiddle(80.f);
    auraMiddle.setOrigin(80.f, 80.f);
    auraMiddle.setPosition(center);
    float auraAlpha2 = static_cast<sf::Uint8>(40 + 30 * std::sin(phase + 1.57f));
    auraMiddle.setFillColor(sf::Color(palette.electricBlue.r, palette.electricBlue.g,
        palette.electricBlue.b, auraAlpha2));
    texture.draw(auraMiddle);

    sf::CircleShape core(50.f);
    core.setOrigin(50.f, 50.f);
    core.setPosition(center);
    core.setFillColor(palette.solarYellow);
    texture.draw(core);

    sf::CircleShape brightCore(20.f);
    brightCore.setOrigin(20.f, 20.f);
    brightCore.setPosition(center);
    brightCore.setFillColor(palette.crispWhite);
    texture.draw(brightCore);
}

/**
 * @brief Initialise et génère le fond d'écran futuriste complet
 */
void initializeFuturisticBackground(sf::RenderWindow& window) {
    if (backgroundInitialized) return;

    unsigned int width = window.getSize().x;
    unsigned int height = window.getSize().y;

    if (!backgroundTexture.create(width, height)) {
        std::cerr << "Erreur: Impossible de créer la texture de fond." << std::endl;
        return;
    }

    static float initPhase = 0.5f;

    generateGradientBackground(backgroundTexture, cosmoColors, width, height);
    addNebulasToBackground(backgroundTexture, cosmoColors, initPhase, width, height);
    addLuminousPointsToBackground(backgroundTexture, cosmoColors, width, height);
    addEnergyTracesToBackground(backgroundTexture, cosmoColors, width, height);
    addSolarCoreToBackground(backgroundTexture, cosmoColors, width, height, initPhase);

    backgroundTexture.display();
    backgroundInitialized = true;
}

/**
 * @brief Dessine le fond futuriste à l'écran
 */
void drawFuturisticBackground(sf::RenderWindow& window, float phase) {
    if (!backgroundInitialized) {
        initializeFuturisticBackground(window);
    }

    sf::Sprite backgroundSprite(backgroundTexture.getTexture());
    window.draw(backgroundSprite);

    unsigned int width = window.getSize().x;
    unsigned int height = window.getSize().y;
    sf::Vector2f center(width / 2.f, height / 2.f);

    float pulseScale = 1.f + 0.1f * std::sin(phase * 2.f);

    sf::CircleShape pulsingCore(50.f * pulseScale);
    pulsingCore.setOrigin(50.f * pulseScale, 50.f * pulseScale);
    pulsingCore.setPosition(center);
    sf::Uint8 pulseAlpha = static_cast<sf::Uint8>(100 + 50 * std::sin(phase * 3.f));
    pulsingCore.setFillColor(sf::Color(cosmoColors.solarYellow.r, cosmoColors.solarYellow.g,
        cosmoColors.solarYellow.b, pulseAlpha));
    window.draw(pulsingCore);
}

void createStarfield(std::vector<Star>& stars) {
    stars.clear();
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (int i = 0; i < 1200; ++i) {
        stars.push_back(Star());
    }
}

// FONCTION UNIQUE AVEC DONNÉES RÉALISTES DES PLANÈTES
void createMenuSolarSystem(std::vector<MenuPlanet>& menuPlanets, const sf::Vector2f& center) {
    menuPlanets.clear();

    // Données réalistes des planètes : (orbite, couleur, nom, rayon réel, densité)
    std::vector<std::tuple<float, sf::Color, std::string, float, float>> planetData = {
        {80.f, sf::Color(169, 169, 169), "Mercure", 0.38f, 5.43f},      // Gris
        {120.f, sf::Color(255, 198, 73), "Vénus", 0.95f, 5.24f},        // Jaune-orange
        {160.f, sf::Color(100, 149, 237), "Terre", 1.0f, 5.52f},        // Bleu-cyan
        {200.f, sf::Color(205, 92, 92), "Mars", 0.53f, 3.93f},          // Rouge
        {280.f, sf::Color(218, 165, 32), "Jupiter", 11.21f, 1.33f},     // Or-brun
        {350.f, sf::Color(238, 232, 170), "Saturne", 9.45f, 0.687f},    // Pâle
        {420.f, sf::Color(175, 238, 238), "Uranus", 4.01f, 1.27f},      // Cyan clair
        {480.f, sf::Color(65, 105, 225), "Neptune", 3.88f, 1.638f}      // Bleu foncé
    };

    for (const auto& data : planetData) {
        menuPlanets.push_back(MenuPlanet(
            std::get<0>(data), std::get<1>(data), center,
            std::get<2>(data), std::get<3>(data), std::get<4>(data)
        ));
    }
}

void updateStars(std::vector<Star>& stars, float time) {
    for (auto& s : stars) {
        s.update(time);
    }
}

void updateMenuSolarSystem(std::vector<MenuPlanet>& menuPlanets, float deltaTime, const sf::Vector2f& center) {
    //  Mise à jour des étoiles filantes
    updateShootingStars(deltaTime, 1200, 800);

    for (auto& mp : menuPlanets) {
        mp.update(deltaTime, center);
    }

    // Mise à jour des particules avancées avec accélération
    particleEmissionTimer += deltaTime;

    // Émission de nouvelles particules autour du soleil - densité augmentée
    if (particleEmissionTimer > 0.015f) {
        particleEmissionTimer = 0.f;

        // Deux types de particules
        for (int pType = 0; pType < 2; ++pType) {
            float angle = static_cast<float>(std::rand() % 360) * 3.14159f / 180.f;
            float distance = 65.f + static_cast<float>(std::rand() % 40);

            Particle p;
            p.position = sf::Vector2f(
                center.x + distance * std::cos(angle),
                center.y + distance * std::sin(angle)
            );

            // Vélocité radiale sortante
            float velocityMagnitude = 30.f + static_cast<float>(std::rand() % 40);
            p.velocity = sf::Vector2f(
                velocityMagnitude * std::cos(angle),
                velocityMagnitude * std::sin(angle)
            );

            // Accélération de gravité vers le soleil
            p.acceleration = sf::Vector2f(
                -50.f * std::cos(angle),
                -50.f * std::sin(angle)
            );

            // Couleurs variées et vibrantes - CONVERSIONS INT?FLOAT CORRIGÉES
            int colorChoice = std::rand() % 6;
            switch (colorChoice) {
            case 0: p.color = sf::Color(255, 255, 100, 220); break;  // Jaune vif
            case 1: p.color = sf::Color(255, 165, 0, 220); break;    // Orange
            case 2: p.color = sf::Color(255, 100, 50, 200); break;   // Rouge-orange
            case 3: p.color = sf::Color(255, 200, 50, 210); break;   // Jaune-or
            case 4: p.color = sf::Color(255, 150, 0, 200); break;    // Orange foncé
            case 5: p.color = sf::Color(255, 220, 100, 190); break;  // Jaune pâle
            }

            p.lifetime = 1.8f;
            p.maxLifetime = 1.8f;
            p.size = 1.5f + static_cast<float>(std::rand() % 4);
            p.rotation = static_cast<float>(std::rand() % 360);
            p.rotationSpeed = static_cast<float>(std::rand() % 200 - 100) * 2.f;

            menuParticles.push_back(p);
        }
    }

    // Mise à jour et nettoyage des particules avec physique réaliste
    for (auto& particle : menuParticles) {
        // Application de l'accélération
        particle.velocity += particle.acceleration * deltaTime * 0.5f;
        particle.position += particle.velocity * deltaTime;
        particle.rotation += particle.rotationSpeed * deltaTime;
        particle.lifetime -= deltaTime;

        // Effet de trainée lumineuse légère
        float speed = std::sqrt(particle.velocity.x * particle.velocity.x +
            particle.velocity.y * particle.velocity.y);
        if (speed > 50.f) {
            LightTrail trail;
            trail.trail[0].position = particle.position;
            trail.trail[1].position = particle.position - particle.velocity * deltaTime * 0.5f;
            trail.trail[0].color = particle.color;
            trail.trail[1].color = sf::Color(particle.color.r, particle.color.g,
                particle.color.b, 50);
            trail.lifetime = 0.3f;
            trail.maxLifetime = 0.3f;
            trail.color = particle.color;
            lightTrails.push_back(trail);
        }

        // Fade out graduel
        float alpha = 220.f * (particle.lifetime / particle.maxLifetime);
        particle.color.a = static_cast<sf::Uint8>(std::max(0.f, std::min(255.f, alpha)));
    }

    // Nettoyage des particules mortes
    menuParticles.erase(
        std::remove_if(menuParticles.begin(), menuParticles.end(),
            [](const Particle& p) { return p.lifetime <= 0.f; }),
        menuParticles.end()
    );

    // Mise à jour des trainées lumineuses
    for (auto& trail : lightTrails) {
        trail.lifetime -= deltaTime;
    }

    lightTrails.erase(
        std::remove_if(lightTrails.begin(), lightTrails.end(),
            [](const LightTrail& t) { return t.lifetime <= 0.f; }),
        lightTrails.end()
    );
}

// ===== FONCTIONS DE DESSIN - RESTE IDENTIQUE =====

void drawMainMenu(sf::RenderWindow& window, const sf::Font& font,
    std::vector<CosmicButton>& menuButtons,
    std::vector<Star>& stars,
    std::vector<MenuPlanet>& menuPlanets,
    const sf::Vector2f& center) {

    static float bgAnimation = 0.f;
    bgAnimation += 0.005f;

    // Récupérer la position de la souris et mettre à jour le parallaxe
    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(mousePixelPos);
    updateParallaxEffect(mousePos, window.getSize());
    sf::Vector2f parallax = getParallaxOffset();

    // Mettre à jour l'interaction avec les planètes
    static float lastDeltaTime = 0.016f; // Approximation ~60 FPS
    updatePlanetInteraction(menuPlanets, mousePos, lastDeltaTime);

    drawFuturisticBackground(window, bgAnimation);

    // Appliquer le parallaxe via une vue temporaire
    sf::View originalView = window.getView();
    sf::View parallaxView = originalView;
    parallaxView.move(-parallax.x * 0.3f, -parallax.y * 0.3f);
    window.setView(parallaxView);

    // Fond étoilé (dessiné avec la vue décalée)
    for (auto& s : stars) {
        s.draw(window);
    }

    // Restaurer la vue originale
    window.setView(originalView);

    // Dessiner les étoiles filantes
    drawShootingStars(window);

    // Afficher les orbites AVANT les planètes
    for (auto& mp : menuPlanets) {
        mp.drawOrbit(window, center);
    }

    // Système solaire du menu
    for (auto& mp : menuPlanets) {
        mp.draw(window);
    }

    //  Dessiner le tooltip de la planète survolée
    drawPlanetTooltip(window, font);

    // Nébuleuses animées avec parallaxe (couche moyenne)
    static float nebulaPhase = 0.f;
    nebulaPhase += 0.002f;

    // Nébuleuse 1 - Bleu spatial profond
    sf::CircleShape nebula1(350.f);
    nebula1.setPosition(100.f - parallax.x * 0.5f, 50.f - parallax.y * 0.5f);
    float alpha1 = 20.f + 15.f * std::sin(nebulaPhase);
    nebula1.setFillColor(sf::Color(20, 50, 100,
        static_cast<sf::Uint8>(std::max(0.f, std::min(255.f, alpha1)))));
    window.draw(nebula1);

    // Nébuleuse 2 - Cyan doux
    sf::CircleShape nebula2(300.f);
    nebula2.setPosition(700.f - parallax.x * 0.4f, 450.f - parallax.y * 0.4f);
    float alpha2 = 15.f + 10.f * std::sin(nebulaPhase + 1.57f);
    nebula2.setFillColor(sf::Color(30, 80, 130,
        static_cast<sf::Uint8>(std::max(0.f, std::min(255.f, alpha2)))));
    window.draw(nebula2);

    // Nébuleuse 3 - Bleu-gris subtil (au lieu de rose)
    sf::CircleShape nebula3(280.f);
    nebula3.setPosition(900.f - parallax.x * 0.6f, 100.f - parallax.y * 0.6f);
    float alpha3 = 12.f + 8.f * std::sin(nebulaPhase + 3.14f);
    nebula3.setFillColor(sf::Color(40, 60, 100,
        static_cast<sf::Uint8>(std::max(0.f, std::min(255.f, alpha3)))));
    window.draw(nebula3);

    // Grille de fond animée avec parallaxe
    for (int i = -5; i <= 15; ++i) {
        for (int j = -5; j <= 10; ++j) {
            float gridX = static_cast<float>(i * 200) - parallax.x * 0.8f;
            float gridY = static_cast<float>(j * 200) - parallax.y * 0.8f;

            sf::CircleShape gridPoint(1.5f);
            gridPoint.setPosition(gridX, gridY);
            float gridAlpha = 20.f + 10.f * std::sin(nebulaPhase + i * 0.5f + j * 0.3f);
            gridPoint.setFillColor(sf::Color(0, 255, 255, static_cast<sf::Uint8>(std::max(0.f, std::min(255.f, gridAlpha)))));
            window.draw(gridPoint);
        }
    }


    // Particules de couleur
    for (const auto& particle : menuParticles) {
        sf::CircleShape particleShape(particle.size);
        particleShape.setOrigin(particle.size, particle.size);
        particleShape.setPosition(particle.position);
        particleShape.setFillColor(particle.color);
        window.draw(particleShape);
    }

    // Traînées lumineuses
    for (const auto& trail : lightTrails) {
        window.draw(trail.trail);
    }


    // ===== SOLEIL AVEC EFFET SPHÉRIQUE 3D =====
    static float sunPulse = 0.f;
    sunPulse += 0.015f;

    // Ombre pour donner de la profondeur (derrière le soleil)
    sf::CircleShape sunShadow(52.f);
    sunShadow.setOrigin(52.f, 52.f);
    sunShadow.setPosition(center.x + 3.f, center.y + 3.f);
    sunShadow.setFillColor(sf::Color(100, 50, 0, 80));
    window.draw(sunShadow);

    // Halo externe
    sf::CircleShape outerGlow(90.f);
    outerGlow.setOrigin(90.f, 90.f);
    outerGlow.setPosition(center);
    outerGlow.setFillColor(sf::Color(255, 180, 50, 30));
    window.draw(outerGlow);

    // Halo moyen
    sf::CircleShape midGlow(70.f);
    midGlow.setOrigin(70.f, 70.f);
    midGlow.setPosition(center);
    midGlow.setFillColor(sf::Color(255, 200, 80, 50));
    window.draw(midGlow);

    // Corps principal (base)
    sf::CircleShape sunBase(50.f);
    sunBase.setOrigin(50.f, 50.f);
    sunBase.setPosition(center);
    sunBase.setFillColor(sf::Color(255, 180, 50)); // Orange-jaune
    window.draw(sunBase);

    // Zone claire (effet 3D - partie éclairée)
    sf::CircleShape sunLight(45.f);
    sunLight.setOrigin(45.f, 45.f);
    sunLight.setPosition(center.x - 5.f, center.y - 5.f);
    sunLight.setFillColor(sf::Color(255, 220, 100));
    window.draw(sunLight);

    // Zone très claire
    sf::CircleShape sunBright(35.f);
    sunBright.setOrigin(35.f, 35.f);
    sunBright.setPosition(center.x - 8.f, center.y - 8.f);
    sunBright.setFillColor(sf::Color(255, 245, 180));
    window.draw(sunBright);

    // Point de lumière (reflet)
    sf::CircleShape sunHighlight(12.f);
    sunHighlight.setOrigin(12.f, 12.f);
    sunHighlight.setPosition(center.x - 20.f, center.y - 20.f);
    sunHighlight.setFillColor(sf::Color(255, 255, 255, 200));
    window.draw(sunHighlight);

    // Petit reflet secondaire
    sf::CircleShape sunHighlight2(5.f);
    sunHighlight2.setOrigin(5.f, 5.f);
    sunHighlight2.setPosition(center.x - 10.f, center.y - 25.f);
    sunHighlight2.setFillColor(sf::Color(255, 255, 255, 150));
    window.draw(sunHighlight2);

    // ===== TITRE PRINCIPAL - STYLE MODERNE ULTRA LUMINEUX =====
    static float titlePulse = 0.f;
    titlePulse += 0.03f;

    // Effet de lueur derrière le titre
    sf::Text titleGlow("COSMO UIT", font, 85);
    titleGlow.setPosition(600.f, 80.f);
    titleGlow.setOrigin(titleGlow.getLocalBounds().width / 2.f, titleGlow.getLocalBounds().height / 2.f);
    float glowAlpha = 80.f + 40.f * std::sin(titlePulse * 0.8f);
    titleGlow.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(glowAlpha)));
    titleGlow.setStyle(sf::Text::Bold);
    window.draw(titleGlow);

    // Ombre du titre
    sf::Text titleShadow("COSMO UIT", font, 80);
    titleShadow.setPosition(604.f, 84.f);
    titleShadow.setOrigin(titleShadow.getLocalBounds().width / 2.f, titleShadow.getLocalBounds().height / 2.f);
    titleShadow.setFillColor(sf::Color(0, 0, 0, 120));
    titleShadow.setStyle(sf::Text::Bold);
    window.draw(titleShadow);

    // Titre principal avec dégradé animé (blanc ? cyan ? blanc)
    sf::Text title("COSMO UIT", font, 80);
    title.setPosition(600.f, 80.f);
    title.setOrigin(title.getLocalBounds().width / 2.f, title.getLocalBounds().height / 2.f);

    //  COULEUR ULTRA CLAIRE : Blanc lumineux avec reflet cyan
    float colorShift = std::sin(titlePulse);
    sf::Uint8 r = static_cast<sf::Uint8>(220 + 35 * colorShift);
    sf::Uint8 g = static_cast<sf::Uint8>(250 + 5 * std::abs(colorShift));
    sf::Uint8 b = 255;
    title.setFillColor(sf::Color(r, g, b, 255));
    title.setStyle(sf::Text::Bold);

    // Contour lumineux animé
    title.setOutlineThickness(4.f);
    float outlineR = 100.f + 155.f * (0.5f + 0.5f * std::sin(titlePulse + 1.f));
    float outlineG = 200.f + 55.f * (0.5f + 0.5f * std::sin(titlePulse + 2.f));
    title.setOutlineColor(sf::Color(
        static_cast<sf::Uint8>(outlineR),
        static_cast<sf::Uint8>(outlineG),
        255, 200
    ));
    window.draw(title);

    // ===== SOUS-TITRE - STYLE ÉLÉGANT ULTRA LUMINEUX =====
    static float subtitlePulse = 0.f;
    subtitlePulse += 0.025f;

    // Ombre du sous-titre
    sf::Text subtitleShadow("- SIMULATEUR SOLAIRE 2D -", font, 28);
    subtitleShadow.setPosition(602.f, 162.f);
    subtitleShadow.setOrigin(subtitleShadow.getLocalBounds().width / 2.f, subtitleShadow.getLocalBounds().height / 2.f);
    subtitleShadow.setFillColor(sf::Color(0, 0, 0, 100));
    subtitleShadow.setStyle(sf::Text::Italic);
    window.draw(subtitleShadow);

    // Sous-titre principal
    sf::Text subtitle("- SIMULATEUR SOLAIRE 2D -", font, 28);
    subtitle.setPosition(600.f, 160.f);
    subtitle.setOrigin(subtitle.getLocalBounds().width / 2.f, subtitle.getLocalBounds().height / 2.f);

    //  COULEUR ULTRA CLAIRE : Dégradé Or ? Blanc ? Or
    float subColorShift = std::sin(subtitlePulse * 1.5f);
    sf::Uint8 subR = 255;
    sf::Uint8 subG = static_cast<sf::Uint8>(220 + 35 * std::abs(subColorShift));
    sf::Uint8 subB = static_cast<sf::Uint8>(150 + 105 * std::abs(subColorShift));
    subtitle.setFillColor(sf::Color(subR, subG, subB, 255));
    subtitle.setStyle(sf::Text::Italic | sf::Text::Bold);

    // Contour doré lumineux
    subtitle.setOutlineThickness(2.f);
    float subOutlineAlpha = 150.f + 100.f * std::sin(subtitlePulse);
    subtitle.setOutlineColor(sf::Color(255, 200, 100, static_cast<sf::Uint8>(subOutlineAlpha)));
    window.draw(subtitle);

    // ===== LIGNE DÉCORATIVE SOUS LE SOUS-TITRE =====
    float lineWidth = 300.f + 50.f * std::sin(subtitlePulse * 2.f);
    sf::RectangleShape decorLine(sf::Vector2f(lineWidth, 2.f));
    decorLine.setOrigin(lineWidth / 2.f, 1.f);
    decorLine.setPosition(600.f, 195.f);
    float lineAlpha = 150.f + 100.f * std::sin(subtitlePulse * 1.5f);
    decorLine.setFillColor(sf::Color(200, 230, 255, static_cast<sf::Uint8>(lineAlpha)));
    window.draw(decorLine);




    // Version et année avec animation
    static float versionPulse = 0.f;
    versionPulse += 0.015f;
    float versionAlpha = 100 + 50 * std::sin(versionPulse);


    //  BOUTONS AVEC COULEURS FUTURISTES ET INTERACTIONS
    for (size_t i = 0; i < menuButtons.size(); ++i) {
        auto& b = menuButtons[i];

        // Animation d'échelle au clic
        float scaleAnimation = 1.f;
        if (b.clicked) {
            scaleAnimation = 0.95f;
        }
        else if (b.hover) {
            scaleAnimation = 1.05f;
        }

        sf::Vector2f originalPos = b.base.getPosition();
        sf::Vector2f originalSize = b.base.getSize();

        sf::Vector2f btnCenter = sf::Vector2f(
            originalPos.x + originalSize.x / 2.f,
            originalPos.y + originalSize.y / 2.f
        );

        if (b.hover || b.clicked) {
            sf::CircleShape shadowGlow(originalSize.x / 2.f + 20.f);
            shadowGlow.setOrigin(shadowGlow.getRadius(), shadowGlow.getRadius());
            shadowGlow.setPosition(btnCenter);

            if (b.clicked) {
                shadowGlow.setFillColor(sf::Color(255, 94, 98, 80));
            }
            else {
                shadowGlow.setFillColor(sf::Color(37, 117, 252, 60));
            }
            window.draw(shadowGlow);
        }

        sf::RectangleShape shadow = b.base;
        shadow.move(4, 4);
        shadow.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(shadow);

        b.draw(window);

        if (b.clicked) {
            for (int spark = 0; spark < 4; ++spark) {
                float angle = (spark / 4.f) * 6.28f;
                float distance = 15.f;
                sf::CircleShape sparkle(2.f);
                sparkle.setPosition(
                    btnCenter.x + distance * std::cos(angle) - 2.f,
                    btnCenter.y + distance * std::sin(angle) - 2.f
                );
                sparkle.setFillColor(sf::Color(255, 209, 102, 200));
                window.draw(sparkle);
            }
        }
    }

    // ===== ASTRONAUTE, VAISSEAU NASA ET MÉTÉORITES =====
    updateSpaceElements(lastDeltaTime, window.getSize().x, window.getSize().y);
    drawSpaceElements(window);


    // Indicateur de haut vers le bas (pulsant)
    static float indicatorPulse = 0.f;
    indicatorPulse += 0.05f;
    float indicatorAlpha = 100 + 80 * std::sin(indicatorPulse);




}

// ===== NOUVELLE PRÉSENTATION - DESIGN MODERNE SPATIAL =====

void drawPresentation(sf::RenderWindow& window, const sf::Font& font,
    std::vector<CosmicButton>& backButtons,
    std::vector<Star>& stars, float time) {

    // ===== FOND SPATIAL AVEC DÉGRADÉ LUMINEUX =====
    for (int i = 0; i < 12; ++i) {
        float ratio = static_cast<float>(i) / 12.f;
        sf::RectangleShape bgLayer(sf::Vector2f(1200.f, 65.f));
        bgLayer.setPosition(0.f, i * 65.f);

        // Dégradé du bleu profond vers le bleu-violet spatial
        sf::Uint8 r = static_cast<sf::Uint8>(8 + 20 * ratio);
        sf::Uint8 g = static_cast<sf::Uint8>(15 + 35 * ratio);
        sf::Uint8 b = static_cast<sf::Uint8>(40 + 60 * ratio);
        bgLayer.setFillColor(sf::Color(r, g, b, 255));
        window.draw(bgLayer);
    }

    // Étoiles animées
    for (auto& s : stars) {
        s.draw(window);
    }

    // ===== NÉBULEUSES LUMINEUSES ANIMÉES =====
    static float nebulaPhase = 0.f;
    nebulaPhase += 0.008f;

    // Grande nébuleuse cyan (haut gauche)
    for (int n = 0; n < 3; ++n) {
        float nebulaSize = 400.f - n * 80.f;
        float nebulaAlpha = 15.f + 10.f * std::sin(nebulaPhase + n * 0.5f);
        sf::CircleShape nebula(nebulaSize);
        nebula.setPosition(-150.f + n * 20.f, -100.f + n * 30.f);
        nebula.setFillColor(sf::Color(80, 200, 255, static_cast<sf::Uint8>(nebulaAlpha)));
        window.draw(nebula);
    }

    // Nébuleuse violette (bas droite)
    for (int n = 0; n < 3; ++n) {
        float nebulaSize = 350.f - n * 70.f;
        float nebulaAlpha = 12.f + 8.f * std::sin(nebulaPhase + 1.57f + n * 0.5f);
        sf::CircleShape nebula(nebulaSize);
        nebula.setPosition(900.f + n * 15.f, 450.f + n * 20.f);
        nebula.setFillColor(sf::Color(150, 100, 255, static_cast<sf::Uint8>(nebulaAlpha)));
        window.draw(nebula);
    }

    // ===== PARTICULES FLOTTANTES =====
    for (int p = 0; p < 30; ++p) {
        float px = 100.f + (p * 37) % 1000;
        float py = 100.f + (p * 53) % 600;
        float pSize = 1.f + std::sin(time * 2.f + p) * 0.5f;
        float pAlpha = 100.f + 100.f * std::sin(time * 3.f + p * 0.3f);

        sf::CircleShape particle(pSize);
        particle.setPosition(px + 5.f * std::sin(time + p), py + 3.f * std::cos(time * 0.8f + p));
        particle.setFillColor(sf::Color(200, 230, 255, static_cast<sf::Uint8>(pAlpha)));
        window.draw(particle);
    }

    // ===== CADRE PRINCIPAL HEXAGONAL  =====
    float frameX = 40.f;
    float frameY = 25.f;
    float frameW = 1120.f;
    float frameH = 710.f;

    // Ombre du cadre principal
    sf::RectangleShape frameShadow(sf::Vector2f(frameW + 8.f, frameH + 8.f));
    frameShadow.setPosition(frameX + 4.f, frameY + 4.f);
    frameShadow.setFillColor(sf::Color(0, 0, 0, 60));
    window.draw(frameShadow);

    // Cadre principal avec effet verre
    for (int i = 0; i < 6; ++i) {
        float ratio = static_cast<float>(i) / 6.f;
        sf::RectangleShape frameLayer(sf::Vector2f(frameW, frameH / 6.f));
        frameLayer.setPosition(frameX, frameY + i * frameH / 6.f);

        sf::Uint8 r = static_cast<sf::Uint8>(15 + 25 * ratio);
        sf::Uint8 g = static_cast<sf::Uint8>(30 + 40 * ratio);
        sf::Uint8 b = static_cast<sf::Uint8>(60 + 50 * ratio);
        frameLayer.setFillColor(sf::Color(r, g, b, 235));
        window.draw(frameLayer);
    }

    // Bordure lumineuse animée
    sf::RectangleShape frameBorder(sf::Vector2f(frameW, frameH));
    frameBorder.setPosition(frameX, frameY);
    frameBorder.setFillColor(sf::Color::Transparent);
    float borderPulse = 2.f + 1.f * std::sin(time * 2.f);
    frameBorder.setOutlineThickness(borderPulse);
    float borderAlpha = 180.f + 75.f * std::sin(time * 2.5f);
    frameBorder.setOutlineColor(sf::Color(100, 200, 255, static_cast<sf::Uint8>(borderAlpha)));
    window.draw(frameBorder);

    // Coins lumineux
    float cornerSize = 25.f;
    std::vector<sf::Vector2f> corners = {
        {frameX, frameY},
        {frameX + frameW - cornerSize, frameY},
        {frameX, frameY + frameH - cornerSize},
        {frameX + frameW - cornerSize, frameY + frameH - cornerSize}
    };

    for (const auto& corner : corners) {
        sf::RectangleShape cornerGlow(sf::Vector2f(cornerSize, cornerSize));
        cornerGlow.setPosition(corner);
        float cornerAlpha = 150.f + 100.f * std::sin(time * 3.f);
        cornerGlow.setFillColor(sf::Color(150, 220, 255, static_cast<sf::Uint8>(cornerAlpha * 0.3f)));
        window.draw(cornerGlow);
    }

    // ===== EN-TÊTE MODERNE =====
    // Fond de l'en-tête avec dégradé
    for (int i = 0; i < 4; ++i) {
        float ratio = static_cast<float>(i) / 4.f;
        sf::RectangleShape headerLayer(sf::Vector2f(frameW - 20.f, 22.f));
        headerLayer.setPosition(frameX + 10.f, frameY + 10.f + i * 22.f);

        sf::Uint8 r = static_cast<sf::Uint8>(30 + 80 * (1.f - ratio));
        sf::Uint8 g = static_cast<sf::Uint8>(100 + 120 * (1.f - ratio));
        sf::Uint8 b = static_cast<sf::Uint8>(180 + 70 * (1.f - ratio));
        headerLayer.setFillColor(sf::Color(r, g, b, 200));
        window.draw(headerLayer);
    }

    // Ligne brillante sous l'en-tête
    sf::RectangleShape headerGlow(sf::Vector2f(frameW - 20.f, 4.f));
    headerGlow.setPosition(frameX + 10.f, frameY + 96.f);
    float glowIntensity = 200.f + 55.f * std::sin(time * 4.f);
    headerGlow.setFillColor(sf::Color(150, 230, 255, static_cast<sf::Uint8>(glowIntensity)));
    window.draw(headerGlow);

    // ===== TITRE PRINCIPAL AVEC EFFETS =====
    // Lueur derrière le titre
    sf::Text titleGlow("PRESENTATION DU PROJET COSMOUIT", font, 42);
    titleGlow.setPosition(600.f, 35.f);
    titleGlow.setOrigin(titleGlow.getLocalBounds().width / 2.f, 0.f);
    float titleGlowAlpha = 80.f + 50.f * std::sin(time * 1.5f);
    titleGlow.setFillColor(sf::Color(200, 240, 255, static_cast<sf::Uint8>(titleGlowAlpha)));
    titleGlow.setStyle(sf::Text::Bold);
    window.draw(titleGlow);

    // Titre principal
    sf::Text mainTitle("PRESENTATION DU PROJET COSMOUIT", font, 38);
    mainTitle.setPosition(600.f, 38.f);
    mainTitle.setOrigin(mainTitle.getLocalBounds().width / 2.f, 0.f);
    mainTitle.setFillColor(sf::Color(255, 255, 255, 255));
    mainTitle.setStyle(sf::Text::Bold);
    mainTitle.setLetterSpacing(2.f);
    mainTitle.setOutlineThickness(2.f);
    mainTitle.setOutlineColor(sf::Color(50, 150, 220, 200));
    window.draw(mainTitle);

    // Sous-titre
    sf::Text subtitle("Simulateur Solaire 2D - Universite Ibn Tofail - 2025/2026", font, 16);
    subtitle.setPosition(600.f, 80.f);
    subtitle.setOrigin(subtitle.getLocalBounds().width / 2.f, 0.f);
    subtitle.setFillColor(sf::Color(180, 220, 255, 230));
    subtitle.setStyle(sf::Text::Italic);
    window.draw(subtitle);

    // ===== SYSTÈME SOLAIRE MINIATURE DÉCORATIF =====
    float solarX = 1050.f;
    float solarY = 60.f;
    float solarRadius = 35.f;

    // Soleil miniature
    sf::CircleShape miniSun(12.f);
    miniSun.setOrigin(12.f, 12.f);
    miniSun.setPosition(solarX, solarY);
    miniSun.setFillColor(sf::Color(255, 220, 100));
    window.draw(miniSun);

    // Orbites et planètes
    for (int o = 0; o < 4; ++o) {
        float orbitR = 18.f + o * 10.f;
        sf::CircleShape orbit(orbitR);
        orbit.setOrigin(orbitR, orbitR);
        orbit.setPosition(solarX, solarY);
        orbit.setFillColor(sf::Color::Transparent);
        orbit.setOutlineThickness(1.f);
        orbit.setOutlineColor(sf::Color(100, 180, 255, 60));
        window.draw(orbit);

        // Planète sur l'orbite
        float pAngle = time * (0.5f + o * 0.3f) + o * 1.57f;
        sf::CircleShape planet(2.f + o * 0.5f);
        planet.setOrigin(2.f + o * 0.5f, 2.f + o * 0.5f);
        planet.setPosition(solarX + orbitR * std::cos(pAngle), solarY + orbitR * std::sin(pAngle));

        std::vector<sf::Color> planetColors = {
            sf::Color(180, 180, 180), sf::Color(255, 200, 100),
            sf::Color(100, 150, 255), sf::Color(255, 120, 100)
        };
        planet.setFillColor(planetColors[o]);
        window.draw(planet);
    }

    // ===== DISPOSITION DES 4 CARTES =====
    float cardW = 520.f;
    float cardH = 250.f;
    float cardGap = 20.f;
    float cardsStartX = frameX + 25.f;
    float cardsStartY = frameY + 115.f;

    // Couleurs des cartes (bordures lumineuses)
    std::vector<sf::Color> cardColors = {
        sf::Color(255, 220, 80),   // Or - Introduction
        sf::Color(80, 255, 150),   // Vert - Fonctionnalités
        sf::Color(255, 150, 80),   // Orange - Techniques
        sf::Color(80, 200, 255)    // Cyan - Technologies
    };

    std::vector<std::string> cardTitles = {
        "INTRODUCTION GENERALE",
        "FONCTIONNALITES UTILISATEUR",
        "CARACTERISTIQUES TECHNIQUES",
        "TECHNOLOGIES ET OUTILS"
    };

    std::vector<std::string> cardIcons = { "*", ">", "+", "#" };

    for (int c = 0; c < 4; ++c) {
        int row = c / 2;
        int col = c % 2;

        float cardX = cardsStartX + col * (cardW + cardGap);
        float cardY = cardsStartY + row * (cardH + cardGap);

        // ===== CARTE AVEC EFFET VERRE =====
        // Ombre de la carte
        sf::RectangleShape cardShadow(sf::Vector2f(cardW, cardH));
        cardShadow.setPosition(cardX + 4.f, cardY + 4.f);
        cardShadow.setFillColor(sf::Color(0, 0, 0, 50));
        window.draw(cardShadow);

        // Fond dégradé de la carte
        for (int l = 0; l < 5; ++l) {
            float ratio = static_cast<float>(l) / 5.f;
            sf::RectangleShape cardLayer(sf::Vector2f(cardW, cardH / 5.f));
            cardLayer.setPosition(cardX, cardY + l * cardH / 5.f);

            sf::Uint8 r = static_cast<sf::Uint8>(20 + 20 * ratio);
            sf::Uint8 g = static_cast<sf::Uint8>(35 + 30 * ratio);
            sf::Uint8 b = static_cast<sf::Uint8>(65 + 40 * ratio);
            cardLayer.setFillColor(sf::Color(r, g, b, 230));
            window.draw(cardLayer);
        }

        // Bordure lumineuse de la carte
        sf::RectangleShape cardBorder(sf::Vector2f(cardW, cardH));
        cardBorder.setPosition(cardX, cardY);
        cardBorder.setFillColor(sf::Color::Transparent);
        float cardPulse = 2.f + 0.5f * std::sin(time * 2.f + c * 0.7f);
        cardBorder.setOutlineThickness(cardPulse);
        float cardBorderAlpha = 180.f + 75.f * std::sin(time * 2.5f + c * 0.5f);
        cardBorder.setOutlineColor(sf::Color(
            cardColors[c].r, cardColors[c].g, cardColors[c].b,
            static_cast<sf::Uint8>(cardBorderAlpha)));
        window.draw(cardBorder);

        // Barre colorée en haut de la carte
        sf::RectangleShape cardTopBar(sf::Vector2f(cardW, 5.f));
        cardTopBar.setPosition(cardX, cardY);
        cardTopBar.setFillColor(cardColors[c]);
        window.draw(cardTopBar);

        // Ligne lumineuse animée
        float linePos = std::fmod(time * 100.f + c * 130.f, cardW);
        sf::RectangleShape scanLine(sf::Vector2f(60.f, 3.f));
        scanLine.setPosition(cardX + linePos, cardY + 1.f);
        scanLine.setFillColor(sf::Color(255, 255, 255, 150));
        window.draw(scanLine);

        // ===== EN-TÊTE DE LA CARTE =====
        // Cercle avec icône
        sf::CircleShape iconCircle(18.f);
        iconCircle.setPosition(cardX + 15.f, cardY + 18.f);
        iconCircle.setFillColor(sf::Color(cardColors[c].r, cardColors[c].g, cardColors[c].b, 200));
        iconCircle.setOutlineThickness(2.f);
        iconCircle.setOutlineColor(sf::Color(255, 255, 255, 200));
        window.draw(iconCircle);

        sf::Text iconText(cardIcons[c], font, 20);
        iconText.setPosition(cardX + 33.f, cardY + 22.f);
        iconText.setOrigin(iconText.getLocalBounds().width / 2.f, 0.f);
        iconText.setFillColor(sf::Color(30, 30, 40));
        iconText.setStyle(sf::Text::Bold);
        window.draw(iconText);

        // Titre de la carte
        sf::Text cardTitle(cardTitles[c], font, 20);
        cardTitle.setPosition(cardX + 60.f, cardY + 20.f);
        cardTitle.setFillColor(cardColors[c]);
        cardTitle.setStyle(sf::Text::Bold);
        window.draw(cardTitle);

        // Ligne séparatrice
        sf::RectangleShape separator(sf::Vector2f(cardW - 30.f, 2.f));
        separator.setPosition(cardX + 15.f, cardY + 50.f);
        separator.setFillColor(sf::Color(cardColors[c].r, cardColors[c].g, cardColors[c].b, 100));
        window.draw(separator);

        // ===== CONTENU DES CARTES =====
        float contentY = cardY + 65.f;

        if (c == 0) {
            // INTRODUCTION
            std::vector<std::string> introLines = {
                "CosmoUIT represente une avancee significative dans",
                "la visualisation educative du systeme solaire.",
                "",
                "Ce simulateur 2D offre une experience immersive",
                "pour explorer les mecanismes celestes.",
                "",
                "Developpe dans le cadre d'un projet universitaire",
                "d'excellence a l'Universite Ibn Tofail."
            };

            for (size_t i = 0; i < introLines.size(); ++i) {
                sf::Text line(introLines[i], font, 13);
                line.setPosition(cardX + 20.f, contentY + i * 20.f);
                line.setFillColor(sf::Color(220, 235, 255, 255));
                window.draw(line);
            }
        }
        else if (c == 1) {
            // FONCTIONNALITÉS
            std::vector<std::pair<std::string, std::string>> features = {
                {"1. Navigation fluide", "360 degres complet"},
                {"2. Zoom avance", "x0.1 a x50"},
                {"3. Vitesse simulation", "Controle temps reel"},
                {"4. Affichage selectif", "Orbites/Labels/Grille"},
                {"5. Suivi planetaire", "Camera automatique"},
                {"6. Mode educatif", "Quiz interactifs"}
            };

            for (size_t i = 0; i < features.size(); ++i) {
                // Puce colorée
                sf::CircleShape bullet(4.f);
                bullet.setPosition(cardX + 20.f, contentY + i * 28.f + 5.f);
                bullet.setFillColor(cardColors[c]);
                window.draw(bullet);

                sf::Text fname(features[i].first, font, 13);
                fname.setPosition(cardX + 35.f, contentY + i * 28.f);
                fname.setFillColor(sf::Color(255, 255, 200, 255));
                fname.setStyle(sf::Text::Bold);
                window.draw(fname);

                sf::Text fdesc(features[i].second, font, 12);
                fdesc.setPosition(cardX + 250.f, contentY + i * 28.f);
                fdesc.setFillColor(sf::Color(180, 230, 200, 255));
                window.draw(fdesc);
            }
        }
        else if (c == 2) {
            // TECHNIQUES
            std::vector<std::pair<std::string, std::string>> techniques = {
                {"Lois de Kepler", "Equations elliptiques"},
                {"Integration numerique", "Calculs haute precision"},
                {"Gravitation Newton", "Force reelle simulee"},
                {"Coord. heliocentriques", "Reference solaire"},
                {"Vitesses orbitales", "Calcul dynamique"},
                {"Gestion collisions", "Detection impacts"}
            };

            for (size_t i = 0; i < techniques.size(); ++i) {
                // Icône +
                sf::Text plusIcon("+", font, 16);
                plusIcon.setPosition(cardX + 18.f, contentY + i * 28.f - 2.f);
                plusIcon.setFillColor(cardColors[c]);
                plusIcon.setStyle(sf::Text::Bold);
                window.draw(plusIcon);

                sf::Text tname(techniques[i].first, font, 13);
                tname.setPosition(cardX + 38.f, contentY + i * 28.f);
                tname.setFillColor(sf::Color(255, 230, 180, 255));
                tname.setStyle(sf::Text::Bold);
                window.draw(tname);

                sf::Text tdesc(techniques[i].second, font, 12);
                tdesc.setPosition(cardX + 230.f, contentY + i * 28.f);
                tdesc.setFillColor(sf::Color(255, 210, 160, 255));
                window.draw(tdesc);
            }
        }
        else if (c == 3) {
            // TECHNOLOGIES
            std::vector<std::tuple<std::string, std::string, std::string>> techs = {
                {"C++ 17", "Langage", "Standard moderne"},
                {"SFML 2.5", "Graphique", "Moteur 2D performant"},
                {"OpenGL", "GPU", "Acceleration materielle"},
                {"CMake", "Build", "Cross-plateforme"},
                {"Git", "Version", "Controle sources"},
                {"VS 2022", "IDE", "Environnement dev"}
            };

            for (size_t i = 0; i < techs.size(); ++i) {
                // Badge technologie
                sf::RectangleShape badge(sf::Vector2f(80.f, 22.f));
                badge.setPosition(cardX + 15.f, contentY + i * 28.f - 2.f);
                badge.setFillColor(sf::Color(cardColors[c].r, cardColors[c].g, cardColors[c].b, 80));
                badge.setOutlineThickness(1.f);
                badge.setOutlineColor(sf::Color(cardColors[c].r, cardColors[c].g, cardColors[c].b, 150));
                window.draw(badge);

                sf::Text techName(std::get<0>(techs[i]), font, 12);
                techName.setPosition(cardX + 55.f, contentY + i * 28.f);
                techName.setOrigin(techName.getLocalBounds().width / 2.f, 0.f);
                techName.setFillColor(sf::Color(255, 255, 255, 255));
                techName.setStyle(sf::Text::Bold);
                window.draw(techName);

                sf::Text techType(std::get<1>(techs[i]), font, 11);
                techType.setPosition(cardX + 110.f, contentY + i * 28.f);
                techType.setFillColor(sf::Color(150, 200, 255, 255));
                window.draw(techType);

                sf::Text techDesc(std::get<2>(techs[i]), font, 11);
                techDesc.setPosition(cardX + 200.f, contentY + i * 28.f);
                techDesc.setFillColor(sf::Color(180, 220, 255, 255));
                window.draw(techDesc);
            }
        }
    }

    // ===== PIED DE PAGE =====
    // Barre de progression décorative
    float footerY = frameY + frameH - 55.f;
    sf::RectangleShape progressBg(sf::Vector2f(frameW - 40.f, 6.f));
    progressBg.setPosition(frameX + 20.f, footerY);
    progressBg.setFillColor(sf::Color(40, 60, 100, 200));
    window.draw(progressBg);

    // Animation de la barre
    float progressWidth = (frameW - 40.f) * (0.5f + 0.5f * std::sin(time * 0.5f));
    sf::RectangleShape progressFill(sf::Vector2f(progressWidth, 6.f));
    progressFill.setPosition(frameX + 20.f, footerY);
    progressFill.setFillColor(sf::Color(100, 200, 255, 200));
    window.draw(progressFill);

    // Point lumineux sur la barre
    sf::CircleShape progressDot(5.f);
    progressDot.setOrigin(5.f, 5.f);
    progressDot.setPosition(frameX + 20.f + progressWidth, footerY + 3.f);
    progressDot.setFillColor(sf::Color(200, 240, 255, 255));
    window.draw(progressDot);

    // Texte du footer
    sf::Text footer("Developpe avec C++17 | SFML 2.5 | CMake | Universite Ibn Tofail 2025", font, 12);
    footer.setPosition(600.f, footerY + 18.f);
    footer.setOrigin(footer.getLocalBounds().width / 2.f, 0.f);
    footer.setFillColor(sf::Color(150, 180, 220, 220));
    window.draw(footer);

    // ===== BOUTON RETOUR MODERNE =====
    for (auto& b : backButtons) {
        sf::Vector2f btnPos = b.base.getPosition();
        sf::Vector2f btnSize = b.base.getSize();

        // Lueur au survol
        if (b.hover) {
            for (int g = 3; g >= 0; --g) {
                sf::RectangleShape hoverGlow(sf::Vector2f(btnSize.x + g * 8.f, btnSize.y + g * 8.f));
                hoverGlow.setPosition(btnPos.x - g * 4.f, btnPos.y - g * 4.f);
                hoverGlow.setFillColor(sf::Color(255, 100, 100, static_cast<sf::Uint8>(40 - g * 10)));
                window.draw(hoverGlow);
            }
        }

        // Fond dégradé du bouton
        sf::RectangleShape btnTop(sf::Vector2f(btnSize.x, btnSize.y / 2.f));
        btnTop.setPosition(btnPos);
        btnTop.setFillColor(b.hover ? sf::Color(255, 80, 80, 255) : sf::Color(200, 50, 50, 240));
        window.draw(btnTop);

        sf::RectangleShape btnBottom(sf::Vector2f(btnSize.x, btnSize.y / 2.f));
        btnBottom.setPosition(btnPos.x, btnPos.y + btnSize.y / 2.f);
        btnBottom.setFillColor(b.hover ? sf::Color(220, 60, 60, 255) : sf::Color(160, 40, 40, 240));
        window.draw(btnBottom);

        // Bordure
        sf::RectangleShape btnBorder(btnSize);
        btnBorder.setPosition(btnPos);
        btnBorder.setFillColor(sf::Color::Transparent);
        btnBorder.setOutlineThickness(2.f);
        btnBorder.setOutlineColor(b.hover ? sf::Color(255, 200, 200) : sf::Color(255, 120, 120));
        window.draw(btnBorder);

        // Texte
        sf::Text btnText("RETOUR", font, 14);
        btnText.setPosition(btnPos.x + btnSize.x / 2.f, btnPos.y + btnSize.y / 2.f - 8.f);
        btnText.setOrigin(btnText.getLocalBounds().width / 2.f, 0.f);
        btnText.setFillColor(sf::Color::White);
        btnText.setStyle(sf::Text::Bold);
        window.draw(btnText);
    }
}



void drawTeamInfo(sf::RenderWindow& window, const sf::Font& font,
    std::vector<CosmicButton>& backButtons,
    std::vector<Star>& stars, float time) {

    // ===== FOND NOIR PROFOND AVEC DÉGRADÉ BLEU NUIT =====
    for (int i = 0; i < 10; ++i) {
        float ratio = static_cast<float>(i) / 10.f;
        sf::RectangleShape bgLayer(sf::Vector2f(1200.f, 80.f));
        bgLayer.setPosition(0.f, i * 80.f);

        sf::Uint8 r = static_cast<sf::Uint8>(5 + 10 * (1.f - ratio));
        sf::Uint8 g = static_cast<sf::Uint8>(10 + 20 * (1.f - ratio));
        sf::Uint8 b = static_cast<sf::Uint8>(25 + 40 * (1.f - ratio));
        bgLayer.setFillColor(sf::Color(r, g, b, 255));
        window.draw(bgLayer);
    }

    // ===== ÉTOILES SUBTILES =====
    for (auto& s : stars) {
        s.draw(window);
    }

    // ===== PARTICULES FLOTTANTES =====
    for (int p = 0; p < 15; ++p) {
        float px = 100.f + (p * 67) % 1000;
        float py = 80.f + (p * 83) % 620;
        float moveX = 2.f * std::sin(time * 0.2f + p * 0.4f);
        float moveY = 1.5f * std::cos(time * 0.15f + p * 0.3f);
        float alpha = 25.f + 15.f * std::sin(time * 0.6f + p * 0.2f);

        sf::CircleShape particle(1.2f);
        particle.setPosition(px + moveX, py + moveY);
        particle.setFillColor(sf::Color(120, 180, 255, static_cast<sf::Uint8>(alpha)));
        window.draw(particle);
    }

    // ===== ICÔNE ORBITE STYLISÉE EN HAUT CENTRE =====
    float orbitCenterX = 600.f;
    float orbitCenterY = 45.f;

    for (int o = 0; o < 3; ++o) {
        float orbitRadius = 18.f + o * 10.f;
        sf::CircleShape orbit(orbitRadius);
        orbit.setOrigin(orbitRadius, orbitRadius);
        orbit.setPosition(orbitCenterX, orbitCenterY);
        orbit.setFillColor(sf::Color::Transparent);
        orbit.setOutlineThickness(1.f);
        orbit.setOutlineColor(sf::Color(80, 160, 255, static_cast<sf::Uint8>(45 - o * 10)));
        window.draw(orbit);

        float planetAngle = time * (0.7f - o * 0.15f) + o * 2.0f;
        sf::CircleShape planet(2.f - o * 0.4f);
        planet.setOrigin(2.f - o * 0.4f, 2.f - o * 0.4f);
        planet.setPosition(orbitCenterX + orbitRadius * std::cos(planetAngle),
            orbitCenterY + orbitRadius * std::sin(planetAngle));
        planet.setFillColor(sf::Color(130, 200, 255, 180));
        window.draw(planet);
    }

    sf::CircleShape centralSun(4.f);
    centralSun.setOrigin(4.f, 4.f);
    centralSun.setPosition(orbitCenterX, orbitCenterY);
    centralSun.setFillColor(sf::Color(255, 220, 100, 255));
    window.draw(centralSun);

    // ===== TITRE PRINCIPAL - DÉCALÉ VERS LA GAUCHE =====
    sf::Text mainTitle("EQUIPE DE DEVELOPPEMENT", font, 38);
    sf::FloatRect titleBounds = mainTitle.getLocalBounds();
    mainTitle.setOrigin(titleBounds.width / 2.f, 0.f);
    mainTitle.setPosition(520.f, 75.f);  // 
    mainTitle.setFillColor(sf::Color(255, 255, 255, 255));
    mainTitle.setStyle(sf::Text::Bold);
    mainTitle.setLetterSpacing(2.5f);
    mainTitle.setOutlineThickness(2.f);
    mainTitle.setOutlineColor(sf::Color(40, 80, 140, 200));
    window.draw(mainTitle);

    // Sous-titre - AUSSI DÉCALÉ VERS LA GAUCHE
    sf::Text projectName("COSMOUIT - Simulateur Solaire 2D", font, 16);
    sf::FloatRect subBounds = projectName.getLocalBounds();
    projectName.setOrigin(subBounds.width / 2.f, 0.f);
    projectName.setPosition(580.f, 120.f); 
    projectName.setFillColor(sf::Color(255, 200, 100, 230));
    projectName.setLetterSpacing(2.f);
    window.draw(projectName);

    // Ligne séparatrice sous le titre - AUSSI DÉCALÉE
    sf::RectangleShape titleSep(sf::Vector2f(450.f, 2.f));
    titleSep.setPosition(390.f, 145.f);  
    titleSep.setFillColor(sf::Color(80, 150, 220, 120));
    window.draw(titleSep);

    // ===== POSITIONS CENTRÉES =====
  

    float totalWidth = 1200.f;
    float contentWidth = 950.f; 
    float startX = (totalWidth - contentWidth) / 2.f; 

    // ===== COLONNE GAUCHE : MEMBRES EN VERTICAL - CENTRÉE =====
    float leftColX = startX; 
    float leftColWidth = 450.f;
    float membersStartY = 170.f;

    // Titre section membres - CENTRÉ
    sf::Text membersTitle("MEMBRES DE L'EQUIPE", font, 16);
    sf::FloatRect memTitleBounds = membersTitle.getLocalBounds();
    membersTitle.setOrigin(memTitleBounds.width / 2.f, 0.f);
    membersTitle.setPosition(leftColX + leftColWidth / 2.f, membersStartY);
    membersTitle.setFillColor(sf::Color(100, 200, 255, 240));
    membersTitle.setStyle(sf::Text::Bold);
    membersTitle.setLetterSpacing(2.f);
    window.draw(membersTitle);

    // Ligne sous le titre - CENTRÉE
    sf::RectangleShape membersTitleLine(sf::Vector2f(leftColWidth - 40.f, 1.f));
    membersTitleLine.setPosition(leftColX + 20.f, membersStartY + 24.f);
    membersTitleLine.setFillColor(sf::Color(100, 200, 255, 100));
    window.draw(membersTitleLine);

    std::vector<std::string> teamMembers = {
        "SARHANE AYMANE",
        "MIDINI HAFSA",
        "EL GHAYATI SARA",
        "SANGARE NFALY",
        "BAJOUDI ALAE"
        
    };

    float cardH = 50.f;
    float cardSpacing = 10.f;
    float cardsStartY = membersStartY + 40.f;

    for (size_t i = 0; i < teamMembers.size(); ++i) {
        float cardY = cardsStartY + i * (cardH + cardSpacing);

        // Fond de la carte
        sf::RectangleShape cardBg(sf::Vector2f(leftColWidth, cardH));
        cardBg.setPosition(leftColX, cardY);
        cardBg.setFillColor(sf::Color(12, 25, 45, 245));
        window.draw(cardBg);

        // Bordure cyan lumineuse
        sf::RectangleShape cardBorder(sf::Vector2f(leftColWidth, cardH));
        cardBorder.setPosition(leftColX, cardY);
        cardBorder.setFillColor(sf::Color::Transparent);
        cardBorder.setOutlineThickness(1.5f);
        float borderAlpha = 140.f + 40.f * std::sin(time * 1.2f + i * 0.6f);
        cardBorder.setOutlineColor(sf::Color(80, 180, 255, static_cast<sf::Uint8>(borderAlpha)));
        window.draw(cardBorder);

        // Barre latérale gauche colorée
        sf::RectangleShape leftBar(sf::Vector2f(4.f, cardH));
        leftBar.setPosition(leftColX, cardY);
        leftBar.setFillColor(sf::Color(100, 200, 255, 255));
        window.draw(leftBar);

        // Numéro du membre
        sf::CircleShape numCircle(13.f);
        numCircle.setPosition(leftColX + 18.f, cardY + 12.f);
        numCircle.setFillColor(sf::Color(60, 130, 200, 200));
        numCircle.setOutlineThickness(2.f);
        numCircle.setOutlineColor(sf::Color(100, 200, 255, 200));
        window.draw(numCircle);

        sf::Text numText(std::to_string(i + 1), font, 15);
        numText.setPosition(leftColX + 31.f, cardY + 16.f);
        numText.setOrigin(numText.getLocalBounds().width / 2.f, 0.f);
        numText.setFillColor(sf::Color(255, 255, 255, 255));
        numText.setStyle(sf::Text::Bold);
        window.draw(numText);

        // Nom du membre
        sf::Text memberName(teamMembers[i], font, 18);
        memberName.setPosition(leftColX + 60.f, cardY + 14.f);
        memberName.setFillColor(sf::Color(240, 250, 255, 255));
        memberName.setStyle(sf::Text::Bold);
        memberName.setLetterSpacing(1.3f);
        window.draw(memberName);
    }

    
    float rightColX = startX + leftColWidth + 50.f;  
    float rightColWidth = 450.f;
    float rightStartY = 170.f;

    // === ENCADREMENT ACADÉMIQUE - CENTRÉ ===
    sf::Text encTitle("ENCADREMENT ACADEMIQUE", font, 16);
    sf::FloatRect encTitleBounds = encTitle.getLocalBounds();
    encTitle.setOrigin(encTitleBounds.width / 2.f, 0.f);
    encTitle.setPosition(rightColX + rightColWidth / 2.f, rightStartY);
    encTitle.setFillColor(sf::Color(255, 200, 100, 240));
    encTitle.setStyle(sf::Text::Bold);
    encTitle.setLetterSpacing(2.f);
    window.draw(encTitle);

    // Ligne sous le titre - CENTRÉE
    sf::RectangleShape encTitleLine(sf::Vector2f(rightColWidth - 40.f, 1.f));
    encTitleLine.setPosition(rightColX + 20.f, rightStartY + 24.f);
    encTitleLine.setFillColor(sf::Color(255, 200, 100, 100));
    window.draw(encTitleLine);

    // Carte encadrement
    float encCardY = rightStartY + 40.f;
    float encCardH = 85.f;

    sf::RectangleShape encBg(sf::Vector2f(rightColWidth, encCardH));
    encBg.setPosition(rightColX, encCardY);
    encBg.setFillColor(sf::Color(12, 25, 45, 245));
    window.draw(encBg);

    sf::RectangleShape encBorder(sf::Vector2f(rightColWidth, encCardH));
    encBorder.setPosition(rightColX, encCardY);
    encBorder.setFillColor(sf::Color::Transparent);
    encBorder.setOutlineThickness(1.5f);
    float encBorderAlpha = 140.f + 40.f * std::sin(time * 1.0f);
    encBorder.setOutlineColor(sf::Color(255, 180, 80, static_cast<sf::Uint8>(encBorderAlpha)));
    window.draw(encBorder);

    // Barre latérale dorée
    sf::RectangleShape encLeftBar(sf::Vector2f(4.f, encCardH));
    encLeftBar.setPosition(rightColX, encCardY);
    encLeftBar.setFillColor(sf::Color(255, 200, 100, 255));
    window.draw(encLeftBar);

    // Nom du professeur - CENTRÉ
    sf::Text profName("Pr. Mohamed DAOUDI", font, 22);
    sf::FloatRect profBounds = profName.getLocalBounds();
    profName.setOrigin(profBounds.width / 2.f, 0.f);
    profName.setPosition(rightColX + rightColWidth / 2.f, encCardY + 15.f);
    profName.setFillColor(sf::Color(255, 255, 255, 255));
    profName.setStyle(sf::Text::Bold);
    window.draw(profName);

    sf::Text profInfo("Professeur Encadrant Principal", font, 12);
    sf::FloatRect profInfoBounds = profInfo.getLocalBounds();
    profInfo.setOrigin(profInfoBounds.width / 2.f, 0.f);
    profInfo.setPosition(rightColX + rightColWidth / 2.f, encCardY + 42.f);
    profInfo.setFillColor(sf::Color(200, 220, 240, 220));
    window.draw(profInfo);

    sf::Text deptInfo("Departement d'Informatique - Universite Ibn Tofail", font, 10);
    sf::FloatRect deptBounds = deptInfo.getLocalBounds();
    deptInfo.setOrigin(deptBounds.width / 2.f, 0.f);
    deptInfo.setPosition(rightColX + rightColWidth / 2.f, encCardY + 62.f);
    deptInfo.setFillColor(sf::Color(160, 190, 220, 200));
    window.draw(deptInfo);

    // === INFORMATIONS DU PROJET - CENTRÉ ===
    float infoY = encCardY + encCardH + 25.f;

    sf::Text infoTitle("INFORMATIONS DU PROJET", font, 16);
    sf::FloatRect infoTitleBounds = infoTitle.getLocalBounds();
    infoTitle.setOrigin(infoTitleBounds.width / 2.f, 0.f);
    infoTitle.setPosition(rightColX + rightColWidth / 2.f, infoY);
    infoTitle.setFillColor(sf::Color(100, 220, 180, 240));
    infoTitle.setStyle(sf::Text::Bold);
    infoTitle.setLetterSpacing(2.f);
    window.draw(infoTitle);

    // Ligne sous le titre - CENTRÉE
    sf::RectangleShape infoTitleLine(sf::Vector2f(rightColWidth - 40.f, 1.f));
    infoTitleLine.setPosition(rightColX + 20.f, infoY + 24.f);
    infoTitleLine.setFillColor(sf::Color(100, 220, 180, 100));
    window.draw(infoTitleLine);

    // Carte infos projet
    float infoCardY = infoY + 40.f;
    float infoCardH = 145.f;

    sf::RectangleShape infoBg(sf::Vector2f(rightColWidth, infoCardH));
    infoBg.setPosition(rightColX, infoCardY);
    infoBg.setFillColor(sf::Color(12, 25, 45, 245));
    window.draw(infoBg);

    sf::RectangleShape infoBorder(sf::Vector2f(rightColWidth, infoCardH));
    infoBorder.setPosition(rightColX, infoCardY);
    infoBorder.setFillColor(sf::Color::Transparent);
    infoBorder.setOutlineThickness(1.5f);
    infoBorder.setOutlineColor(sf::Color(100, 220, 180, 150));
    window.draw(infoBorder);

    // Barre latérale verte
    sf::RectangleShape infoLeftBar(sf::Vector2f(4.f, infoCardH));
    infoLeftBar.setPosition(rightColX, infoCardY);
    infoLeftBar.setFillColor(sf::Color(100, 220, 180, 255));
    window.draw(infoLeftBar);

    std::vector<std::pair<std::string, std::string>> projectInfo = {
        {"Nom", "CosmoUIT - Simulateur Solaire 2D"},
        {"Version", "2.0 Release Finale"},
        {"Duree", "2 mois"},
        {"Langage", "C++ 17 avec SFML"},
        {"Annee", "2025/2026"}
    };

    float infoLineY = infoCardY + 12.f;
    for (size_t i = 0; i < projectInfo.size(); ++i) {
        // Fond alterné
        if (i % 2 == 0) {
            sf::RectangleShape lineBg(sf::Vector2f(rightColWidth - 20.f, 24.f));
            lineBg.setPosition(rightColX + 10.f, infoLineY - 2.f);
            lineBg.setFillColor(sf::Color(30, 50, 80, 100));
            window.draw(lineBg);
        }

        // Label
        sf::Text label(projectInfo[i].first + " :", font, 13);
        label.setPosition(rightColX + 20.f, infoLineY);
        label.setFillColor(sf::Color(150, 200, 220, 220));
        label.setStyle(sf::Text::Bold);
        window.draw(label);

        // Valeur
        sf::Text value(projectInfo[i].second, font, 13);
        value.setPosition(rightColX + 120.f, infoLineY);
        value.setFillColor(sf::Color(240, 250, 255, 255));
        window.draw(value);

        infoLineY += 26.f;
    }

    // ===== SECTION CITATION - EN BAS, CENTRÉE =====
    float quoteBoxY = 540.f;
    float quoteBoxH = 95.f;
    float quoteBoxW = 900.f;
    float quoteBoxX = (1200.f - quoteBoxW) / 2.f; 

    // Fond pour la section citation
    sf::RectangleShape quoteBackground(sf::Vector2f(quoteBoxW, quoteBoxH));
    quoteBackground.setPosition(quoteBoxX, quoteBoxY);
    quoteBackground.setFillColor(sf::Color(8, 15, 30, 230));
    window.draw(quoteBackground);

    // Séparateur AVANT citation - VISIBLE ET CENTRÉ
    float sepWidth = 380.f;
    float sepGap = 50.f;

    sf::RectangleShape sepLine1(sf::Vector2f(sepWidth, 2.f));
    sepLine1.setPosition(quoteBoxX, quoteBoxY + 8.f);
    sepLine1.setFillColor(sf::Color(100, 180, 255, 180));
    window.draw(sepLine1);

    sf::RectangleShape sepLine2(sf::Vector2f(sepWidth, 2.f));
    sepLine2.setPosition(quoteBoxX + quoteBoxW - sepWidth, quoteBoxY + 8.f);
    sepLine2.setFillColor(sf::Color(100, 180, 255, 180));
    window.draw(sepLine2);

    // Étoile centrale décorative
    sf::CircleShape starDecor(6.f);
    starDecor.setOrigin(6.f, 6.f);
    starDecor.setPosition(600.f, quoteBoxY + 9.f);
    starDecor.setFillColor(sf::Color(255, 220, 100, 255));
    window.draw(starDecor);

    // Citation avec effet typewriter
    std::string fullQuote = "L'astronomie nous apprend a regarder au-dela de nous-memes";
    static float typewriterProgress = 0.f;
    typewriterProgress += 0.025f;

    int charsToShow = static_cast<int>(typewriterProgress * 2.f) % (static_cast<int>(fullQuote.length()) + 50);
    if (charsToShow > static_cast<int>(fullQuote.length())) {
        charsToShow = static_cast<int>(fullQuote.length());
    }
    std::string visibleQuote = fullQuote.substr(0, charsToShow);

    // Guillemet décoratif
    sf::Text quoteOpen("\"", font, 45);
    quoteOpen.setPosition(quoteBoxX + 30.f, quoteBoxY + 18.f);
    quoteOpen.setFillColor(sf::Color(100, 180, 255, 100));
    window.draw(quoteOpen);

    // Citation - CENTRÉE
    sf::Text quoteText(visibleQuote, font, 22);
    sf::FloatRect quoteBounds = quoteText.getLocalBounds();
    quoteText.setOrigin(quoteBounds.width / 2.f, 0.f);
    quoteText.setPosition(600.f, quoteBoxY + 30.f);
    quoteText.setFillColor(sf::Color(220, 235, 250, 255));
    quoteText.setStyle(sf::Text::Italic);
    quoteText.setLetterSpacing(1.1f);
    window.draw(quoteText);

    // Curseur clignotant
    if (charsToShow < static_cast<int>(fullQuote.length())) {
        float cursorAlpha = 200.f * (0.5f + 0.5f * std::sin(time * 10.f));
        sf::RectangleShape cursor(sf::Vector2f(2.f, 24.f));
        cursor.setPosition(600.f + quoteBounds.width / 2.f + 5.f, quoteBoxY + 30.f);
        cursor.setFillColor(sf::Color(100, 200, 255, static_cast<sf::Uint8>(cursorAlpha)));
        window.draw(cursor);
    }

    // Auteur - CENTRÉ
    sf::Text authorText("- Carl Sagan", font, 15);
    sf::FloatRect authorBounds = authorText.getLocalBounds();
    authorText.setOrigin(authorBounds.width / 2.f, 0.f);
    authorText.setPosition(600.f, quoteBoxY + 62.f);
    authorText.setFillColor(sf::Color(180, 200, 220, 220));
    authorText.setStyle(sf::Text::Italic);
    window.draw(authorText);

    // Séparateur APRÈS citation
    sf::RectangleShape sepLine3(sf::Vector2f(quoteBoxW - 40.f, 1.f));
    sepLine3.setPosition(quoteBoxX + 20.f, quoteBoxY + quoteBoxH - 5.f);
    sepLine3.setFillColor(sf::Color(100, 180, 255, 120));
    window.draw(sepLine3);

    // ===== FOOTER - CENTRÉ =====
    sf::Text footerText("Projet Universitaire - Master Educative - 2025", font, 11);
    sf::FloatRect footerBounds = footerText.getLocalBounds();
    footerText.setOrigin(footerBounds.width / 2.f, 0.f);
    footerText.setPosition(600.f, 655.f);
    footerText.setFillColor(sf::Color(120, 150, 180, 180));
    window.draw(footerText);

    // ===== BOUTON RETOUR - EN BAS À DROITE =====
    for (auto& b : backButtons) {
        b.base.setPosition(1040.f, 650.f);
        b.base.setSize(sf::Vector2f(130.f, 42.f));

        sf::Vector2f btnPos = b.base.getPosition();
        sf::Vector2f btnSize = b.base.getSize();

        // Effet hover lumineux
        if (b.hover) {
            sf::RectangleShape hoverGlow(sf::Vector2f(btnSize.x + 8.f, btnSize.y + 8.f));
            hoverGlow.setPosition(btnPos.x - 4.f, btnPos.y - 4.f);
            hoverGlow.setFillColor(sf::Color(255, 180, 100, 50));
            window.draw(hoverGlow);
        }

        // Fond du bouton avec dégradé
        sf::RectangleShape btnTop(sf::Vector2f(btnSize.x, btnSize.y / 2.f));
        btnTop.setPosition(btnPos);
        btnTop.setFillColor(b.hover ? sf::Color(255, 120, 80, 255) : sf::Color(200, 80, 60, 245));
        window.draw(btnTop);

        sf::RectangleShape btnBottom(sf::Vector2f(btnSize.x, btnSize.y / 2.f));
        btnBottom.setPosition(btnPos.x, btnPos.y + btnSize.y / 2.f);
        btnBottom.setFillColor(b.hover ? sf::Color(220, 90, 70, 255) : sf::Color(160, 60, 45, 245));
        window.draw(btnBottom);

        // Bordure
        sf::RectangleShape btnBorder(btnSize);
        btnBorder.setPosition(btnPos);
        btnBorder.setFillColor(sf::Color::Transparent);
        btnBorder.setOutlineThickness(2.f);
        btnBorder.setOutlineColor(b.hover ? sf::Color(255, 220, 200, 255) : sf::Color(255, 150, 130, 200));
        window.draw(btnBorder);

        // Texte du bouton - CENTRÉ
        sf::Text btnText("RETOUR", font, 16);
        sf::FloatRect btnTextBounds = btnText.getLocalBounds();
        btnText.setOrigin(btnTextBounds.width / 2.f, 0.f);
        btnText.setPosition(btnPos.x + btnSize.x / 2.f, btnPos.y + btnSize.y / 2.f - 10.f);
        btnText.setFillColor(sf::Color::White);
        btnText.setStyle(sf::Text::Bold);
        btnText.setLetterSpacing(1.5f);
        window.draw(btnText);
    }
}




void drawPlanetDetails(sf::RenderWindow& window, const sf::Font& font,
    std::vector<CosmicButton>& backButtons,
    std::vector<Star>& stars, float time,
    const Planet& planet) {

    // ===== FOND SPATIAL ANIMÉ =====
    static float bgAnim = 0.f;
    bgAnim += 0.008f;

    // Dégradé de fond dynamique
    for (int i = 0; i < 8; ++i) {
        float ratio = static_cast<float>(i) / 8.f;
        sf::RectangleShape bgLayer(sf::Vector2f(1200.f, 100.f));
        bgLayer.setPosition(0.f, i * 100.f);

        sf::Uint8 r = static_cast<sf::Uint8>(15 + 25 * ratio);
        sf::Uint8 g = static_cast<sf::Uint8>(25 + 40 * ratio);
        sf::Uint8 b = static_cast<sf::Uint8>(50 + 60 * ratio);
        bgLayer.setFillColor(sf::Color(r, g, b, 255));
        window.draw(bgLayer);
    }

    // Fond étoilé animé
    for (auto& s : stars) {
        s.draw(window);
    }

    // Nébuleuses colorées selon la planète
    for (int n = 0; n < 3; ++n) {
        float nebulaX = 200.f + n * 400.f + 30.f * std::sin(bgAnim + n);
        float nebulaY = 150.f + n * 200.f + 20.f * std::cos(bgAnim * 0.8f + n);
        float nebulaAlpha = 25.f + 15.f * std::sin(bgAnim * 1.5f + n);

        sf::CircleShape nebula(200.f - n * 40.f);
        nebula.setPosition(nebulaX, nebulaY);
        nebula.setFillColor(sf::Color(
            planet.color.r, planet.color.g, planet.color.b,
            static_cast<sf::Uint8>(nebulaAlpha)));
        window.draw(nebula);
    }

    // ===== PANNEAU PRINCIPAL AVEC EFFET VERRE =====
    sf::RectangleShape panelShadow(sf::Vector2f(1110.f, 660.f));
    panelShadow.setPosition(55.f, 80.f);
    panelShadow.setFillColor(sf::Color(0, 0, 0, 80));
    window.draw(panelShadow);

    for (int i = 0; i < 12; ++i) {
        float ratio = static_cast<float>(i) / 12.f;
        sf::RectangleShape panelLayer(sf::Vector2f(1100.f, 54.f));
        panelLayer.setPosition(50.f, 75.f + i * 54.f);

        sf::Uint8 r = static_cast<sf::Uint8>(30 + 20 * ratio);
        sf::Uint8 g = static_cast<sf::Uint8>(45 + 30 * ratio);
        sf::Uint8 b = static_cast<sf::Uint8>(75 + 40 * ratio);
        panelLayer.setFillColor(sf::Color(r, g, b, 235));
        window.draw(panelLayer);
    }

    sf::RectangleShape panelBorder(sf::Vector2f(1100.f, 650.f));
    panelBorder.setPosition(50.f, 75.f);
    panelBorder.setFillColor(sf::Color::Transparent);
    float borderPulse = 2.f + 1.f * std::sin(time * 3.f);
    panelBorder.setOutlineThickness(borderPulse);
    float borderAlpha = 180.f + 75.f * std::sin(time * 2.f);
    panelBorder.setOutlineColor(sf::Color(
        planet.color.r, planet.color.g, planet.color.b,
        static_cast<sf::Uint8>(borderAlpha)));
    window.draw(panelBorder);

    // ===== EN-TÊTE =====
    for (int i = 0; i < 5; ++i) {
        float ratio = static_cast<float>(i) / 5.f;
        sf::RectangleShape headerLayer(sf::Vector2f(1100.f, 22.f));
        headerLayer.setPosition(50.f, 75.f + i * 22.f);

        sf::Uint8 r = static_cast<sf::Uint8>(planet.color.r * (0.6f - 0.15f * ratio));
        sf::Uint8 g = static_cast<sf::Uint8>(planet.color.g * (0.6f - 0.15f * ratio));
        sf::Uint8 b = static_cast<sf::Uint8>(planet.color.b * (0.6f - 0.15f * ratio));
        headerLayer.setFillColor(sf::Color(r, g, b, 230));
        window.draw(headerLayer);
    }

    sf::RectangleShape headerGlow(sf::Vector2f(1100.f, 4.f));
    headerGlow.setPosition(50.f, 183.f);
    float glowAlpha = 200.f + 55.f * std::sin(time * 4.f);
    headerGlow.setFillColor(sf::Color(
        std::min(255, planet.color.r + 80),
        std::min(255, planet.color.g + 80),
        std::min(255, planet.color.b + 80),
        static_cast<sf::Uint8>(glowAlpha)));
    window.draw(headerGlow);

    // ===== TITRE =====
    sf::Text titleGlow(planet.name, font, 52);
    titleGlow.setPosition(600.f, 100.f);
    titleGlow.setOrigin(titleGlow.getLocalBounds().width / 2.f, 0.f);
    float titleGlowAlpha = 60.f + 40.f * std::sin(time * 2.f);
    titleGlow.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(titleGlowAlpha)));
    titleGlow.setStyle(sf::Text::Bold);
    window.draw(titleGlow);

    sf::Text titleShadow(planet.name, font, 48);
    titleShadow.setPosition(604.f, 104.f);
    titleShadow.setOrigin(titleShadow.getLocalBounds().width / 2.f, 0.f);
    titleShadow.setFillColor(sf::Color(0, 0, 0, 150));
    titleShadow.setStyle(sf::Text::Bold);
    window.draw(titleShadow);

    sf::Text title(planet.name, font, 48);
    title.setPosition(600.f, 100.f);
    title.setOrigin(title.getLocalBounds().width / 2.f, 0.f);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    title.setOutlineThickness(2.f);
    title.setOutlineColor(planet.color);
    window.draw(title);

    float subColorShift = std::sin(time * 1.5f);
    sf::Uint8 subR = static_cast<sf::Uint8>(planet.color.r + 50 * subColorShift);
    sf::Uint8 subG = static_cast<sf::Uint8>(planet.color.g + 50 * subColorShift);
    sf::Uint8 subB = static_cast<sf::Uint8>(planet.color.b + 50 * subColorShift);

    sf::Text subtitle(planet.planetType, font, 22);
    subtitle.setPosition(600.f, 158.f);
    subtitle.setOrigin(subtitle.getLocalBounds().width / 2.f, 0.f);
    subtitle.setFillColor(sf::Color(subR, subG, subB, 255));
    subtitle.setStyle(sf::Text::Italic);
    window.draw(subtitle);

    // ===== PLANÈTE À GAUCHE =====
    float leftX = 250.f;
    float planetY = 370.f;

    for (int o = 0; o < 3; ++o) {
        float orbitRadius = 90.f + o * 25.f;
        float orbitAlpha = 60.f - o * 15.f + 20.f * std::sin(time * 2.f + o);

        sf::CircleShape orbit(orbitRadius);
        orbit.setOrigin(orbitRadius, orbitRadius);
        orbit.setPosition(leftX, planetY);
        orbit.setFillColor(sf::Color::Transparent);
        orbit.setOutlineThickness(1.5f);
        orbit.setOutlineColor(sf::Color(
            planet.color.r, planet.color.g, planet.color.b,
            static_cast<sf::Uint8>(orbitAlpha)));
        window.draw(orbit);
    }

    sf::CircleShape outerGlow(95.f);
    outerGlow.setOrigin(95.f, 95.f);
    outerGlow.setPosition(leftX, planetY);
    float outerGlowAlpha = 40.f + 20.f * std::sin(time * 3.f);
    outerGlow.setFillColor(sf::Color(
        planet.color.r, planet.color.g, planet.color.b,
        static_cast<sf::Uint8>(outerGlowAlpha)));
    window.draw(outerGlow);

    if (planet.hasTexture()) {
        sf::Sprite planetSprite;
        planetSprite.setTexture(*planet.getTexture());

        sf::FloatRect bounds = planetSprite.getLocalBounds();
        float scale = 150.f / std::max(bounds.width, bounds.height);
        planetSprite.setScale(scale, scale);
        planetSprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        planetSprite.setPosition(leftX, planetY);
        planetSprite.setRotation(time * 15.f);
        window.draw(planetSprite);

        sf::CircleShape planetBorder(76.f);
        planetBorder.setOrigin(76.f, 76.f);
        planetBorder.setPosition(leftX, planetY);
        planetBorder.setFillColor(sf::Color::Transparent);
        planetBorder.setOutlineThickness(3.f);
        float borderBrightness = 150.f + 100.f * std::sin(time * 2.5f);
        planetBorder.setOutlineColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(borderBrightness)));
        window.draw(planetBorder);
    }
    else {
        sf::CircleShape planetVisual(75.f);
        planetVisual.setOrigin(75.f, 75.f);
        planetVisual.setPosition(leftX, planetY);
        planetVisual.setFillColor(planet.color);
        planetVisual.setOutlineThickness(4.f);
        planetVisual.setOutlineColor(sf::Color(255, 255, 255, 200));
        window.draw(planetVisual);
    }

    sf::CircleShape highlight(30.f);
    highlight.setOrigin(30.f, 30.f);
    highlight.setPosition(leftX - 25.f, planetY - 30.f);
    highlight.setFillColor(sf::Color(255, 255, 255, 80));
    window.draw(highlight);

    // ===== FAIT MARQUANT =====
    for (int i = 0; i < 4; ++i) {
        float ratio = static_cast<float>(i) / 4.f;
        sf::RectangleShape factLayer(sf::Vector2f(340.f, 30.f));
        factLayer.setPosition(80.f, 510.f + i * 30.f);
        factLayer.setFillColor(sf::Color(
            static_cast<sf::Uint8>(40 + 20 * ratio),
            static_cast<sf::Uint8>(55 + 25 * ratio),
            static_cast<sf::Uint8>(90 + 30 * ratio), 230));
        window.draw(factLayer);
    }

    sf::RectangleShape factPanel(sf::Vector2f(340.f, 120.f));
    factPanel.setPosition(80.f, 510.f);
    factPanel.setFillColor(sf::Color::Transparent);
    factPanel.setOutlineThickness(2.f);
    factPanel.setOutlineColor(sf::Color(255, 220, 100, 200));
    window.draw(factPanel);

    sf::RectangleShape factTopBar(sf::Vector2f(340.f, 4.f));
    factTopBar.setPosition(80.f, 510.f);
    factTopBar.setFillColor(sf::Color(255, 220, 100, 255));
    window.draw(factTopBar);

    sf::Text starIcon("*", font, 28);
    starIcon.setPosition(100.f, 520.f);
    starIcon.setFillColor(sf::Color(255, 220, 100));
    window.draw(starIcon);

    sf::Text factTitle("FAIT MARQUANT", font, 16);
    factTitle.setPosition(130.f, 525.f);
    factTitle.setFillColor(sf::Color(255, 240, 150));
    factTitle.setStyle(sf::Text::Bold);
    window.draw(factTitle);

    std::string factText = planet.fact;
    if (factText.length() > 35) {
        size_t spacePos = factText.find(' ', 30);
        if (spacePos != std::string::npos && spacePos < factText.length()) {
            factText.insert(spacePos + 1, "\n");
        }
    }

    sf::Text factContent(factText, font, 14);
    factContent.setPosition(100.f, 555.f);
    factContent.setFillColor(sf::Color(220, 240, 255));
    factContent.setStyle(sf::Text::Italic);
    window.draw(factContent);

    // ===== CARACTÉRISTIQUES =====
    float rightColumnX = 460.f;
    float currentY = 210.f;

    sf::RectangleShape charTitleBg(sf::Vector2f(600.f, 35.f));
    charTitleBg.setPosition(rightColumnX, currentY);
    charTitleBg.setFillColor(sf::Color(60, 90, 140, 200));
    window.draw(charTitleBg);

    sf::Text charTitle("CARACTERISTIQUES", font, 22);
    charTitle.setPosition(rightColumnX + 15.f, currentY + 5.f);
    charTitle.setFillColor(sf::Color(150, 230, 255));
    charTitle.setStyle(sf::Text::Bold);
    window.draw(charTitle);

    currentY += 50.f;

    auto planetInfo = planet.getFormattedInfo();

    std::vector<sf::Color> rowColors = {
        sf::Color(70, 100, 150, 200),
        sf::Color(80, 115, 170, 200)
    };

    for (size_t i = 0; i < planetInfo.size(); ++i) {
        const auto& info = planetInfo[i];

        float pairWidth = 290.f;
        float xOffset = (i % 2 == 0) ? 0.f : pairWidth + 15.f;

        sf::RectangleShape infoBox(sf::Vector2f(pairWidth, 42.f));
        infoBox.setPosition(rightColumnX + xOffset, currentY);
        infoBox.setFillColor(rowColors[i % 2]);
        infoBox.setOutlineThickness(1.f);
        infoBox.setOutlineColor(sf::Color(100, 150, 200, 150));
        window.draw(infoBox);

        sf::RectangleShape leftBar(sf::Vector2f(4.f, 42.f));
        leftBar.setPosition(rightColumnX + xOffset, currentY);
        leftBar.setFillColor(planet.color);
        window.draw(leftBar);

        sf::Text label(info.first + ":", font, 14);
        label.setPosition(rightColumnX + xOffset + 12.f, currentY + 12.f);
        label.setFillColor(sf::Color(180, 210, 255));
        label.setStyle(sf::Text::Bold);
        window.draw(label);

        sf::Text value(info.second, font, 14);
        value.setPosition(rightColumnX + xOffset + 120.f, currentY + 12.f);
        value.setFillColor(sf::Color(255, 255, 200));
        window.draw(value);

        if (i % 2 == 1 || i == planetInfo.size() - 1) {
            currentY += 50.f;
        }
    }

    // ===== COMPOSITION =====
    currentY += 15.f;

    sf::RectangleShape compTitleBg(sf::Vector2f(600.f, 35.f));
    compTitleBg.setPosition(rightColumnX, currentY);
    compTitleBg.setFillColor(sf::Color(80, 60, 40, 200));
    window.draw(compTitleBg);

    sf::Text compTitle("COMPOSITION ATMOSPHERIQUE", font, 20);
    compTitle.setPosition(rightColumnX + 15.f, currentY + 6.f);
    compTitle.setFillColor(sf::Color(255, 200, 120));
    compTitle.setStyle(sf::Text::Bold);
    window.draw(compTitle);

    currentY += 45.f;

    for (int i = 0; i < 3; ++i) {
        float ratio = static_cast<float>(i) / 3.f;
        sf::RectangleShape compLayer(sf::Vector2f(600.f, 25.f));
        compLayer.setPosition(rightColumnX, currentY + i * 25.f);
        compLayer.setFillColor(sf::Color(
            static_cast<sf::Uint8>(50 + 20 * ratio),
            static_cast<sf::Uint8>(45 + 15 * ratio),
            static_cast<sf::Uint8>(70 + 20 * ratio), 220));
        window.draw(compLayer);
    }

    sf::RectangleShape compBorder(sf::Vector2f(600.f, 75.f));
    compBorder.setPosition(rightColumnX, currentY);
    compBorder.setFillColor(sf::Color::Transparent);
    compBorder.setOutlineThickness(2.f);
    compBorder.setOutlineColor(sf::Color(200, 160, 100, 180));
    window.draw(compBorder);

    sf::Text compText(planet.composition, font, 15);
    compText.setPosition(rightColumnX + 20.f, currentY + 28.f);
    compText.setFillColor(sf::Color(240, 230, 200));
    compText.setStyle(sf::Text::Italic);
    window.draw(compText);

    // ===== BARRE DÉCORATIVE =====
    sf::RectangleShape bottomBar(sf::Vector2f(1000.f, 4.f));
    bottomBar.setPosition(100.f, 655.f);
    float barAlpha = 150.f + 100.f * std::sin(time * 2.f);
    bottomBar.setFillColor(sf::Color(
        planet.color.r, planet.color.g, planet.color.b,
        static_cast<sf::Uint8>(barAlpha)));
    window.draw(bottomBar);

    sf::Text educationalNote("Donnees scientifiques basees sur les observations astronomiques modernes", font, 12);
    educationalNote.setPosition(600.f, 670.f);
    educationalNote.setOrigin(educationalNote.getLocalBounds().width / 2.f, 0.f);
    educationalNote.setFillColor(sf::Color(160, 180, 210));
    educationalNote.setStyle(sf::Text::Italic);
    window.draw(educationalNote);

    // ===== BOUTON RETOUR - CORRIGÉ EN BAS À DROITE =====
    for (auto& b : backButtons) {
        //  POSITION FIXE EN BAS À DROITE
        b.base.setPosition(1020.f, 680.f);
        b.base.setSize(sf::Vector2f(120.f, 40.f));

        sf::Vector2f btnPos = b.base.getPosition();
        sf::Vector2f btnSize = b.base.getSize();

        // Récupérer position souris
        sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

        // Vérifier le survol
        sf::FloatRect btnBounds(btnPos.x, btnPos.y, btnSize.x, btnSize.y);
        bool isHovered = btnBounds.contains(mousePos);
        b.hover = isHovered;

        //  EFFET HOVER LUMINEUX
        if (b.hover) {
            sf::RectangleShape hoverGlow(sf::Vector2f(btnSize.x + 10.f, btnSize.y + 10.f));
            hoverGlow.setPosition(btnPos.x - 5.f, btnPos.y - 5.f);
            hoverGlow.setFillColor(sf::Color(255, 100, 100, 60));
            window.draw(hoverGlow);
        }

        //  FOND DU BOUTON AVEC DÉGRADÉ
        sf::RectangleShape btnTop(sf::Vector2f(btnSize.x, btnSize.y / 2.f));
        btnTop.setPosition(btnPos);
        btnTop.setFillColor(b.hover ? sf::Color(255, 100, 80, 255) : sf::Color(200, 60, 50, 245));
        window.draw(btnTop);

        sf::RectangleShape btnBottom(sf::Vector2f(btnSize.x, btnSize.y / 2.f));
        btnBottom.setPosition(btnPos.x, btnPos.y + btnSize.y / 2.f);
        btnBottom.setFillColor(b.hover ? sf::Color(220, 80, 60, 255) : sf::Color(160, 50, 40, 245));
        window.draw(btnBottom);

        //  BORDURE
        sf::RectangleShape btnBorder(btnSize);
        btnBorder.setPosition(btnPos);
        btnBorder.setFillColor(sf::Color::Transparent);
        btnBorder.setOutlineThickness(2.f);
        btnBorder.setOutlineColor(b.hover ? sf::Color(255, 200, 180, 255) : sf::Color(255, 130, 110, 200));
        window.draw(btnBorder);

        //  TEXTE DU BOUTON - CENTRÉ
        sf::Text btnText("RETOUR", font, 15);
        sf::FloatRect btnTextBounds = btnText.getLocalBounds();
        btnText.setOrigin(btnTextBounds.width / 2.f, btnTextBounds.height / 2.f);
        btnText.setPosition(btnPos.x + btnSize.x / 2.f, btnPos.y + btnSize.y / 2.f - 2.f);
        btnText.setFillColor(sf::Color::White);
        btnText.setStyle(sf::Text::Bold);
        window.draw(btnText);
    }
}

void drawSimulation(sf::RenderWindow& window, const sf::Font& font,
    sf::View& worldView, sf::View& uiView,
    std::vector<Star>& stars, float time,
    const sf::CircleShape& sun, const sf::CircleShape& sunGlow,
    std::vector<Planet>& planets,
    std::vector<class Comet>& comets,
    std::vector<CosmicButton>& simButtons,
    CosmicButton& toolsMainButton,
    bool toolMenuOpen,
    float toolMenuAnimation,
    const sf::Text& infoText, const sf::Text& speedText,
    bool showOrbits, bool showTrails, bool showLabels,
    bool showStats, bool showGrid,
    const sf::RectangleShape& statsPanel,
    const sf::Text& statsTitle,
    const sf::Texture& saturnRingTexture,
    bool cameraFollowMode,
    int followedPlanetIndex,
    MissionTimeline* missionTimeline,
    const SpaceMission* hoveredMission,
    bool showMissionPanel,
    EducationalMode* educationalMode) {

    window.setView(worldView);

    //  Calculer la zone visible en fonction du zoom
    sf::Vector2f viewCenter = worldView.getCenter();
    sf::Vector2f viewSize = worldView.getSize();

    // Zone visible de la caméra
    float viewLeft = viewCenter.x - viewSize.x / 2.f;
    float viewRight = viewCenter.x + viewSize.x / 2.f;
    float viewTop = viewCenter.y - viewSize.y / 2.f;
    float viewBottom = viewCenter.y + viewSize.y / 2.f;

    //  Étoiles - DESSINÉES PARTOUT DANS LA ZONE VISIBLE
    for (auto& s : stars) {
        s.draw(window);
    }

    //  Grille - ADAPTÉE AU ZOOM ET À LA POSITION DE LA CAMÉRA
    if (showGrid) {
        // Calculer la taille de la grille en fonction du zoom
        float gridSpacing = 100.f;

        // Trouver les indices de départ et de fin pour la grille
        int startX = static_cast<int>(std::floor(viewLeft / gridSpacing)) - 5;
        int endX = static_cast<int>(std::ceil(viewRight / gridSpacing)) + 5;
        int startY = static_cast<int>(std::floor(viewTop / gridSpacing)) - 5;
        int endY = static_cast<int>(std::ceil(viewBottom / gridSpacing)) + 5;

        //  LIGNES VERTICALES
        for (int i = startX; i <= endX; ++i) {
            sf::VertexArray vLine(sf::Lines, 2);
            float x = i * gridSpacing;
            vLine[0].position = sf::Vector2f(x, viewTop - 500.f);
            vLine[1].position = sf::Vector2f(x, viewBottom + 500.f);
            vLine[0].color = vLine[1].color = sf::Color(50, 50, 50, 100);
            window.draw(vLine);
        }

        //  LIGNES HORIZONTALES
        for (int i = startY; i <= endY; ++i) {
            sf::VertexArray hLine(sf::Lines, 2);
            float y = i * gridSpacing;
            hLine[0].position = sf::Vector2f(viewLeft - 500.f, y);
            hLine[1].position = sf::Vector2f(viewRight + 500.f, y);
            hLine[0].color = hLine[1].color = sf::Color(50, 50, 50, 100);
            window.draw(hLine);
        }
    }

    // Orbites
    if (showOrbits) {
        for (const auto& p : planets) {
            sf::VertexArray orbit(sf::LineStrip, 121);
            for (int k = 0; k <= 120; ++k) {
                float t = 6.2831853f * k / 120.f;
                float r = p.semiMajorAxis * (1 - p.eccentricity * p.eccentricity) /
                    (1 + p.eccentricity * std::cos(t));
                orbit[k].position = sf::Vector2f(
                    Constants::SUN_CENTER.x + r * std::cos(t),
                    Constants::SUN_CENTER.y + r * std::sin(t)
                );
                orbit[k].color = sf::Color(p.color.r / 2, p.color.g / 2, p.color.b / 2, 100);
            }
            window.draw(orbit);
        }
    }

    // Soleil
    window.draw(sunGlow);
    window.draw(sun);

    // Planètes et leurs lunes
    for (auto& p : planets) {
        p.draw(window, showTrails, showLabels);
        if (p.getName() == "Saturne") {
            p.drawRings(window, saturnRingTexture);
        }

        if (p.hasMoons()) {
            p.drawMoons(window, showLabels);
        }
    }

    // Comètes
    for (auto& c : comets) {
        c.draw(window, showTrails);
    }

    // Dessiner les marqueurs de missions SI le panneau est ouvert
    if (showMissionPanel && missionTimeline != nullptr) {
        window.setView(worldView);  // Vue monde pour les marqueurs
        missionTimeline->drawMissionMarkers(window, planets, Constants::SUN_CENTER);
    }

    // Panneau d'info planète suivie
    if (cameraFollowMode && followedPlanetIndex >= 0 &&
        followedPlanetIndex < static_cast<int>(planets.size())) {

        const auto& planet = planets[followedPlanetIndex];
        sf::Vector2f planetPos = planet.shape.getPosition();
        float planetRadius = planet.shape.getRadius();

        float distToSun = std::hypot(planetPos.x - Constants::SUN_CENTER.x,
            planetPos.y - Constants::SUN_CENTER.y);
        float speed = std::hypot(planet.velocity.x, planet.velocity.y);
        float angleRad = std::atan2(planetPos.y - Constants::SUN_CENTER.y,
            planetPos.x - Constants::SUN_CENTER.x);
        float angleDeg = angleRad * 180.f / 3.14159f;
        if (angleDeg < 0) angleDeg += 360.f;

        float pulseScale = 1.0f + 0.25f * std::sin(time * 5.f);
        float indicatorRadius = (planetRadius + 25.f) * pulseScale;

        sf::CircleShape followCircle(indicatorRadius);
        followCircle.setOrigin(indicatorRadius, indicatorRadius);
        followCircle.setPosition(planetPos);
        followCircle.setFillColor(sf::Color::Transparent);
        followCircle.setOutlineThickness(4.f);
        followCircle.setOutlineColor(sf::Color(0, 255, 100, 255));
        window.draw(followCircle);

        for (int i = 0; i < 4; ++i) {
            float angle = (i * 90.f + time * 60.f) * 3.14159f / 180.f;
            float arrowDist = indicatorRadius + 15.f;

            sf::ConvexShape arrow;
            arrow.setPointCount(3);
            arrow.setPoint(0, sf::Vector2f(0, -10));
            arrow.setPoint(1, sf::Vector2f(-7, 10));
            arrow.setPoint(2, sf::Vector2f(7, 10));
            arrow.setPosition(planetPos.x + arrowDist * std::cos(angle),
                planetPos.y + arrowDist * std::sin(angle));
            arrow.setRotation(angle * 180.f / 3.14159f + 90.f);
            arrow.setFillColor(sf::Color(0, 255, 100, 255));
            window.draw(arrow);
        }

        float panelW = 160.f;
        float panelH = 120.f;
        float panelOffsetX = planetRadius + 60.f;
        sf::Vector2f panelPos(planetPos.x + panelOffsetX, planetPos.y - panelH / 2.f);

        sf::VertexArray connectorLine(sf::Lines, 2);
        connectorLine[0].position = sf::Vector2f(planetPos.x + planetRadius + 5.f, planetPos.y);
        connectorLine[0].color = sf::Color(0, 255, 100, 180);
        connectorLine[1].position = sf::Vector2f(panelPos.x, panelPos.y + panelH / 2.f);
        connectorLine[1].color = sf::Color(0, 255, 100, 255);
        window.draw(connectorLine);

        sf::RectangleShape infoPanel(sf::Vector2f(panelW, panelH));
        infoPanel.setPosition(panelPos);
        infoPanel.setFillColor(sf::Color(0, 30, 60, 240));
        infoPanel.setOutlineThickness(3.f);
        infoPanel.setOutlineColor(sf::Color(0, 255, 100, 255));
        window.draw(infoPanel);

        sf::RectangleShape topBar(sf::Vector2f(panelW, 5.f));
        topBar.setPosition(panelPos);
        topBar.setFillColor(planet.color);
        window.draw(topBar);

        sf::Text planetName(planet.name, font, 14);
        planetName.setPosition(panelPos.x + 8.f, panelPos.y + 10.f);
        planetName.setFillColor(sf::Color(255, 255, 0));
        planetName.setStyle(sf::Text::Bold);
        window.draw(planetName);

        float dataY = panelPos.y + 32.f;
        float lineH = 20.f;

        std::ostringstream distS;
        distS << std::fixed << std::setprecision(0) << distToSun << " u";
        sf::Text distL("Dist: ", font, 10);
        distL.setPosition(panelPos.x + 8.f, dataY);
        distL.setFillColor(sf::Color(180, 200, 220));
        window.draw(distL);
        sf::Text distV(distS.str(), font, 11);
        distV.setPosition(panelPos.x + 50.f, dataY);
        distV.setFillColor(sf::Color(255, 180, 0));
        distV.setStyle(sf::Text::Bold);
        window.draw(distV);

        dataY += lineH;
        std::ostringstream speedS;
        speedS << std::fixed << std::setprecision(2) << speed << " u/s";
        sf::Text speedL("Vit: ", font, 10);
        speedL.setPosition(panelPos.x + 8.f, dataY);
        speedL.setFillColor(sf::Color(180, 200, 220));
        window.draw(speedL);
        sf::Color spdCol = speed > 1.5f ? sf::Color(255, 50, 50) :
            speed > 0.8f ? sf::Color(255, 200, 0) : sf::Color(0, 255, 100);
        sf::Text speedV(speedS.str(), font, 11);
        speedV.setPosition(panelPos.x + 50.f, dataY);
        speedV.setFillColor(spdCol);
        speedV.setStyle(sf::Text::Bold);
        window.draw(speedV);

        dataY += lineH;
        std::ostringstream angleS;
        angleS << std::fixed << std::setprecision(0) << angleDeg << " deg";
        sf::Text angleL("Pos: ", font, 10);
        angleL.setPosition(panelPos.x + 8.f, dataY);
        angleL.setFillColor(sf::Color(180, 200, 220));
        window.draw(angleL);
        sf::Text angleV(angleS.str(), font, 11);
        angleV.setPosition(panelPos.x + 50.f, dataY);
        angleV.setFillColor(sf::Color(0, 220, 255));
        angleV.setStyle(sf::Text::Bold);
        window.draw(angleV);

        float mX = panelPos.x + panelW - 28.f;
        float mY = panelPos.y + panelH - 28.f;
        sf::CircleShape mOrb(18.f);
        mOrb.setOrigin(18.f, 18.f);
        mOrb.setPosition(mX, mY);
        mOrb.setFillColor(sf::Color::Transparent);
        mOrb.setOutlineThickness(1.f);
        mOrb.setOutlineColor(sf::Color(80, 120, 180, 150));
        window.draw(mOrb);

        sf::CircleShape mSun(3.f);
        mSun.setOrigin(3.f, 3.f);
        mSun.setPosition(mX, mY);
        mSun.setFillColor(sf::Color(255, 200, 50));
        window.draw(mSun);

        sf::CircleShape mPlanet(4.f);
        mPlanet.setOrigin(4.f, 4.f);
        mPlanet.setPosition(mX + 18.f * std::cos(angleRad), mY + 18.f * std::sin(angleRad));
        mPlanet.setFillColor(planet.color);
        mPlanet.setOutlineThickness(1.f);
        mPlanet.setOutlineColor(sf::Color::White);
        window.draw(mPlanet);
    }

    // ===== UI VIEW =====
    window.setView(uiView);
    window.draw(infoText);
    window.draw(speedText);

   

    // ===== MENU OUTILS MODERNE =====
    static float toolsAnimTime = 0.f;
    toolsAnimTime += 0.016f;

    // Bouton principal OUTILS avec design moderne
    float mainBtnX = 1055.f;
    float mainBtnY = 110.f;
    float mainBtnW = 140.f;
    float mainBtnH = 45.f;

    // Fond dégradé du bouton principal
    for (int i = 0; i < 3; ++i) {
        float ratio = static_cast<float>(i) / 3.f;
        sf::RectangleShape mainBtnLayer(sf::Vector2f(mainBtnW, mainBtnH / 3.f));
        mainBtnLayer.setPosition(mainBtnX, mainBtnY + i * mainBtnH / 3.f);
        
        sf::Uint8 r = static_cast<sf::Uint8>(80 + 40 * (1.f - ratio));
        sf::Uint8 g = static_cast<sf::Uint8>(60 + 30 * (1.f - ratio));
        sf::Uint8 b = static_cast<sf::Uint8>(140 + 40 * (1.f - ratio));
        mainBtnLayer.setFillColor(sf::Color(r, g, b, 240));
        window.draw(mainBtnLayer);
    }

    // Bordure lumineuse animée
    sf::RectangleShape mainBtnBorder(sf::Vector2f(mainBtnW, mainBtnH));
    mainBtnBorder.setPosition(mainBtnX, mainBtnY);
    mainBtnBorder.setFillColor(sf::Color::Transparent);
    mainBtnBorder.setOutlineThickness(2.f);
    float borderPulse = 180.f + 75.f * std::sin(toolsAnimTime * 3.f);
    mainBtnBorder.setOutlineColor(sf::Color(200, 150, 255, static_cast<sf::Uint8>(borderPulse)));
    window.draw(mainBtnBorder);

    // Icône engrenage animée
    float gearRotation = toolsAnimTime * 30.f;
    sf::Text gearIcon("@", font, 22);  
    gearIcon.setPosition(mainBtnX + 18.f, mainBtnY + 10.f);
    gearIcon.setFillColor(sf::Color(255, 220, 150));
    gearIcon.setRotation(toolMenuOpen ? gearRotation : 0.f);
    window.draw(gearIcon);

    // Texte OUTILS
    sf::Text toolsLabel("OUTILS", font, 16);
    toolsLabel.setPosition(mainBtnX + 45.f, mainBtnY + 13.f);
    toolsLabel.setFillColor(sf::Color::White);
    toolsLabel.setStyle(sf::Text::Bold);
    window.draw(toolsLabel);

    // Indicateur d'ouverture
    sf::Text arrowIndicator(toolMenuOpen ? "-" : "+", font, 20);
    arrowIndicator.setPosition(mainBtnX + mainBtnW - 25.f, mainBtnY + 10.f);
    arrowIndicator.setFillColor(sf::Color(200, 255, 200));
    arrowIndicator.setStyle(sf::Text::Bold);
    window.draw(arrowIndicator);

    // ===== MENU DÉROULANT =====
    if (toolMenuAnimation > 0.01f) {
        float menuX = mainBtnX - 5.f;
        float menuY = mainBtnY + mainBtnH + 8.f;
        float menuW = 150.f;
        float btnH = 32.f;  
        float btnSpacing = 3.f;  

        //  LA HAUTEUR DU MENU À L'ÉCRAN
        float maxMenuHeight = 580.f; 
        float totalHeight = simButtons.size() * (btnH + btnSpacing) + 15.f;
        float menuHeight = std::min(totalHeight, maxMenuHeight);
        float animatedHeight = menuHeight * toolMenuAnimation;

        // Fond du menu avec effet verre
        sf::RectangleShape menuShadow(sf::Vector2f(menuW + 6.f, animatedHeight + 6.f));
        menuShadow.setPosition(menuX + 3.f, menuY + 3.f);
        menuShadow.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(60 * toolMenuAnimation)));
        window.draw(menuShadow);

        // Dégradé du fond
        for (int i = 0; i < 8; ++i) {
            float ratio = static_cast<float>(i) / 8.f;
            float layerH = animatedHeight / 8.f;
            sf::RectangleShape menuLayer(sf::Vector2f(menuW, layerH));
            menuLayer.setPosition(menuX, menuY + i * layerH);

            sf::Uint8 r = static_cast<sf::Uint8>(25 + 15 * ratio);
            sf::Uint8 g = static_cast<sf::Uint8>(35 + 20 * ratio);
            sf::Uint8 b = static_cast<sf::Uint8>(60 + 30 * ratio);
            menuLayer.setFillColor(sf::Color(r, g, b, static_cast<sf::Uint8>(245 * toolMenuAnimation)));
            window.draw(menuLayer);
        }

        // Bordure du menu
        sf::RectangleShape menuBorder(sf::Vector2f(menuW, animatedHeight));
        menuBorder.setPosition(menuX, menuY);
        menuBorder.setFillColor(sf::Color::Transparent);
        menuBorder.setOutlineThickness(2.f);
        menuBorder.setOutlineColor(sf::Color(100, 150, 220, static_cast<sf::Uint8>(200 * toolMenuAnimation)));
        window.draw(menuBorder);

        // Ligne lumineuse en haut
        sf::RectangleShape topGlow(sf::Vector2f(menuW, 3.f));
        topGlow.setPosition(menuX, menuY);
        float glowAlpha = (150.f + 100.f * std::sin(toolsAnimTime * 4.f)) * toolMenuAnimation;
        topGlow.setFillColor(sf::Color(150, 200, 255, static_cast<sf::Uint8>(glowAlpha)));
        window.draw(topGlow);

        // ===== BOUTONS COMPACTS =====
              
        struct ButtonStyle {
            sf::Color bgColor;
            sf::Color iconColor;
            sf::Color hoverColor;
            sf::Color textColor;
        };

        //  COULEURS VIVES ET CLAIRES
        std::vector<ButtonStyle> buttonStyles = {
            // Accélérer - Vert émeraude lumineux
            {{40, 180, 120, 245}, {20, 255, 140}, {60, 220, 150, 255}, {255, 255, 255}},
            // Ralentir - Bleu ciel brillant
            {{50, 150, 220, 245}, {80, 200, 255}, {70, 180, 245, 255}, {255, 255, 255}},
            // Pause - Violet royal
            {{130, 90, 200, 245}, {180, 140, 255}, {160, 120, 230, 255}, {255, 255, 255}},
            // Zoom + - Orange soleil
            {{240, 150, 50, 245}, {255, 200, 80}, {255, 180, 80, 255}, {40, 40, 40}},
            // Zoom - - Corail chaud
            {{230, 100, 100, 245}, {255, 140, 140}, {250, 130, 130, 255}, {255, 255, 255}},
            // Orbites - Cyan électrique
            {{40, 180, 200, 245}, {80, 230, 255}, {60, 210, 230, 255}, {255, 255, 255}},
            // Traînées - Magenta vif
            {{180, 80, 180, 245}, {230, 130, 230}, {210, 110, 210, 255}, {255, 255, 255}},
            // Labels - Or brillant
            {{220, 180, 50, 245}, {255, 230, 100}, {245, 210, 80, 255}, {40, 40, 40}},
            // Grille - Argent moderne
            {{120, 140, 170, 245}, {180, 200, 230}, {150, 170, 200, 255}, {255, 255, 255}},
            // Effacer - Rouge rubis
            {{200, 60, 80, 245}, {255, 100, 120}, {230, 80, 100, 255}, {255, 255, 255}},
            // Reset - Vert lime
            {{100, 190, 80, 245}, {150, 240, 120}, {130, 220, 110, 255}, {40, 40, 40}},
            // Menu - Bleu marine
            {{70, 100, 160, 245}, {120, 160, 220}, {100, 140, 200, 255}, {255, 255, 255}},
            // Suivre - Turquoise brillant
            {{40, 200, 180, 245}, {80, 255, 230}, {60, 230, 210, 255}, {40, 40, 40}},
            // Missions - Rose fuchsia
            {{220, 80, 150, 245}, {255, 130, 190}, {245, 110, 175, 255}, {255, 255, 255}},
            // Quiz - Indigo profond
            {{100, 80, 200, 245}, {150, 130, 255}, {130, 110, 230, 255}, {255, 255, 255}},
            // Compare - Bleu azur
            {{60, 140, 220, 245}, {100, 190, 255}, {80, 170, 245, 255}, {255, 255, 255}}
        };

        std::vector<std::string> buttonIcons = {
            ">>", "<<", "||", "+", "-", "O", "~", "T", "#", "X", "R", "M", "S", "H", "Q", "P"
        };

        std::vector<std::string> buttonLabels = {
            "Accelerer", "Ralentir", "Pause", "Zoom +", "Zoom -",
            "Orbites", "Trainees", "Labels", "Grille", "Effacer",
            "Reset", "Menu", "Suivre", "Missions", "Quiz", "Compare"
        };

        float currentY = menuY + 8.f;

        for (size_t i = 0; i < simButtons.size() && i < buttonStyles.size(); ++i) {
            float buttonDelay = static_cast<float>(i) * 0.02f;
            float buttonAlpha = std::max(0.f, std::min(1.f, (toolMenuAnimation - buttonDelay) * 4.f));

            if (buttonAlpha > 0.01f && currentY + btnH < menuY + animatedHeight) {
                float btnX = menuX + 4.f;
                float btnW = menuW - 8.f;

                //  Mettre à jour la position RÉELLE du bouton
                simButtons[i].base.setPosition(btnX, currentY);
                simButtons[i].base.setSize(sf::Vector2f(btnW, btnH));

                auto& btn = simButtons[i];
                const auto& style = buttonStyles[i];
                bool isHovered = btn.hover;

                float hoverOffset = isHovered ? -2.f : 0.f;
                float hoverScale = isHovered ? 1.02f : 1.f;

                //  OMBRE 3D pour effet de profondeur
                if (isHovered) {
                    sf::RectangleShape shadow(sf::Vector2f(btnW, btnH));
                    shadow.setPosition(btnX + 3.f, currentY + 3.f);
                    shadow.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(80 * buttonAlpha)));
                    window.draw(shadow);
                }

                //  FOND DÉGRADÉ VERTICAL (2 couches)
                sf::Color bgColor = isHovered ? style.hoverColor : style.bgColor;

                // Couche supérieure (plus claire)
                sf::RectangleShape btnTop(sf::Vector2f(btnW * hoverScale, btnH / 2.f));
                btnTop.setPosition(btnX + hoverOffset, currentY);
                sf::Uint8 topR = static_cast<sf::Uint8>(std::min(255, bgColor.r + 30));
                sf::Uint8 topG = static_cast<sf::Uint8>(std::min(255, bgColor.g + 30));
                sf::Uint8 topB = static_cast<sf::Uint8>(std::min(255, bgColor.b + 30));
                btnTop.setFillColor(sf::Color(topR, topG, topB, static_cast<sf::Uint8>(bgColor.a * buttonAlpha)));
                window.draw(btnTop);

                // Couche inférieure (couleur de base)
                sf::RectangleShape btnBottom(sf::Vector2f(btnW * hoverScale, btnH / 2.f));
                btnBottom.setPosition(btnX + hoverOffset, currentY + btnH / 2.f);
                btnBottom.setFillColor(sf::Color(bgColor.r, bgColor.g, bgColor.b,
                    static_cast<sf::Uint8>(bgColor.a * buttonAlpha)));
                window.draw(btnBottom);

                //  BORDURE LUMINEUSE
                sf::RectangleShape btnBorder(sf::Vector2f(btnW * hoverScale, btnH));
                btnBorder.setPosition(btnX + hoverOffset, currentY);
                btnBorder.setFillColor(sf::Color::Transparent);
                btnBorder.setOutlineThickness(isHovered ? 2.5f : 1.5f);

                // Bordure plus claire au survol
                sf::Uint8 borderAlpha = static_cast<sf::Uint8>((isHovered ? 255 : 180) * buttonAlpha);
                btnBorder.setOutlineColor(sf::Color(
                    style.iconColor.r, style.iconColor.g, style.iconColor.b, borderAlpha));
                window.draw(btnBorder);

                // BARRE LATÉRALE COLORÉE (accent visuel)
                sf::RectangleShape leftBar(sf::Vector2f(4.f, btnH));
                leftBar.setPosition(btnX + hoverOffset, currentY);
                leftBar.setFillColor(sf::Color(style.iconColor.r, style.iconColor.g,
                    style.iconColor.b, static_cast<sf::Uint8>(255 * buttonAlpha)));
                window.draw(leftBar);

                //  CERCLE D'ICÔNE AVEC EFFET 3D
                float circleRadius = 12.f;

                // Ombre du cercle
                sf::CircleShape iconShadow(circleRadius);
                iconShadow.setPosition(btnX + 10.f + hoverOffset, currentY + 5.f);
                iconShadow.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(60 * buttonAlpha)));
                window.draw(iconShadow);

                // Cercle principal
                sf::CircleShape iconCircle(circleRadius);
                iconCircle.setPosition(btnX + 8.f + hoverOffset, currentY + 4.f);
                iconCircle.setFillColor(sf::Color(style.iconColor.r, style.iconColor.g,
                    style.iconColor.b, static_cast<sf::Uint8>(255 * buttonAlpha)));
                iconCircle.setOutlineThickness(2.f);
                iconCircle.setOutlineColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(200 * buttonAlpha)));
                window.draw(iconCircle);

                // Reflet sur le cercle
                sf::CircleShape iconHighlight(5.f);
                iconHighlight.setPosition(btnX + 10.f + hoverOffset, currentY + 6.f);
                iconHighlight.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(100 * buttonAlpha)));
                window.draw(iconHighlight);

                //  ICÔNE TEXTE (dans le cercle)
                if (i < buttonIcons.size()) {
                    sf::Text iconText(buttonIcons[i], font, 11);
                    iconText.setPosition(btnX + 8.f + circleRadius + hoverOffset, currentY + 8.f);
                    iconText.setOrigin(iconText.getLocalBounds().width / 2.f, 0.f);
                    iconText.setFillColor(sf::Color(30, 30, 40, static_cast<sf::Uint8>(255 * buttonAlpha)));
                    iconText.setStyle(sf::Text::Bold);
                    window.draw(iconText);
                }

                //  LABEL TEXTE - COULEUR ADAPTÉE
                if (i < buttonLabels.size()) {
                    sf::Text labelText(buttonLabels[i], font, 12);
                    labelText.setPosition(btnX + 40.f + hoverOffset, currentY + 8.f);
                    labelText.setFillColor(sf::Color(
                        style.textColor.r, style.textColor.g, style.textColor.b,
                        static_cast<sf::Uint8>(255 * buttonAlpha)));
                    labelText.setStyle(sf::Text::Bold);

                    // Ombre légère pour le texte
                    if (isHovered) {
                        labelText.setOutlineThickness(1.f);
                        labelText.setOutlineColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(100 * buttonAlpha)));
                    }
                    window.draw(labelText);
                }

                //  EFFET DE BRILLANCE AU SURVOL
                if (isHovered) {
                    float shinePos = std::fmod(toolsAnimTime * 80.f, btnW + 40.f) - 20.f;
                    sf::RectangleShape shine(sf::Vector2f(20.f, btnH));
                    shine.setPosition(btnX + shinePos + hoverOffset, currentY);

                    // Dégradé de brillance
                    shine.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(40 * buttonAlpha)));
                    window.draw(shine);
                }

                currentY += btnH + btnSpacing;
            }
        }
    }

    // Mettre à jour le bouton principal (pour la détection de clic)
    toolsMainButton.base.setPosition(mainBtnX, mainBtnY);
    toolsMainButton.base.setSize(sf::Vector2f(mainBtnW, mainBtnH));

    toolsMainButton.draw(window);

    // Badge de suivi en haut
    if (cameraFollowMode && followedPlanetIndex >= 0 &&
        followedPlanetIndex < static_cast<int>(planets.size())) {
        const auto& planet = planets[followedPlanetIndex];

        sf::RectangleShape badge(sf::Vector2f(140.f, 30.f));
        badge.setPosition(20.f, 50.f);
        badge.setFillColor(sf::Color(0, 60, 30, 230));
        badge.setOutlineThickness(2.f);
        badge.setOutlineColor(sf::Color(0, 255, 100));
        window.draw(badge);

        sf::Text badgeText("SUIVI: " + planet.name, font, 11);
        badgeText.setPosition(30.f, 56.f);
        badgeText.setFillColor(sf::Color(0, 255, 100));
        badgeText.setStyle(sf::Text::Bold);
        window.draw(badgeText);
    }

    // Stats
    if (showStats) {
        window.draw(statsPanel);
        window.draw(statsTitle);
        float yPos = 60.f;
        for (size_t i = 0; i < planets.size(); ++i) {
            const auto& p = planets[i];
            sf::CircleShape dot(5.f);
            dot.setPosition(895.f, yPos + 5.f);
            dot.setFillColor(p.color);
            window.draw(dot);
            std::ostringstream oss;
            oss << p.getName() << " (" << p.numMoons << " lunes)";
            sf::Text stat(oss.str(), font, 13);
            stat.setPosition(910.f, yPos);
            stat.setFillColor(sf::Color::White);
            window.draw(stat);
            yPos += 58.f;
        }
    }


    // ===== PANNEAU D'INFO MISSION =====
    if (showMissionPanel && hoveredMission != nullptr && missionTimeline != nullptr) {
        //  Obtenir la taille de la vue UI
        sf::Vector2f uiSize = uiView.getSize();

       
        float panelX = uiSize.x - 320.f; 
        float panelY = 200.f;

        sf::Vector2f panelPos(panelX, panelY);
        missionTimeline->drawMissionInfo(window, font, *hoveredMission, panelPos);
    }
    //  le quiz éducatif
    if (educationalMode && educationalMode->isQuizActive()) {
        educationalMode->drawQuiz(window);
    }
}
// ===== TEXTURES POUR LA STRUCTURE INTERNE =====
static std::vector<sf::Texture> structureTextures(8);
static bool structureTexturesLoaded = false;

static void loadStructureTextures() {
    if (structureTexturesLoaded) return;

    std::vector<std::string> textureFiles = {
        "textures/structure/mercury_structure.png",
        "textures/structure/venus_structure.png",
        "textures/structure/earth_structure.png",
        "textures/structure/mars_structure.png",
        "textures/structure/jupiter_structure.png",
        "textures/structure/saturn_structure.png",
        "textures/structure/uranus_structure.png",
        "textures/structure/neptune_structure.png"
    };

    for (size_t i = 0; i < 8; ++i) {
        if (structureTextures[i].loadFromFile(textureFiles[i])) {
            structureTextures[i].setSmooth(true);
            std::cout << "Structure texture loaded: " << textureFiles[i] << std::endl;
        }
        else {
            std::cerr << "Failed to load: " << textureFiles[i] << std::endl;
        }
    }
    structureTexturesLoaded = true;
}

// ===== FONCTION POUR DESSINER UNE FLÈCHE COURBÉE =====
static void drawCurvedArrow(sf::RenderWindow& window,
    sf::Vector2f start, sf::Vector2f end,
    sf::Color color, float curvature = 30.f) {
    // Calculer le point de contrôle pour la courbe de Bézier
    sf::Vector2f mid = (start + end) / 2.f;
    sf::Vector2f normal(-(end.y - start.y), end.x - start.x);
    float len = std::sqrt(normal.x * normal.x + normal.y * normal.y);
    if (len > 0) {
        normal /= len;
    }
    sf::Vector2f control = mid + normal * curvature;

    // Dessiner la courbe avec des segments
    sf::VertexArray curve(sf::LineStrip, 20);
    for (int i = 0; i < 20; ++i) {
        float t = static_cast<float>(i) / 19.f;
        float u = 1.f - t;

        // Formule de Bézier quadratique
        sf::Vector2f point = u * u * start + 2.f * u * t * control + t * t * end;

        // Dégradé de couleur le long de la flèche
        sf::Uint8 alpha = static_cast<sf::Uint8>(100 + 155 * t);
        curve[i].position = point;
        curve[i].color = sf::Color(color.r, color.g, color.b, alpha);
    }
    window.draw(curve);

    // Pointe de la flèche
    sf::Vector2f dir = end - curve[18].position;
    float dirLen = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (dirLen > 0) {
        dir /= dirLen;
    }

    sf::ConvexShape arrowHead;
    arrowHead.setPointCount(3);
    arrowHead.setPoint(0, end);
    arrowHead.setPoint(1, end - dir * 12.f + sf::Vector2f(-dir.y, dir.x) * 6.f);
    arrowHead.setPoint(2, end - dir * 12.f + sf::Vector2f(dir.y, -dir.x) * 6.f);
    arrowHead.setFillColor(color);
    window.draw(arrowHead);
}

// ===== FONCTION POUR DESSINER UN POINT D'ANCRAGE LUMINEUX =====
static void drawAnchorPoint(sf::RenderWindow& window, sf::Vector2f pos, sf::Color color, float time) {
    // Cercle pulsant
    float pulse = 1.f + 0.3f * std::sin(time * 4.f);

    // Lueur externe
    sf::CircleShape glow(8.f * pulse);
    glow.setOrigin(8.f * pulse, 8.f * pulse);
    glow.setPosition(pos);
    glow.setFillColor(sf::Color(color.r, color.g, color.b, 50));
    window.draw(glow);

    // Point central
    sf::CircleShape point(4.f);
    point.setOrigin(4.f, 4.f);
    point.setPosition(pos);
    point.setFillColor(color);
    point.setOutlineThickness(1.f);
    point.setOutlineColor(sf::Color::White);
    window.draw(point);
}

// ===== STRUCTURE INTERNE DES PLANÈTES  =====


void drawPlanetStructure(sf::RenderWindow& window, const sf::Font& font,
    std::vector<CosmicButton>& backButtons,
    std::vector<Star>& stars, float time) {

    // Charger les textures si nécessaire
    loadStructureTextures();

    // Fond spatial animé
    static float structureBg = 0.f;
    structureBg += 0.003f;
    int bgShade = static_cast<int>(8 + 4 * std::sin(structureBg));
    window.clear(sf::Color(bgShade, bgShade + 5, 25 + static_cast<int>(8 * std::sin(structureBg * 0.5f))));

    // Fond étoilé
    for (auto& s : stars) {
        s.draw(window);
    }

    // ===== DONNÉES SCIENTIFIQUES =====
    struct LayerAnchor {
        float angle;
        float radiusPercent;
    };

    struct PlanetLayer {
        std::string name;
        float radiusPercent;
        sf::Color color;
        std::string thickness;
        LayerAnchor anchor;
    };

    struct PlanetStructure {
        std::string name;
        sf::Color mainColor;
        std::vector<PlanetLayer> layers;
        std::string description;
        std::string circumference;
        std::string age;
        std::string orbitalPeriod;
        std::string surfaceArea;
        std::string weight;
        std::string satellites;
        int textureIndex;
    };

    static int selectedPlanetIndex = 2;
    static float selectionAnim = 0.f;
    static float layerPulse = 0.f;
    selectionAnim += 0.02f;
    layerPulse += 0.05f;

    // ===== STRUCTURES PLANÉTAIRES EN FRANÇAIS =====
    std::vector<PlanetStructure> planetStructures = {
        // MERCURE
        {"MERCURE", sf::Color(169, 169, 169), {
            {"CROUTE", 1.0f, sf::Color(139, 119, 101), "0 - 100 KM", {-30.f, 0.95f}},
            {"MANTEAU", 0.85f, sf::Color(180, 100, 60), "100 - 600 KM", {-60.f, 0.70f}},
            {"NOYAU EXTERNE", 0.55f, sf::Color(255, 140, 0), "600 - 1 100 KM", {-90.f, 0.42f}},
            {"NOYAU INTERNE", 0.35f, sf::Color(255, 200, 50), "(SOLIDE) 1 100 - 1 800 KM", {-120.f, 0.18f}}
        }, "La plus petite planete, la plus proche du Soleil avec un noyau metallique geant.",
           "15 329 km", "4,5 Mrd ans", "88 jours", "74,8 M km2",
           "3,28 x 10^23 kg", "0 lune", 0},

        // VÉNUS
        {"VENUS", sf::Color(255, 198, 73), {
            {"CROUTE", 1.0f, sf::Color(200, 150, 100), "0 - 50 KM", {-25.f, 0.97f}},
            {"MANTEAU", 0.92f, sf::Color(205, 92, 0), "50 - 3 000 KM", {-55.f, 0.72f}},
            {"NOYAU EXTERNE", 0.5f, sf::Color(255, 140, 0), "(LIQUIDE) 3 000 - 5 000 KM", {-85.f, 0.38f}},
            {"NOYAU INTERNE", 0.3f, sf::Color(255, 180, 50), "(SOLIDE) 5 000 - 6 000 KM", {-115.f, 0.15f}}
        }, "Taille similaire a la Terre mais avec des conditions de surface extremes.",
           "38 025 km", "4,5 Mrd ans", "225 jours", "460 M km2",
           "4,87 x 10^24 kg", "0 lune", 1},

        // TERRE
        {"TERRE", sf::Color(100, 180, 255), {
            {"LITHOSPHERE", 1.0f, sf::Color(70, 130, 180), "0 - 60 KM", {-20.f, 0.97f}},
            {"ASTHENOSPHERE", 0.94f, sf::Color(60, 100, 160), "100 - 700 KM", {-40.f, 0.88f}},
            {"MANTEAU", 0.82f, sf::Color(200, 120, 50), "35 - 2 890 KM", {-65.f, 0.68f}},
            {"NOYAU EXTERNE", 0.55f, sf::Color(255, 140, 0), "(LIQUIDE) 2 890 - 5 100 KM", {-90.f, 0.42f}},
            {"NOYAU INTERNE", 0.18f, sf::Color(255, 255, 200), "(SOLIDE) 5 150 - 6 360 KM", {-120.f, 0.10f}}
        }, "La troisieme planete du Soleil et le seul objet astronomique connu pour abriter la vie.",
           "40 075 km", "4,5 Mrd ans", "365 jours", "510 M km2",
           "5,97 x 10^24 kg", "1 (Lune)", 2},

        // MARS
        {"MARS", sf::Color(205, 92, 92), {
            {"CROUTE", 1.0f, sf::Color(180, 80, 50), "0 - 50 KM", {-25.f, 0.96f}},
            {"MANTEAU", 0.88f, sf::Color(160, 70, 40), "50 - 1 800 KM", {-60.f, 0.70f}},
            {"NOYAU", 0.5f, sf::Color(200, 100, 50), "1 800 - 3 400 KM", {-100.f, 0.30f}}
        }, "La planete rouge avec le plus grand volcan du systeme solaire.",
           "21 344 km", "4,6 Mrd ans", "687 jours", "145 M km2",
           "6,39 x 10^23 kg", "2 (Phobos, Deimos)", 3},

        // JUPITER
        {"JUPITER", sf::Color(218, 165, 32), {
            {"ATMOSPHERE", 1.0f, sf::Color(220, 180, 140), "0 - 1 000 KM", {-20.f, 0.97f}},
            {"H2 MOLECULAIRE", 0.9f, sf::Color(200, 150, 100), "1 000 - 20 000 KM", {-45.f, 0.82f}},
            {"H METALLIQUE", 0.7f, sf::Color(180, 120, 80), "20 000 - 60 000 KM", {-75.f, 0.55f}},
            {"NOYAU", 0.12f, sf::Color(139, 90, 43), "~70 000 KM", {-110.f, 0.08f}}
        }, "La plus grande planete avec le champ magnetique le plus puissant.",
           "439 264 km", "4,6 Mrd ans", "11,86 ans", "61,4 Mrd km2",
           "1,90 x 10^27 kg", "95 lunes", 4},

        // SATURNE
        {"SATURNE", sf::Color(238, 232, 170), {
            {"ATMOSPHERE", 1.0f, sf::Color(230, 210, 180), "0 - 1 000 KM", {-20.f, 0.97f}},
            {"H2 MOLECULAIRE", 0.9f, sf::Color(210, 190, 150), "1 000 - 30 000 KM", {-50.f, 0.78f}},
            {"H METALLIQUE", 0.55f, sf::Color(180, 140, 100), "30 000 - 50 000 KM", {-85.f, 0.42f}},
            {"NOYAU", 0.2f, sf::Color(139, 119, 101), "~58 000 KM", {-115.f, 0.12f}}
        }, "Celebre pour ses anneaux, densite inferieure a l'eau.",
           "378 675 km", "4,5 Mrd ans", "29,46 ans", "42,7 Mrd km2",
           "5,68 x 10^26 kg", "146 lunes", 5},

        // URANUS
        {"URANUS", sf::Color(175, 238, 238), {
            {"ATMOSPHERE", 1.0f, sf::Color(180, 230, 230), "0 - 2 000 KM", {-25.f, 0.95f}},
            {"MANTEAU DE GLACE", 0.85f, sf::Color(100, 180, 200), "2 000 - 18 000 KM", {-65.f, 0.60f}},
            {"NOYAU ROCHEUX", 0.2f, sf::Color(80, 100, 120), "~25 000 KM", {-110.f, 0.12f}}
        }, "Tourne sur le cote avec une inclinaison axiale extreme de 98 degres.",
           "160 590 km", "4,5 Mrd ans", "84 ans", "8,08 Mrd km2",
           "8,68 x 10^25 kg", "27 lunes", 6},

        // NEPTUNE
        {"NEPTUNE", sf::Color(65, 105, 225), {
            {"ATMOSPHERE", 1.0f, sf::Color(80, 120, 200), "0 - 2 000 KM", {-25.f, 0.95f}},
            {"MANTEAU DE GLACE", 0.85f, sf::Color(50, 80, 180), "2 000 - 17 000 KM", {-65.f, 0.60f}},
            {"NOYAU ROCHEUX", 0.2f, sf::Color(40, 50, 80), "~24 000 KM", {-110.f, 0.12f}}
        }, "La planete la plus venteuse avec des vitesses atteignant 2 100 km/h.",
           "155 600 km", "4,5 Mrd ans", "165 ans", "7,62 Mrd km2",
           "1,02 x 10^26 kg", "16 lunes", 7}
    };

    const auto& planet = planetStructures[selectedPlanetIndex];

    // ===== TITRE PRINCIPAL =====
    sf::Text planetTitle(planet.name + ":", font, 72);
    planetTitle.setPosition(60.f, 40.f);
    planetTitle.setFillColor(sf::Color::White);
    planetTitle.setStyle(sf::Text::Bold);
    planetTitle.setLetterSpacing(3.f);
    window.draw(planetTitle);

    // Description
    sf::Text descText(planet.description, font, 16);
    descText.setPosition(60.f, 120.f);
    descText.setFillColor(sf::Color(200, 220, 240));
    window.draw(descText);

    // ===== NAVIGATION PAR ONGLETS =====
    float navY = 160.f;
    float navBtnWidth = 130.f;
    float startX = 60.f;

    for (size_t i = 0; i < planetStructures.size(); ++i) {
        sf::RectangleShape navBtn(sf::Vector2f(navBtnWidth, 40.f));
        navBtn.setPosition(startX + i * (navBtnWidth + 8.f), navY);

        bool isSelected = static_cast<int>(i) == selectedPlanetIndex;

        if (isSelected) {
            float pulse = 0.7f + 0.3f * std::sin(selectionAnim * 4.f);
            navBtn.setFillColor(sf::Color(
                static_cast<sf::Uint8>(planetStructures[i].mainColor.r * pulse),
                static_cast<sf::Uint8>(planetStructures[i].mainColor.g * pulse),
                static_cast<sf::Uint8>(planetStructures[i].mainColor.b * pulse), 230));
            navBtn.setOutlineThickness(3);
            navBtn.setOutlineColor(sf::Color::White);
        }
        else {
            navBtn.setFillColor(sf::Color(20, 35, 60, 200));
            navBtn.setOutlineThickness(1);
            navBtn.setOutlineColor(sf::Color(planetStructures[i].mainColor.r,
                planetStructures[i].mainColor.g, planetStructures[i].mainColor.b, 150));
        }
        window.draw(navBtn);

        sf::Text navText(planetStructures[i].name, font, 14);
        navText.setPosition(startX + i * (navBtnWidth + 8.f) + navBtnWidth / 2.f, navY + 12.f);
        navText.setOrigin(navText.getLocalBounds().width / 2.f, 0.f);
        navText.setFillColor(isSelected ? sf::Color::White : sf::Color(180, 200, 220));
        if (isSelected) navText.setStyle(sf::Text::Bold);
        window.draw(navText);
    }

    // Gestion du clic sur les onglets
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

    static bool wasPressed = false;
    bool isPressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);

    if (isPressed && !wasPressed) {
        for (size_t i = 0; i < planetStructures.size(); ++i) {
            sf::FloatRect btnBounds(startX + i * (navBtnWidth + 8.f), navY, navBtnWidth, 40.f);
            if (btnBounds.contains(mousePos)) {
                selectedPlanetIndex = static_cast<int>(i);
            }
        }
    }
    wasPressed = isPressed;

    // ===== VISUALISATION DE LA STRUCTURE =====
    float structX = 280.f;
    float structY = 380.f;
    float maxRadius = 180.f;

    // Lueur derrière la structure
    for (int g = 5; g >= 0; --g) {
        sf::CircleShape glow(maxRadius + g * 15.f);
        glow.setOrigin(maxRadius + g * 15.f, maxRadius + g * 15.f);
        glow.setPosition(structX, structY);
        glow.setFillColor(sf::Color(planet.mainColor.r, planet.mainColor.g,
            planet.mainColor.b, static_cast<sf::Uint8>(20 - g * 3)));
        window.draw(glow);
    }

    // Vérifier si texture disponible
    bool hasTexture = structureTextures[planet.textureIndex].getSize().x > 0;

    if (hasTexture) {
        sf::Sprite structureSprite;
        structureSprite.setTexture(structureTextures[planet.textureIndex]);

        sf::FloatRect bounds = structureSprite.getLocalBounds();
        float scale = (maxRadius * 2.f) / std::max(bounds.width, bounds.height);
        structureSprite.setScale(scale, scale);
        structureSprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        structureSprite.setPosition(structX, structY);
        window.draw(structureSprite);
    }
    else {
        // Dessiner les couches manuellement
        for (int i = static_cast<int>(planet.layers.size()) - 1; i >= 0; --i) {
            const auto& layer = planet.layers[i];
            float radius = maxRadius * layer.radiusPercent;

            sf::ConvexShape halfCircle;
            halfCircle.setPointCount(82);
            halfCircle.setPoint(0, sf::Vector2f(structX, structY));

            for (int p = 0; p <= 80; ++p) {
                float angle = 90.f + (p / 80.f) * 180.f;
                float rad = angle * 3.14159f / 180.f;
                halfCircle.setPoint(p + 1, sf::Vector2f(
                    structX + radius * std::cos(rad),
                    structY + radius * std::sin(rad)
                ));
            }

            halfCircle.setFillColor(layer.color);
            halfCircle.setOutlineThickness(1);
            halfCircle.setOutlineColor(sf::Color(0, 0, 0, 120));
            window.draw(halfCircle);

            if (i == 0) {
                sf::ConvexShape rightHalf;
                rightHalf.setPointCount(82);
                rightHalf.setPoint(0, sf::Vector2f(structX, structY));

                for (int p = 0; p <= 80; ++p) {
                    float angle = -90.f + (p / 80.f) * 180.f;
                    float rad = angle * 3.14159f / 180.f;
                    rightHalf.setPoint(p + 1, sf::Vector2f(
                        structX + radius * std::cos(rad),
                        structY + radius * std::sin(rad)
                    ));
                }

                rightHalf.setFillColor(planet.mainColor);
                rightHalf.setOutlineThickness(2);
                rightHalf.setOutlineColor(sf::Color(255, 255, 255, 120));
                window.draw(rightHalf);

                sf::CircleShape highlight(radius * 0.2f);
                highlight.setOrigin(radius * 0.2f, radius * 0.2f);
                highlight.setPosition(structX + radius * 0.4f, structY - radius * 0.4f);
                highlight.setFillColor(sf::Color(255, 255, 255, 50));
                window.draw(highlight);
            }
        }
    }

    // ===== LÉGENDE DES COUCHES =====
    float labelX = 530.f;
    float labelStartY = 220.f;
    float labelSpacing = 65.f;

    for (size_t i = 0; i < planet.layers.size(); ++i) {
        const auto& layer = planet.layers[i];
        float labelY = labelStartY + i * labelSpacing;

        float anchorAngle = layer.anchor.angle * 3.14159f / 180.f;
        float anchorRadius = maxRadius * layer.anchor.radiusPercent;
        sf::Vector2f anchorPos(
            structX + anchorRadius * std::cos(anchorAngle),
            structY + anchorRadius * std::sin(anchorAngle)
        );

        sf::VertexArray line(sf::LineStrip, 3);
        line[0].position = anchorPos;
        line[0].color = sf::Color(planet.mainColor.r, planet.mainColor.g, planet.mainColor.b, 150);
        line[1].position = sf::Vector2f(labelX - 30.f, labelY + 15.f);
        line[1].color = planet.mainColor;
        line[2].position = sf::Vector2f(labelX - 10.f, labelY + 15.f);
        line[2].color = planet.mainColor;
        window.draw(line);

        sf::CircleShape anchor(6.f);
        anchor.setOrigin(6.f, 6.f);
        anchor.setPosition(anchorPos);
        anchor.setFillColor(planet.mainColor);
        anchor.setOutlineThickness(2);
        anchor.setOutlineColor(sf::Color::White);
        window.draw(anchor);

        sf::Text layerName(layer.name, font, 22);
        layerName.setPosition(labelX, labelY);
        layerName.setFillColor(sf::Color::White);
        layerName.setStyle(sf::Text::Bold);
        window.draw(layerName);

        sf::Text depthText(layer.thickness, font, 16);
        depthText.setPosition(labelX, labelY + 28.f);
        depthText.setFillColor(sf::Color(180, 200, 220));
        window.draw(depthText);

        if (i < planet.layers.size() - 1) {
            sf::RectangleShape sep(sf::Vector2f(450.f, 1.f));
            sep.setPosition(labelX, labelY + 55.f);
            sep.setFillColor(sf::Color(80, 120, 180, 100));
            window.draw(sep);
        }
    }

    // ===== BOÎTES D'INFORMATIONS =====
    float boxY = 620.f;
    float boxWidth = 175.f;  
    float boxHeight = 80.f;  
    float boxSpacing = 8.f;  
    float boxStartX = 60.f;

    struct InfoBox {
        std::string title;
        std::string value;
    };

    std::vector<InfoBox> infoBoxes = {
        {"CIRCONFERENCE", planet.circumference},
        {"AGE", planet.age},
        {"PERIODE ORBITALE", planet.orbitalPeriod},
        {"SUPERFICIE", planet.surfaceArea},
        {"MASSE", planet.weight},
        {"SATELLITES", planet.satellites}
    };

    for (size_t i = 0; i < infoBoxes.size(); ++i) {
        float boxX = boxStartX + i * (boxWidth + boxSpacing);

        // Fond de la boîte
        sf::RectangleShape box(sf::Vector2f(boxWidth, boxHeight));
        box.setPosition(boxX, boxY);
        box.setFillColor(sf::Color(15, 25, 45, 220));
        box.setOutlineThickness(2);
        box.setOutlineColor(sf::Color(60, 100, 160, 180));
        window.draw(box);

        // Ligne supérieure colorée
        sf::RectangleShape topLine(sf::Vector2f(boxWidth, 3.f));
        topLine.setPosition(boxX, boxY);
        topLine.setFillColor(planet.mainColor);
        window.draw(topLine);

        // Titre
        sf::Text titleText(infoBoxes[i].title, font, 11);  
        titleText.setPosition(boxX + boxWidth / 2.f, boxY + 10.f);
        titleText.setOrigin(titleText.getLocalBounds().width / 2.f, 0.f);
        titleText.setFillColor(planet.mainColor);
        titleText.setStyle(sf::Text::Bold);
        window.draw(titleText);

        // Valeur
        sf::Text valueText(infoBoxes[i].value, font, 12);  
        valueText.setPosition(boxX + boxWidth / 2.f, boxY + 35.f);
        valueText.setOrigin(valueText.getLocalBounds().width / 2.f, 0.f);
        valueText.setFillColor(sf::Color(220, 230, 245));
        window.draw(valueText);
    }

    // ===== BOUTON RETOUR - POSITION CORRIGÉE EN BAS À DROITE =====
    for (auto& b : backButtons) {
        //  POSITION FIXE EN BAS À DROITE
        b.base.setPosition(1050.f, 680.f);
        b.base.setSize(sf::Vector2f(120.f, 40.f));

        sf::Vector2f btnPos = b.base.getPosition();
        sf::Vector2f btnSize = b.base.getSize();

        // Vérifier le survol
        sf::FloatRect btnBounds(btnPos.x, btnPos.y, btnSize.x, btnSize.y);
        bool isHovered = btnBounds.contains(mousePos);
        b.hover = isHovered;

        // Effet hover lumineux
        if (b.hover) {
            sf::RectangleShape hoverGlow(sf::Vector2f(btnSize.x + 10.f, btnSize.y + 10.f));
            hoverGlow.setPosition(btnPos.x - 5.f, btnPos.y - 5.f);
            hoverGlow.setFillColor(sf::Color(255, 100, 100, 60));
            window.draw(hoverGlow);
        }

        // Fond du bouton avec dégradé
        sf::RectangleShape btnTop(sf::Vector2f(btnSize.x, btnSize.y / 2.f));
        btnTop.setPosition(btnPos);
        btnTop.setFillColor(b.hover ? sf::Color(255, 100, 80, 255) : sf::Color(200, 60, 50, 245));
        window.draw(btnTop);

        sf::RectangleShape btnBottom(sf::Vector2f(btnSize.x, btnSize.y / 2.f));
        btnBottom.setPosition(btnPos.x, btnPos.y + btnSize.y / 2.f);
        btnBottom.setFillColor(b.hover ? sf::Color(220, 80, 60, 255) : sf::Color(160, 50, 40, 245));
        window.draw(btnBottom);

        // Bordure
        sf::RectangleShape btnBorder(btnSize);
        btnBorder.setPosition(btnPos);
        btnBorder.setFillColor(sf::Color::Transparent);
        btnBorder.setOutlineThickness(2.f);
        btnBorder.setOutlineColor(b.hover ? sf::Color(255, 200, 180, 255) : sf::Color(255, 130, 110, 200));
        window.draw(btnBorder);

        // Texte du bouton - CENTRÉ
        sf::Text btnText("RETOUR", font, 15);
        sf::FloatRect btnTextBounds = btnText.getLocalBounds();
        btnText.setOrigin(btnTextBounds.width / 2.f, btnTextBounds.height / 2.f);
        btnText.setPosition(btnPos.x + btnSize.x / 2.f, btnPos.y + btnSize.y / 2.f - 2.f);
        btnText.setFillColor(sf::Color::White);
        btnText.setStyle(sf::Text::Bold);
        window.draw(btnText);
    }
}
// ===== FONCTION POUR AFFICHER L'ORDRE DES PLANÈTES =====

void drawPlanetsOrderMessage(sf::RenderWindow& window, const sf::Font& font) {
    // Sauvegarder et définir la vue par défaut
    sf::View currentView = window.getView();
    sf::View defaultView(sf::FloatRect(0.f, 0.f,
        static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y)));
    window.setView(defaultView);

    sf::Vector2u windowSize = window.getSize();

    float boxWidth = 580.f;
    float boxHeight = 620.f;

    float boxX = (static_cast<float>(windowSize.x) - boxWidth) / 2.f;
    float boxY = (static_cast<float>(windowSize.y) - boxHeight) / 2.f;

    // Fond semi-transparent plus clair
    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(windowSize.x),
        static_cast<float>(windowSize.y)));
    overlay.setFillColor(sf::Color(0, 20, 50, 180));
    window.draw(overlay);

    // Boîte principale - COULEURS PLUS CLAIRES
    sf::RectangleShape messageBox(sf::Vector2f(boxWidth, boxHeight));
    messageBox.setPosition(boxX, boxY);
    messageBox.setFillColor(sf::Color(25, 50, 90, 250));  // Bleu foncé clair
    messageBox.setOutlineThickness(4.f);
    messageBox.setOutlineColor(sf::Color(130, 200, 255, 255));  // Bordure bleue claire
    window.draw(messageBox);

    // En-tête - COULEUR PLUS CLAIRE
    sf::RectangleShape header(sf::Vector2f(boxWidth, 80.f));
    header.setPosition(boxX, boxY);
    header.setFillColor(sf::Color(50, 120, 200, 255));  // Bleu clair
    window.draw(header);

    // Titre - PLUS VISIBLE
    sf::Text titleText("ORDRE DES PLANETES", font, 36);
    titleText.setPosition(boxX + boxWidth / 2.f, boxY + 22.f);
    titleText.setOrigin(titleText.getLocalBounds().width / 2.f, 0.f);
    titleText.setFillColor(sf::Color(255, 255, 255, 255));  // Blanc pur
    titleText.setStyle(sf::Text::Bold);
    titleText.setOutlineThickness(2.f);
    titleText.setOutlineColor(sf::Color(30, 80, 150));
    window.draw(titleText);

    // Ligne séparatrice plus visible
    sf::RectangleShape separator(sf::Vector2f(boxWidth - 30.f, 3.f));
    separator.setPosition(boxX + 15.f, boxY + 85.f);
    separator.setFillColor(sf::Color(130, 200, 255, 255));
    window.draw(separator);

    // Indication instructive - PLUS VISIBLE
    sf::Text instructionText("Appuyez sur les touches 1-8 pour suivre une planete :", font, 16);
    instructionText.setPosition(boxX + 25.f, boxY + 100.f);
    instructionText.setFillColor(sf::Color(255, 240, 150, 255));  // Jaune clair
    instructionText.setStyle(sf::Text::Bold);
    window.draw(instructionText);

    // Liste des planètes avec données
    std::vector<std::tuple<int, std::string, std::string>> planets = {
        {1, "Mercure", "57.9 millions km"},
        {2, "Venus", "108.2 millions km"},
        {3, "Terre", "149.6 millions km"},
        {4, "Mars", "227.9 millions km"},
        {5, "Jupiter", "778.5 millions km"},
        {6, "Saturne", "1.434 milliards km"},
        {7, "Uranus", "2.873 milliards km"},
        {8, "Neptune", "4.495 milliards km"}
    };

    // Couleurs PLUS CLAIRES pour chaque planète
    std::vector<sf::Color> colors = {
        sf::Color(200, 200, 200),   // Mercure - Gris clair
        sf::Color(255, 220, 130),   // Venus - Jaune clair
        sf::Color(130, 180, 255),   // Terre - Bleu clair
        sf::Color(240, 150, 130),   // Mars - Rouge clair
        sf::Color(255, 210, 140),   // Jupiter - Orange clair
        sf::Color(255, 245, 200),   // Saturne - Crème
        sf::Color(200, 255, 255),   // Uranus - Cyan clair
        sf::Color(140, 170, 255)    // Neptune - Bleu lavande
    };

    float startY = boxY + 135.f;
    float lineHeight = 55.f;

    for (size_t i = 0; i < planets.size(); ++i) {
        float currentY = startY + i * lineHeight;

        // Fond alterné plus visible
        sf::RectangleShape lineBg(sf::Vector2f(boxWidth - 30.f, 50.f));
        lineBg.setPosition(boxX + 15.f, currentY);
        if (i % 2 == 0) {
            lineBg.setFillColor(sf::Color(40, 70, 120, 200));
        }
        else {
            lineBg.setFillColor(sf::Color(50, 85, 140, 200));
        }
        lineBg.setOutlineThickness(1.f);
        lineBg.setOutlineColor(sf::Color(100, 150, 200, 100));
        window.draw(lineBg);

        // Numéro dans un cercle coloré
        sf::CircleShape numberCircle(20.f);
        numberCircle.setOrigin(20.f, 20.f);
        numberCircle.setPosition(boxX + 45.f, currentY + 25.f);
        numberCircle.setFillColor(colors[i]);
        numberCircle.setOutlineThickness(3.f);
        numberCircle.setOutlineColor(sf::Color(255, 255, 255, 230));
        window.draw(numberCircle);

        // Texte du numéro - NOIR pour contraste
        sf::Text numberText(std::to_string(std::get<0>(planets[i])), font, 22);
        numberText.setPosition(boxX + 45.f, currentY + 12.f);
        numberText.setOrigin(numberText.getLocalBounds().width / 2.f, 0.f);
        numberText.setFillColor(sf::Color(30, 30, 30));  // Noir pour contraste
        numberText.setStyle(sf::Text::Bold);
        window.draw(numberText);

        // Nom de la planète - PLUS GRAND ET VISIBLE
        sf::Text planetName(std::get<1>(planets[i]), font, 22);
        planetName.setPosition(boxX + 90.f, currentY + 12.f);
        planetName.setFillColor(colors[i]);
        planetName.setStyle(sf::Text::Bold);
        planetName.setOutlineThickness(1.f);
        planetName.setOutlineColor(sf::Color(0, 0, 0, 150));
        window.draw(planetName);

        // Distance - PLUS VISIBLE
        sf::Text distanceText(std::get<2>(planets[i]), font, 16);
        distanceText.setPosition(boxX + 340.f, currentY + 15.f);
        distanceText.setFillColor(sf::Color(220, 240, 255, 255));
        window.draw(distanceText);
    }

    // Message en bas - PLUS VISIBLE
    sf::RectangleShape footerBg(sf::Vector2f(boxWidth - 30.f, 35.f));
    footerBg.setPosition(boxX + 15.f, boxY + boxHeight - 50.f);
    footerBg.setFillColor(sf::Color(40, 80, 140, 200));
    window.draw(footerBg);

    sf::Text infoText("Cliquez n'importe ou pour fermer", font, 16);
    infoText.setPosition(boxX + boxWidth / 2.f, boxY + boxHeight - 42.f);
    infoText.setOrigin(infoText.getLocalBounds().width / 2.f, 0.f);
    infoText.setFillColor(sf::Color(200, 230, 255, 255));
    infoText.setStyle(sf::Text::Italic);
    window.draw(infoText);

    // Restaurer la vue originale
    window.setView(currentView);
}

void drawPlanetComparison(sf::RenderWindow& window, const sf::Font& font,
    const std::vector<Planet>& planets,
    int& selectedPlanet1, int& selectedPlanet2,
    bool& isActive) {

    // Sauvegarder et définir la vue par défaut
    sf::View currentView = window.getView();
    sf::View defaultView(sf::FloatRect(0.f, 0.f,
        static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y)));
    window.setView(defaultView);

    sf::Vector2u windowSize = window.getSize();
    float boxWidth = 1080.f;
    float boxHeight = 740.f;
    float boxX = (static_cast<float>(windowSize.x) - boxWidth) / 2.f;
    float boxY = (static_cast<float>(windowSize.y) - boxHeight) / 2.f;

    // ✅ FOND SPATIAL ANIMÉ
    static float spaceAnim = 0.f;
    spaceAnim += 0.01f;

    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(windowSize.x),
        static_cast<float>(windowSize.y)));
    overlay.setFillColor(sf::Color(5, 10, 25, 220));
    window.draw(overlay);

    // Étoiles scintillantes
    for (int i = 0; i < 50; ++i) {
        float x = (i * 127.f) / 50.f * windowSize.x / 127.f;
        float y = (i * 83.f) / 50.f * windowSize.y / 83.f;
        float twinkle = 150.f + 100.f * std::sin(spaceAnim * 3.f + i);

        sf::CircleShape star(1.f + std::sin(spaceAnim + i) * 0.5f);
        star.setPosition(x, y);
        star.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(twinkle)));
        window.draw(star);
    }

    // Boîte principale avec dégradé
    for (int i = 0; i < 10; ++i) {
        float ratio = static_cast<float>(i) / 10.f;
        sf::Uint8 r = static_cast<sf::Uint8>(20 + 40 * ratio);
        sf::Uint8 g = static_cast<sf::Uint8>(35 + 60 * ratio);
        sf::Uint8 b = static_cast<sf::Uint8>(70 + 50 * ratio);

        sf::RectangleShape gradientLayer(sf::Vector2f(boxWidth, boxHeight / 10.f));
        gradientLayer.setPosition(boxX, boxY + i * boxHeight / 10.f);
        gradientLayer.setFillColor(sf::Color(r, g, b, 245));
        window.draw(gradientLayer);
    }

    // Bordure lumineuse
    sf::RectangleShape borderGlow(sf::Vector2f(boxWidth, boxHeight));
    borderGlow.setPosition(boxX, boxY);
    borderGlow.setFillColor(sf::Color::Transparent);
    borderGlow.setOutlineThickness(5.f);
    float glowPulse = 150.f + 100.f * std::sin(spaceAnim * 2.f);
    borderGlow.setOutlineColor(sf::Color(100, 200, 255, static_cast<sf::Uint8>(glowPulse)));
    window.draw(borderGlow);

    // En-tête dégradé
    for (int i = 0; i < 5; ++i) {
        float ratio = static_cast<float>(i) / 5.f;
        sf::RectangleShape headerLayer(sf::Vector2f(boxWidth, 14.f));
        headerLayer.setPosition(boxX, boxY + i * 14.f);
        sf::Uint8 r = static_cast<sf::Uint8>(30 + 100 * (1.f - ratio));
        sf::Uint8 g = static_cast<sf::Uint8>(80 + 150 * (1.f - ratio));
        sf::Uint8 b = static_cast<sf::Uint8>(150 + 100 * (1.f - ratio));
        headerLayer.setFillColor(sf::Color(r, g, b, 255));
        window.draw(headerLayer);
    }

    sf::RectangleShape topGlow(sf::Vector2f(boxWidth, 3.f));
    topGlow.setPosition(boxX, boxY);
    float topPulse = 200.f + 55.f * std::sin(spaceAnim * 4.f);
    topGlow.setFillColor(sf::Color(150, 220, 255, static_cast<sf::Uint8>(topPulse)));
    window.draw(topGlow);

    //  BOUTON RETOUR AVEC DÉTECTION DE SURVOL FIABLE
    float returnBtnX = boxX + 25.f;
    float returnBtnY = boxY + 15.f;
    float returnBtnW = 130.f;
    float returnBtnH = 45.f;

    //  RÉCUPÉRER LA POSITION DE LA SOURIS DANS LA VUE PAR DÉFAUT
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel, defaultView);

    //  VÉRIFIER SI LA SOURIS SURVOLE LE BOUTON
    sf::FloatRect returnBtnBounds(returnBtnX, returnBtnY, returnBtnW, returnBtnH);
    bool isHovered = returnBtnBounds.contains(mousePos);

    //  EFFET VISUEL AU SURVOL
    if (isHovered) {
        // Lueur de survol
        sf::RectangleShape hoverGlow(sf::Vector2f(returnBtnW + 10.f, returnBtnH + 10.f));
        hoverGlow.setPosition(returnBtnX - 5.f, returnBtnY - 5.f);
        hoverGlow.setFillColor(sf::Color(255, 100, 100, 60));
        window.draw(hoverGlow);
    }

    // Fond du bouton
    sf::RectangleShape returnBg1(sf::Vector2f(returnBtnW, returnBtnH / 2.f));
    returnBg1.setPosition(returnBtnX, returnBtnY);
    returnBg1.setFillColor(isHovered ? sf::Color(255, 80, 80, 255) : sf::Color(220, 50, 50, 240));
    window.draw(returnBg1);

    sf::RectangleShape returnBg2(sf::Vector2f(returnBtnW, returnBtnH / 2.f));
    returnBg2.setPosition(returnBtnX, returnBtnY + returnBtnH / 2.f);
    returnBg2.setFillColor(isHovered ? sf::Color(220, 60, 60, 255) : sf::Color(180, 40, 40, 240));
    window.draw(returnBg2);

    // Bordure
    sf::RectangleShape returnBtn(sf::Vector2f(returnBtnW, returnBtnH));
    returnBtn.setPosition(returnBtnX, returnBtnY);
    returnBtn.setFillColor(sf::Color::Transparent);
    returnBtn.setOutlineThickness(isHovered ? 3.f : 2.f);
    returnBtn.setOutlineColor(isHovered ? sf::Color(255, 200, 200) : sf::Color(255, 120, 120));
    window.draw(returnBtn);

    // Texte
    sf::Text returnText(" RETOUR", font, 17);
    returnText.setPosition(returnBtnX + returnBtnW / 2.f, returnBtnY + 12.f);
    returnText.setOrigin(returnText.getLocalBounds().width / 2.f, 0.f);
    returnText.setFillColor(sf::Color::White);
    returnText.setStyle(sf::Text::Bold);
    if (isHovered) {
        returnText.setOutlineThickness(1.f);
        returnText.setOutlineColor(sf::Color(100, 0, 0));
    }
    window.draw(returnText);

    //  DÉTECTER LE CLIC SUR LE BOUTON
    static bool wasPressed = false;
    bool isPressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);

    if (isPressed && !wasPressed && isHovered) {
        isActive = false;  // ✅ FERMER LE PANNEAU
        selectedPlanet1 = -1;
        selectedPlanet2 = -1;
    }
    wasPressed = isPressed;

    // Titre
    sf::Text title("COMPARAISON DE PLANETES", font, 34);
    title.setPosition(boxX + boxWidth / 2.f, boxY + 20.f);
    title.setOrigin(title.getLocalBounds().width / 2.f, 0.f);

    float hueShift = std::sin(spaceAnim * 1.5f);
    sf::Uint8 r = static_cast<sf::Uint8>(200 + 55 * hueShift);
    sf::Uint8 g = static_cast<sf::Uint8>(230 + 25 * std::abs(hueShift));
    title.setFillColor(sf::Color(r, g, 255, 255));
    title.setStyle(sf::Text::Bold);
    title.setOutlineThickness(2.f);
    title.setOutlineColor(sf::Color(50, 100, 200, 180));
    window.draw(title);

    // Instruction
    sf::Text instruction("Selectionnez deux planetes a comparer :", font, 17);
    instruction.setPosition(boxX + 35.f, boxY + 85.f);
    instruction.setFillColor(sf::Color(255, 240, 150));
    instruction.setStyle(sf::Text::Bold);
    window.draw(instruction);

    // Grille de planètes
    float planetBtnW = 235.f;
    float planetBtnH = 52.f;
    float startX = boxX + 65.f;
    float startY = boxY + 135.f;
    float spacingX = 252.f;
    float spacingY = 65.f;

    for (size_t i = 0; i < planets.size(); ++i) {
        int row = static_cast<int>(i / 4);
        int col = static_cast<int>(i % 4);

        float btnX = startX + col * spacingX;
        float btnY = startY + row * spacingY;

        bool isSelected1 = (selectedPlanet1 == static_cast<int>(i));
        bool isSelected2 = (selectedPlanet2 == static_cast<int>(i));

        // Ombre 3D
        sf::RectangleShape shadow(sf::Vector2f(planetBtnW, planetBtnH));
        shadow.setPosition(btnX + 4.f, btnY + 4.f);
        shadow.setFillColor(sf::Color(0, 0, 0, 100));
        window.draw(shadow);

        // Fond dégradé
        if (isSelected1) {
            sf::RectangleShape grad1(sf::Vector2f(planetBtnW, planetBtnH / 2.f));
            grad1.setPosition(btnX, btnY);
            grad1.setFillColor(sf::Color(80, 220, 140, 240));
            window.draw(grad1);

            sf::RectangleShape grad2(sf::Vector2f(planetBtnW, planetBtnH / 2.f));
            grad2.setPosition(btnX, btnY + planetBtnH / 2.f);
            grad2.setFillColor(sf::Color(50, 180, 100, 240));
            window.draw(grad2);
        }
        else if (isSelected2) {
            sf::RectangleShape grad1(sf::Vector2f(planetBtnW, planetBtnH / 2.f));
            grad1.setPosition(btnX, btnY);
            grad1.setFillColor(sf::Color(120, 150, 255, 240));
            window.draw(grad1);

            sf::RectangleShape grad2(sf::Vector2f(planetBtnW, planetBtnH / 2.f));
            grad2.setPosition(btnX, btnY + planetBtnH / 2.f);
            grad2.setFillColor(sf::Color(80, 110, 220, 240));
            window.draw(grad2);
        }
        else {
            sf::RectangleShape grad1(sf::Vector2f(planetBtnW, planetBtnH / 2.f));
            grad1.setPosition(btnX, btnY);
            grad1.setFillColor(sf::Color(60, 90, 140, 220));
            window.draw(grad1);

            sf::RectangleShape grad2(sf::Vector2f(planetBtnW, planetBtnH / 2.f));
            grad2.setPosition(btnX, btnY + planetBtnH / 2.f);
            grad2.setFillColor(sf::Color(40, 70, 120, 220));
            window.draw(grad2);
        }

        // Bordure
        sf::RectangleShape planetBtn(sf::Vector2f(planetBtnW, planetBtnH));
        planetBtn.setPosition(btnX, btnY);
        planetBtn.setFillColor(sf::Color::Transparent);
        planetBtn.setOutlineThickness(isSelected1 || isSelected2 ? 3.f : 2.f);

        if (isSelected1) {
            planetBtn.setOutlineColor(sf::Color(0, 255, 150));
        }
        else if (isSelected2) {
            planetBtn.setOutlineColor(sf::Color(120, 180, 255));
        }
        else {
            planetBtn.setOutlineColor(planets[i].getColor());
        }
        window.draw(planetBtn);

        // Numéro avec lueur
        sf::CircleShape numGlow(24.f);
        numGlow.setOrigin(24.f, 24.f);
        numGlow.setPosition(btnX + 35.f, btnY + 26.f);
        numGlow.setFillColor(sf::Color(planets[i].getColor().r,
            planets[i].getColor().g,
            planets[i].getColor().b, 60));
        window.draw(numGlow);

        sf::CircleShape numCircle(19.f);
        numCircle.setOrigin(19.f, 19.f);
        numCircle.setPosition(btnX + 35.f, btnY + 26.f);
        numCircle.setFillColor(planets[i].getColor());
        numCircle.setOutlineThickness(3.f);
        numCircle.setOutlineColor(sf::Color::White);
        window.draw(numCircle);

        sf::Text numText(std::to_string(i + 1), font, 19);
        numText.setPosition(btnX + 35.f, btnY + 12.f);
        numText.setOrigin(numText.getLocalBounds().width / 2.f, 0.f);
        numText.setFillColor(sf::Color(30, 30, 30));
        numText.setStyle(sf::Text::Bold);
        window.draw(numText);

        // Nom
        sf::Text planetName(planets[i].getName(), font, 18);
        planetName.setPosition(btnX + 68.f, btnY + 11.f);
        planetName.setFillColor(sf::Color::White);
        planetName.setStyle(sf::Text::Bold);
        window.draw(planetName);

        // Indicateur
        if (isSelected1) {
            sf::Text indicator("PLANETE 1", font, 13);
            indicator.setPosition(btnX + 68.f, btnY + 31.f);
            indicator.setFillColor(sf::Color(200, 255, 200));
            window.draw(indicator);
        }
        else if (isSelected2) {
            sf::Text indicator("PLANETE 2", font, 13);
            indicator.setPosition(btnX + 68.f, btnY + 31.f);
            indicator.setFillColor(sf::Color(200, 220, 255));
            window.draw(indicator);
        }
    }

    // Section de comparaison
    if (selectedPlanet1 >= 0 && selectedPlanet2 >= 0) {
        float compY = boxY + 280.f;

        // Séparateur
        for (int i = 0; i < 4; ++i) {
            sf::RectangleShape sepLayer(sf::Vector2f(boxWidth - 40.f, 1.f));
            sepLayer.setPosition(boxX + 20.f, compY + i);
            float intensity = 150.f + 100.f * std::sin(spaceAnim * 3.f + i);
            sepLayer.setFillColor(sf::Color(100, 200, 255, static_cast<sf::Uint8>(intensity)));
            window.draw(sepLayer);
        }

        const Planet& p1 = planets[selectedPlanet1];
        const Planet& p2 = planets[selectedPlanet2];

        float dataY = compY + 40.f;

        // Noms avec lueurs
        sf::CircleShape glow1(40.f);
        glow1.setOrigin(40.f, 40.f);
        glow1.setPosition(boxX + 240.f, dataY + 15.f);
        glow1.setFillColor(sf::Color(p1.getColor().r, p1.getColor().g, p1.getColor().b, 40));
        window.draw(glow1);

        sf::Text name1(p1.getName(), font, 30);
        name1.setPosition(boxX + 240.f, dataY);
        name1.setFillColor(p1.getColor());
        name1.setStyle(sf::Text::Bold);
        name1.setOrigin(name1.getLocalBounds().width / 2.f, 0.f);
        name1.setOutlineThickness(2.f);
        name1.setOutlineColor(sf::Color(0, 0, 0, 180));
        window.draw(name1);

        sf::CircleShape glow2(40.f);
        glow2.setOrigin(40.f, 40.f);
        glow2.setPosition(boxX + 840.f, dataY + 15.f);
        glow2.setFillColor(sf::Color(p2.getColor().r, p2.getColor().g, p2.getColor().b, 40));
        window.draw(glow2);

        sf::Text name2(p2.getName(), font, 30);
        name2.setPosition(boxX + 840.f, dataY);
        name2.setFillColor(p2.getColor());
        name2.setStyle(sf::Text::Bold);
        name2.setOrigin(name2.getLocalBounds().width / 2.f, 0.f);
        name2.setOutlineThickness(2.f);
        name2.setOutlineColor(sf::Color(0, 0, 0, 180));
        window.draw(name2);

        dataY += 70.f;

        // Données
        struct CompData {
            std::string label;
            std::string val1;
            std::string val2;
        };

        std::vector<CompData> data = {
            {"Diametre",
             std::to_string(static_cast<int>(p1.realDiameterKm)) + " km",
             std::to_string(static_cast<int>(p2.realDiameterKm)) + " km"},
            {"Masse",
             std::to_string(static_cast<int>(p1.mass)) + " x10^24 kg",
             std::to_string(static_cast<int>(p2.mass)) + " x10^24 kg"},
            {"Lunes",
             std::to_string(p1.numMoons),
             std::to_string(p2.numMoons)},
            {"Gravite",
             std::to_string(static_cast<int>(p1.gravity)) + " m/s2",
             std::to_string(static_cast<int>(p2.gravity)) + " m/s2"},
            {"Temperature",
             std::to_string(static_cast<int>(p1.temperature)) + " C",
             std::to_string(static_cast<int>(p2.temperature)) + " C"}
        };

        float lineH = 56.f;

        for (size_t i = 0; i < data.size(); ++i) {
            const auto& row = data[i];

            // Label
            sf::Text label(row.label, font, 20);
            label.setPosition(boxX + 70.f, dataY + 16.f);
            label.setFillColor(sf::Color(220, 230, 255));
            label.setStyle(sf::Text::Bold);
            window.draw(label);

            // Colonne 1
            sf::RectangleShape bg1Top(sf::Vector2f(340.f, 26.f));
            bg1Top.setPosition(boxX + 260.f, dataY);
            bg1Top.setFillColor(sf::Color(80, 200, 140, 150));
            window.draw(bg1Top);

            sf::RectangleShape bg1Bottom(sf::Vector2f(340.f, 26.f));
            bg1Bottom.setPosition(boxX + 260.f, dataY + 26.f);
            bg1Bottom.setFillColor(sf::Color(50, 160, 100, 150));
            window.draw(bg1Bottom);

            sf::Text val1(row.val1, font, 19);
            val1.setPosition(boxX + 430.f, dataY + 14.f);
            val1.setOrigin(val1.getLocalBounds().width / 2.f, 0.f);
            val1.setFillColor(sf::Color(255, 255, 255));
            val1.setStyle(sf::Text::Bold);
            window.draw(val1);

            // Colonne 2
            sf::RectangleShape bg2Top(sf::Vector2f(340.f, 26.f));
            bg2Top.setPosition(boxX + 670.f, dataY);
            bg2Top.setFillColor(sf::Color(120, 160, 255, 150));
            window.draw(bg2Top);

            sf::RectangleShape bg2Bottom(sf::Vector2f(340.f, 26.f));
            bg2Bottom.setPosition(boxX + 670.f, dataY + 26.f);
            bg2Bottom.setFillColor(sf::Color(80, 120, 220, 150));
            window.draw(bg2Bottom);

            sf::Text val2(row.val2, font, 19);
            val2.setPosition(boxX + 840.f, dataY + 14.f);
            val2.setOrigin(val2.getLocalBounds().width / 2.f, 0.f);
            val2.setFillColor(sf::Color(255, 255, 255));
            val2.setStyle(sf::Text::Bold);
            window.draw(val2);

            dataY += lineH;
        }
    }

    // Footer
    for (int i = 0; i < 55; ++i) {
        float ratio = static_cast<float>(i) / 55.f;
        sf::RectangleShape footerLine(sf::Vector2f(boxWidth - 40.f, 1.f));
        footerLine.setPosition(boxX + 20.f, boxY + boxHeight - 75.f + i);
        sf::Uint8 alpha = static_cast<sf::Uint8>(200 - 100 * ratio);
        footerLine.setFillColor(sf::Color(50, 100, 180, alpha));
        window.draw(footerLine);
    }

    std::string infoStr;
    if (selectedPlanet1 < 0) {
        infoStr = "Touches 1-8 : Selectionner la PREMIERE planete | ESC : Fermer";
    }
    else if (selectedPlanet2 < 0) {
        infoStr = "Touches 1-8 : Selectionner la DEUXIEME planete | C : Reinitialiser";
    }
    else {
        infoStr = "C : Nouvelle comparaison | ESC : Fermer";
    }

    sf::Text infoText(infoStr, font, 16);
    infoText.setPosition(boxX + boxWidth / 2.f, boxY + boxHeight - 50.f);
    infoText.setOrigin(infoText.getLocalBounds().width / 2.f, 0.f);
    infoText.setFillColor(sf::Color(220, 240, 255));
    infoText.setStyle(sf::Text::Bold);
    window.draw(infoText);

    window.setView(currentView);
}

