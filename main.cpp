#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>

#include "AppState.h"
#include "Constants.h"
#include "Planet.h"
#include "Comet.h"
#include "Star.h"
#include "MenuPlanet.h"
#include "CosmicButton.h"
#include "UI.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "CosmoUIT - Simulateur Solaire 2D");
    window.setFramerateLimit(60);

    // État initial
    AppState currentState = AppState::MAIN_MENU;
    Planet* selectedPlanet = nullptr;

    // Views
    sf::View worldView;
    worldView.reset(sf::FloatRect(-600.f, -400.f, 1200.f, 800.f));
    worldView.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));

    sf::View uiView;
    uiView.reset(sf::FloatRect(0.f, 0.f, 1200.f, 800.f));
    uiView.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));

    // Chargement de la police
    sf::Font font;
    if (!font.loadFromFile(Constants::FONT_PATH)) {
        std::cerr << "Erreur: Police non trouvee." << std::endl;
        return 1;
    }

    // Création des étoiles et du syst?me solaire du menu
    std::vector<Star> stars;
    std::vector<MenuPlanet> menuPlanets;
    createStarfield(stars);
    createMenuSolarSystem(menuPlanets, Constants::MENU_CENTER);

    // Chargement des textures
    std::vector<sf::Texture> planetTextures(8);
    std::vector<std::string> textureFiles = {
        "textures/mercury.jpg", "textures/venus.jpg", "textures/earth.jpg",
        "textures/mars.jpg", "textures/jupiter.jpg", "textures/saturn.jpg",
        "textures/uranus.jpg", "textures/neptune.jpg"
    };
    for (size_t i = 0; i < 8; ++i) {
        if (!planetTextures[i].loadFromFile(textureFiles[i])) {
            std::cerr << "Texture manquante: " << textureFiles[i] << std::endl;
        }
    }

    sf::Texture sunTexture, saturnRingTexture;
    if (!sunTexture.loadFromFile("textures/sun.jpg"))
        std::cerr << "Soleil manquant." << std::endl;
    if (!saturnRingTexture.loadFromFile("textures/saturn_ring.png"))
        std::cerr << "Anneau manquant." << std::endl;

    // Soleil
    sf::CircleShape sun(50.f);
    sun.setOrigin(50.f, 50.f);
    sun.setPosition(Constants::SUN_CENTER);
    sun.setTexture(&sunTexture);

    sf::CircleShape sunGlow(60.f);
    sunGlow.setOrigin(60.f, 60.f);
    sunGlow.setPosition(Constants::SUN_CENTER);
    sunGlow.setFillColor(sf::Color(255, 200, 50, 50));

    // Création des plan?tes
    std::vector<Planet> planets;

    // Dans la section création des planètes, utilisez des caractéristiques plus courtes :

    planets.push_back(Planet(60.f, 0.2f, "Mercure", 0.33f, 0, 4879, "Journée plus longue que son année",
        sf::Color(169, 169, 169), 0, 0.f, "Exosphère ténue: oxygène, sodium, hydrogène",
        167.f, 3.7f, "Antiquité (Babyloniens)",
        { "Plus proche du Soleil", "Aucun satellite", "Surface cratérisée", "Amplitude thermique forte" },
        font, &planetTextures[0]));

    planets.push_back(Planet(120.f, 0.01f, "Vénus", 4.87f, 1, 12104, "Rotation rétrograde unique",
        sf::Color(255, 198, 73), 0, 0.f, "96.5% CO?, 3.5% azote, traces de SO?",
        464.f, 8.87f, "Antiquité",
        { "Atmosphère épaisse", "Effet de serre extrême", "Journée = 243 jours" },
        font, &planetTextures[1]));

    planets.push_back(Planet(150.f, 0.017f, "Terre", 5.97f, 2, 12756, "Seule planète habitée connue",
        sf::Color(100, 149, 237), 1, 0.f, "78% azote, 21% oxygène, 1% argon",
        15.f, 9.8f, "Formation naturelle",
        { "71% surface océanique", "Champ magnétique", "Activité tectonique" },
        font, &planetTextures[2]));

    planets.push_back(Planet(220.f, 0.09f, "Mars", 0.642f, 3, 6792, "Plus hauts volcans du système",
        sf::Color(205, 92, 92), 2, 0.f, "95% CO?, 2.7% azote, 1.6% argon",
        -65.f, 3.7f, "Antiquité",
        { "Saisons similaires Terre", "Calottes polaires", "Possibilité vie passée" },
        font, &planetTextures[3]));

    planets.push_back(Planet(350.f, 0.05f, "Jupiter", 1898.f, 4, 142984, "Plus grande planète du système",
        sf::Color(222, 184, 135), 79, 0.f, "90% hydrogène, 10% hélium, traces",
        -108.f, 24.8f, "Antiquité",
        { "Grande Tache Rouge", "Champ magnétique puissant", "Anneaux ténus" },
        font, &planetTextures[4]));

    planets.push_back(Planet(450.f, 0.05f, "Saturne", 568.f, 5, 120536, "Anneaux spectaculaires visibles",
        sf::Color(238, 232, 170), 82, 26.7f, "96% hydrogène, 3% hélium, 1% autres",
        -139.f, 10.4f, "Antiquité",
        { "Densité < eau", "Anneaux complexes", "Lune Titan importante" },
        font, &planetTextures[5]));

    planets.push_back(Planet(550.f, 0.04f, "Uranus", 86.8f, 6, 51118, "Rotation inclinée à 98°",
        sf::Color(175, 238, 238), 27, 0.f, "83% hydrogène, 15% hélium, 2% méthane",
        -197.f, 8.9f, "William Herschel (1781)",
        { "Axe rotation unique", "Saisons extrêmes", "Anneaux verticaux" },
        font, &planetTextures[6]));

    planets.push_back(Planet(650.f, 0.01f, "Neptune", 102.f, 7, 49528, "Vents les plus rapides (2100 km/h)",
        sf::Color(65, 105, 225), 14, 0.f, "80% hydrogène, 19% hélium, 1% méthane",
        -201.f, 11.2f, "Urbain Le Verrier (1846)",
        { "Découverte mathématique", "Grande Tache Sombre", "Anneaux incomplets" },
        font, &planetTextures[7]));
    // Com?tes
    std::vector<Comet> comets;

    // Variables de simulation
    float speedFactor = 1.f;
    bool paused = false, showOrbits = true, showTrails = true;
    bool showLabels = true, showStats = false, showGrid = false;
    int focusedPlanet = -1;
    float elapsedSimulationTime = 0.f;

    // Boutons du menu principal - DISPOSITION EN LOSANGE
    std::vector<CosmicButton> menuButtons;

    // ✅ COULEURS LUMINEUSES : BLEU NUIT + BLEU CIEL BRILLANT
    sf::Color btnNormalColor = sf::Color(20, 40, 70, 240);
    sf::Color btnHoverColor = sf::Color(100, 180, 230, 255);
    sf::Color btnClickColor = sf::Color(173, 216, 230, 255);

    // ✅ NOUVELLES POSITIONS EN LOSANGE (4 boutons)
    //              [SIMULATION]           <- Haut
    //   [PRESENTATION]    [STRUCTURE]     <- Milieu
    //              [EQUIPE]               <- Bas

    float btnWidth = 250.f;
    float btnHeight = 45.f;
    float screenCenterX = 600.f;
    float topY = 580.f;
    float middleY = 640.f;
    float bottomY = 700.f;
    float horizontalOffset = 280.f;

    // Bouton 1 - LANCER SIMULATION (en haut, centré)
    menuButtons.push_back(CosmicButton(
        sf::Vector2f(screenCenterX - btnWidth / 2.f, topY),
        sf::Vector2f(btnWidth, btnHeight),
        "LANCER SIMULATION",
        btnNormalColor,
        btnHoverColor,
        btnClickColor,
        font,
        [&]() { currentState = AppState::SIMULATION; }
    ));

    // Bouton 2 - PRESENTATION (milieu gauche)
    menuButtons.push_back(CosmicButton(
        sf::Vector2f(screenCenterX - horizontalOffset - btnWidth / 2.f, middleY),
        sf::Vector2f(btnWidth, btnHeight),
        "PRESENTATION",
        btnNormalColor,
        btnHoverColor,
        btnClickColor,
        font,
        [&]() { currentState = AppState::PRESENTATION; }
    ));

    // Bouton 3 - STRUCTURE INTERNE (milieu droite) ✅ NOUVEAU
    menuButtons.push_back(CosmicButton(
        sf::Vector2f(screenCenterX + horizontalOffset - btnWidth / 2.f, middleY),
        sf::Vector2f(btnWidth, btnHeight),
        "STRUCTURE INTERNE",
        btnNormalColor,
        btnHoverColor,
        btnClickColor,
        font,
        [&]() { currentState = AppState::PLANET_STRUCTURE; }
    ));

    // Bouton 4 - EQUIPE (en bas, centré)
    menuButtons.push_back(CosmicButton(
        sf::Vector2f(screenCenterX - btnWidth / 2.f, bottomY),
        sf::Vector2f(btnWidth, btnHeight),
        "EQUIPE",
        btnNormalColor,
        btnHoverColor,
        btnClickColor,
        font,
        [&]() { currentState = AppState::TEAM_INFO; }
    ));
    // Bouton retour (APRÈS les menuButtons)
    std::vector<CosmicButton> backButtons;
    backButtons.push_back(CosmicButton(
        sf::Vector2f(50.f, 700.f),
        sf::Vector2f(150.f, 50.f),
        "RETOUR",
        sf::Color(183, 28, 28, 200),
        sf::Color(198, 40, 40, 200),
        sf::Color(255, 94, 98, 200),
        font,
        [&]() {
            if (currentState == AppState::PLANET_DETAILS) {
                currentState = AppState::SIMULATION;
                selectedPlanet = nullptr;
            }
            else {
                currentState = AppState::MAIN_MENU;
            }
        }
    ));

    // Boutons de simulation
    std::vector<CosmicButton> simButtons;
    float x = 20.f;

   // ✅ APRÈS - Avec clickCol
auto addButton = [&](const std::string& icon, const std::string& label,
    sf::Color normalCol, sf::Color hoverCol, auto&& func, float yPos) {
        simButtons.push_back(CosmicButton(
            sf::Vector2f(x, yPos),
            sf::Vector2f(Constants::BTN_WIDTH, Constants::BTN_HEIGHT),
            icon, label, normalCol, hoverCol,
            sf::Color(255, 94, 98, 200),  // ✅ NOUVEAU clickCol (rose)
            font, func
        ));
        x += Constants::BTN_WIDTH + Constants::BTN_SPACING;
    };

    addButton(">>", "Accelerer", sf::Color(46, 125, 50), sf::Color(56, 142, 60),
        [&]() { speedFactor = std::min(64.f, speedFactor * 2.f); }, Constants::BTN_Y);
    addButton("<<", "Ralentir", sf::Color(198, 40, 40), sf::Color(211, 47, 47),
        [&]() { speedFactor = std::max(0.125f, speedFactor * 0.5f); }, Constants::BTN_Y);
    addButton("||", "Pause", sf::Color(245, 124, 0), sf::Color(251, 140, 0),
        [&]() { paused = !paused; }, Constants::BTN_Y);
    addButton("+", "Zoom +", sf::Color(25, 118, 210), sf::Color(30, 136, 229),
        [&]() { if (worldView.getSize().x > 200.f) worldView.zoom(0.9f); }, Constants::BTN_Y);
    addButton("-", "Zoom -", sf::Color(94, 53, 177), sf::Color(106, 27, 154),
        [&]() { if (worldView.getSize().x < 5000.f) worldView.zoom(1.1f); }, Constants::BTN_Y);
    addButton("O", "Orbites", sf::Color(66, 66, 66), sf::Color(97, 97, 97),
        [&]() { showOrbits = !showOrbits; }, Constants::BTN_Y);
   

    // Deuxi?me ligne
    x = 20.f;
  
    addButton("#", "Grille", sf::Color(69, 90, 100), sf::Color(84, 110, 122),
        [&]() { showGrid = !showGrid; }, Constants::BTN_Y2);
    addButton("C", "Effacer", sf::Color(191, 54, 12), sf::Color(216, 67, 21),
        [&]() {
            for (auto& p : planets) p.clearTrail();
            comets.clear();
        }, Constants::BTN_Y2);
    addButton("R", "Reset", sf::Color(183, 28, 28), sf::Color(198, 40, 40),
        [&]() {
            worldView.setCenter(0.f, 0.f);
            worldView.setSize(1200.f, 800.f);
            focusedPlanet = -1;
        }, Constants::BTN_Y2);
    addButton("M", "Menu", sf::Color(123, 31, 162), sf::Color(142, 36, 170),
        [&]() { currentState = AppState::MAIN_MENU; }, Constants::BTN_Y2);
     addButton("~", "Trainees", sf::Color(0, 137, 123), sf::Color(0, 150, 136),
        [&]() { showTrails = !showTrails; }, Constants::BTN_Y2);
    addButton("T", "Labels", sf::Color(103, 58, 183), sf::Color(123, 31, 162),
        [&]() { showLabels = !showLabels; }, Constants::BTN_Y2);

    // Textes d'information
    sf::Text infoText, speedText;
    infoText.setFont(font);
    infoText.setCharacterSize(16);
    infoText.setPosition(20, 20);
    infoText.setFillColor(sf::Color::Cyan);

    speedText.setFont(font);
    speedText.setCharacterSize(14);
    speedText.setPosition(20, 50);
    speedText.setFillColor(sf::Color::Yellow);

    // Panneau de statistiques
    sf::RectangleShape statsPanel(sf::Vector2f(300.f, 500.f));
    statsPanel.setPosition(880.f, 20.f);
    statsPanel.setFillColor(sf::Color(0, 0, 0, 200));
    statsPanel.setOutlineThickness(2);
    statsPanel.setOutlineColor(sf::Color(100, 181, 246));

    sf::Text statsTitle("=== STATISTIQUES ===", font, 18);
    statsTitle.setPosition(920.f, 30.f);
    statsTitle.setFillColor(sf::Color(100, 181, 246));
    statsTitle.setStyle(sf::Text::Bold);

    sf::Clock clock;

    // BOUCLE PRINCIPALE
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Gestion des clics souris
            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f uiPos = window.mapPixelToCoords(mousePos, uiView);

                if (currentState == AppState::MAIN_MENU) {
                    for (auto& b : menuButtons) {
                        if (b.isClicked(uiPos)) {
                            b.executeAction();
                            break;
                        }
                    }
                }
                else if (currentState == AppState::PRESENTATION ||
                    currentState == AppState::TEAM_INFO ||
                    currentState == AppState::PLANET_DETAILS ||
                    currentState == AppState::PLANET_STRUCTURE) {  // ✅ AJOUTÉ
                    for (auto& b : backButtons) {
                        if (b.isClicked(uiPos)) {
                            b.executeAction();
                            break;
                        }
                    }
                }
                else if (currentState == AppState::SIMULATION) {
                    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos, worldView);

                    bool clicked = false;
                    for (auto& b : simButtons) {
                        if (b.isClicked(uiPos)) {
                            b.executeAction();
                            clicked = true;
                            break;
                        }
                    }

                    if (!clicked) {
                        focusedPlanet = -1;
                        for (size_t i = 0; i < planets.size(); ++i) {
                            if (planets[i].shape.getGlobalBounds().contains(worldPos)) {
                                selectedPlanet = &planets[i];
                                currentState = AppState::PLANET_DETAILS;
                                clicked = true;
                                break;
                            }
                        }

                        if (!clicked && comets.size() < 50) {
                            comets.push_back(Comet(worldPos));
                        }
                    }
                }
            }

            // Gestion du zoom avec la molette
            if (event.type == sf::Event::MouseWheelScrolled &&
                currentState == AppState::SIMULATION) {
                float f = event.mouseWheelScroll.delta > 0 ? 0.9f : 1.1f;
                if ((f < 1.f && worldView.getSize().x > 200.f) ||
                    (f > 1.f && worldView.getSize().x < 5000.f)) {
                    worldView.zoom(f);
                }
            }

            // Touches clavier
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    if (currentState == AppState::PLANET_DETAILS) {
                        currentState = AppState::SIMULATION;
                        selectedPlanet = nullptr;
                    }
                    else if (currentState != AppState::MAIN_MENU) {
                        currentState = AppState::MAIN_MENU;
                    }
                    else {
                        window.close();
                    }
                }

                if (currentState == AppState::SIMULATION) {
                    if (event.key.code == sf::Keyboard::Space)
                        paused = !paused;
                    if (event.key.code == sf::Keyboard::R) {
                        worldView.setCenter(0.f, 0.f);
                        worldView.setSize(1200.f, 800.f);
                        focusedPlanet = -1;
                    }
                }
            }
        }

        // UPDATE
        float dt = clock.restart().asSeconds();
        float time = clock.getElapsedTime().asSeconds();

        sf::Vector2i mouse = sf::Mouse::getPosition(window);
        sf::Vector2f mouseUI = window.mapPixelToCoords(mouse, uiView);

        // Mise ? jour des étoiles
        updateStars(stars, time);

        if (currentState == AppState::MAIN_MENU) {
            for (auto& b : menuButtons) {
                b.update(mouseUI);
            }
            updateMenuSolarSystem(menuPlanets, dt, Constants::MENU_CENTER);
        }
        else if (currentState == AppState::PRESENTATION ||
            currentState == AppState::TEAM_INFO ||
            currentState == AppState::PLANET_DETAILS ||
            currentState == AppState::PLANET_STRUCTURE) {  // ✅ AJOUTÉ
            for (auto& b : backButtons) {
                b.update(mouseUI);
            }
        }
        else if (currentState == AppState::SIMULATION) {
            // Hover boutons
            for (auto& b : simButtons) {
                b.update(mouseUI);
            }

            // Déplacement caméra
            sf::Vector2f move(0, 0);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                move.x -= Constants::PAN_SPEED * dt;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                move.x += Constants::PAN_SPEED * dt;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                move.y -= Constants::PAN_SPEED * dt;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                move.y += Constants::PAN_SPEED * dt;
            if (move.x != 0 || move.y != 0)
                focusedPlanet = -1;

            // Simulation
            float deltaTime = dt * speedFactor * (paused ? 0 : 1);
            if (!paused)
                elapsedSimulationTime += deltaTime;

            // Mise ? jour plan?tes
            for (auto& p : planets) {
                p.update(deltaTime, Constants::SUN_CENTER, Constants::G,
                    Constants::MASS_SUN, showTrails);
            }

            // Mise ? jour com?tes
            for (auto& c : comets) {
                c.update(deltaTime, Constants::SUN_CENTER, Constants::G,
                    Constants::MASS_SUN, showTrails);
            }

            comets.erase(std::remove_if(comets.begin(), comets.end(),
                [](const Comet& c) { return !c.isActive(); }), comets.end());

            // Focus plan?te
            if (focusedPlanet >= 0 && focusedPlanet < planets.size()) {
                worldView.setCenter(planets[focusedPlanet].getPosition());
                const auto& p = planets[focusedPlanet];
                float dist = std::hypot(p.getPosition().x - Constants::SUN_CENTER.x,
                    p.getPosition().y - Constants::SUN_CENTER.y);
                float speed = std::hypot(p.velocity.x, p.velocity.y);
                std::ostringstream oss;
                oss << p.getName() << " | Dist: " << std::fixed
                    << std::setprecision(1) << dist << " u | Vit: " << speed
                    << " u/s | Diam: " << p.realDiameterKm << " km | " << p.fact;
                infoText.setString(oss.str());
            }
           
        }

        // RENDER
        window.clear(sf::Color(5, 5, 15));

        switch (currentState) {
        case AppState::MAIN_MENU:
            drawMainMenu(window, font, menuButtons, stars, menuPlanets,
                Constants::MENU_CENTER);
            break;

        case AppState::PRESENTATION:
            drawPresentation(window, font, backButtons, stars, time);
            break;

        case AppState::TEAM_INFO:
            drawTeamInfo(window, font, backButtons, stars, time);
            break;

        case AppState::PLANET_STRUCTURE:  // ✅ NOUVEAU
            drawPlanetStructure(window, font, backButtons, stars, time);
            break;

        case AppState::PLANET_DETAILS:
            if (selectedPlanet) {
                drawPlanetDetails(window, font, backButtons, stars, time,
                    *selectedPlanet);
            }
            break;

        case AppState::SIMULATION:
            drawSimulation(window, font, worldView, uiView, stars, time,
                sun, sunGlow, planets, comets, simButtons,
                infoText, speedText, showOrbits, showTrails,
                showLabels, showStats, showGrid, statsPanel,
                statsTitle, saturnRingTexture);
            break;
        }

        window.display();
    }

    return 0;
}