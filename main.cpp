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
#include "SpaceMission.h"  
#include "EducationalMode.h"  

// Variable globale pour gérer l'affichage du message des planètes
static bool showPlanetsOrderMessage = false;
//  Variables pour le mode comparaison
static bool showPlanetComparison = false;
static int comparisonPlanet1 = -1;
static int comparisonPlanet2 = -1;

int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "CosmoUIT - Simulateur Solaire 2D");
    window.setFramerateLimit(60);

    //  la déclaration du font
    sf::Font font;
    if (!font.loadFromFile(Constants::FONT_PATH)) {
        std::cerr << "Erreur: Police non trouvee." << std::endl;
        return 1;
    }

    // État initial
    AppState currentState = AppState::MAIN_MENU;
    Planet* selectedPlanet = nullptr;

  //  Mode Éducatif 
    EducationalMode educationalMode(font);

    // Views
    sf::View worldView;
    worldView.reset(sf::FloatRect(-600.f, -400.f, 1200.f, 800.f));
    worldView.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));

    sf::View uiView;
    uiView.reset(sf::FloatRect(0.f, 0.f, 1200.f, 800.f));
    uiView.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));

    // Création des étoiles et du système solaire du menu
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
    if (!sunTexture.loadFromFile("textures/sun.png"))
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
    // Cometes
    std::vector<Comet> comets;

    // Variables de simulation
    float speedFactor = 1.f;
    bool paused = false, showOrbits = true, showTrails = true;
  bool showLabels = true, showStats = false, showGrid = false;
    int focusedPlanet = -1;
    float elapsedSimulationTime = 0.f;

    //  Timeline des Missions Spatiales
    MissionTimeline missionTimeline;
  const SpaceMission* hoveredMission = nullptr;
    bool showMissionTimeline = true;
    bool showMissionPanel = false;  //  Missions masquées par défaut - cliquer sur "H" pour afficher

  //  Menu déroulant pour les boutons
    bool toolMenuOpen = false;
    float toolMenuAnimation = 0.f;


    // Mode Suivi Planétiaire
    bool cameraFollowMode = false;
    int followedPlanetIndex = -1;
    float smoothCameraLerp = 0.08f;  // Vitesse de suivi (plus petit = plus fluide)

    // Boutons du menu principal - DISPOSITION EN LOSANGE
    std::vector<CosmicButton> menuButtons;

    // ===== BOUTONS DU MENU - DISPOSITION HORIZONTALE AVEC COULEURS BRILLANTES =====

     //  COULEURS BLEU SOMBRE MAIS LUMINEUX (Bleu nuit brillant)
    sf::Color btnNormalColor = sf::Color(40, 80, 140, 245);     // Bleu nuit lumineux
    sf::Color btnHoverColor = sf::Color(60, 120, 200, 255);     // Bleu royal brillant
    sf::Color btnClickColor = sf::Color(100, 160, 240, 255);    // Bleu ciel au clic


    // DIMENSIONS AUGMENTÉES
    float btnWidth = 250.f;   // Plus large (était 200)
    float btnHeight = 55.f;   // Plus haut (était 45)
    float btnSpacing = 20.f;  // Espacement augmenté
    float totalWidth = 4 * btnWidth + 3 * btnSpacing;
    float startX = (1200.f - totalWidth) / 2.f;  // Centré sur écran 1200px
    float btnY = 600.f;  // Position Y pour tous les boutons

    menuButtons.clear();

    // Bouton 1 : CADRE DU PROJET
    menuButtons.push_back(CosmicButton(
        sf::Vector2f(startX, btnY),
        sf::Vector2f(btnWidth, btnHeight),
        "CADRE DU PROJET",
        btnNormalColor, btnHoverColor, btnClickColor,
        font, [&]() { currentState = AppState::PRESENTATION; }
    ));

    // Bouton 2 : ACCÈS SIMULATEUR
    menuButtons.push_back(CosmicButton(
        sf::Vector2f(startX + btnWidth + btnSpacing, btnY),
        sf::Vector2f(btnWidth, btnHeight),
        "ACCES SIMULATEUR",
        btnNormalColor, btnHoverColor, btnClickColor,
        font, [&]() { currentState = AppState::SIMULATION; }
    ));

    // Bouton 3 : ÉQUIPE PÉDAGOGIQUE
    menuButtons.push_back(CosmicButton(
        sf::Vector2f(startX + 2 * (btnWidth + btnSpacing), btnY),
        sf::Vector2f(btnWidth, btnHeight),
        "EQUIPE PEDAGOGIQUE",
        btnNormalColor, btnHoverColor, btnClickColor,
        font, [&]() { currentState = AppState::TEAM_INFO; }
    ));

    // Bouton 4 : STRUCTURE INTERNE
    menuButtons.push_back(CosmicButton(
        sf::Vector2f(startX + 3 * (btnWidth + btnSpacing), btnY),
        sf::Vector2f(btnWidth, btnHeight),
        "STRUCTURE INTERNE",
        btnNormalColor, btnHoverColor, btnClickColor,
        font, [&]() { currentState = AppState::PLANET_STRUCTURE; }
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

    // ===== MENU DÉROULANT DES OUTILS =====
    std::vector<CosmicButton> simButtons;
    
    // Position du menu (en haut à droite)
    float menuX = 1070.f;
    float menuY = 160.f;
    float menuBtnW = 120.f;
    float menuBtnH = 35.f;
    float menuSpacing = 5.f;
    
    // ===== PALETTE COSMIQUE RÉALISTE =====
    // Inspirée des couleurs du système solaire et de l'espace
    // ===== PALETTE COSMIQUE CLAIRE ET RÉALISTE =====
    struct CosmicPalette {
        // Couleurs principales - PLUS CLAIRES ET VISIBLES
        sf::Color solarCorona{ 255, 210, 100, 245 };      // Jaune solaire lumineux
        sf::Color solarFlare{ 255, 160, 80, 245 };        // Orange clair
        sf::Color marsRust{ 220, 120, 100, 245 };         // Rouge Mars clair
        sf::Color neptuneBlue{ 100, 150, 220, 245 };      // Bleu Neptune lumineux
        sf::Color uranusAqua{ 120, 210, 230, 245 };       // Cyan Uranus clair
        sf::Color saturnGold{ 240, 210, 140, 245 };       // Or Saturne lumineux
        sf::Color jupiterBrown{ 210, 180, 140, 245 };     // Brun Jupiter clair
        sf::Color venusAmber{ 250, 220, 130, 245 };       // Ambre Vénus lumineux
        sf::Color mercuryGray{ 180, 180, 190, 245 };      // Gris Mercure clair
        sf::Color deepSpace{ 60, 80, 130, 245 };          // Bleu espace lumineux
        sf::Color nebulaViolet{ 160, 130, 200, 245 };     // Violet nébuleuse clair
        sf::Color starWhite{ 255, 255, 255, 255 };        // Blanc pur

        // Couleurs hover (encore plus lumineuses)
        sf::Color solarCoronaHover{ 255, 230, 150, 255 };
        sf::Color solarFlareHover{ 255, 190, 120, 255 };
        sf::Color marsRustHover{ 240, 150, 130, 255 };
        sf::Color neptuneBlueHover{ 130, 180, 250, 255 };
        sf::Color uranusAquaHover{ 150, 235, 255, 255 };
        sf::Color saturnGoldHover{ 255, 235, 180, 255 };
        sf::Color jupiterBrownHover{ 235, 210, 175, 255 };
        sf::Color venusAmberHover{ 255, 245, 170, 255 };
        sf::Color mercuryGrayHover{ 210, 210, 220, 255 };
        sf::Color deepSpaceHover{ 90, 120, 180, 255 };
        sf::Color nebulaVioletHover{ 200, 170, 240, 255 };
        sf::Color starWhiteHover{ 255, 255, 255, 255 };
    } cosmic;
    
    // Bouton principal "OUTILS" (style nébuleuse)
    CosmicButton toolsMainButton(
 sf::Vector2f(menuX, menuY - 50.f),
      sf::Vector2f(menuBtnW, 40.f),
  "OUTILS",
      cosmic.deepSpace,
   cosmic.deepSpaceHover,
        sf::Color(100, 150, 255, 255),
        font,
        [&]() { 
            toolMenuOpen = !toolMenuOpen; 
            toolMenuAnimation = 0.f;
  }
    );
    
    // Fonction helper pour ajouter un bouton au menu
    float currentY = menuY;
    auto addToolButton = [&](const std::string& icon, const std::string& label,
   sf::Color normalCol, sf::Color hoverCol, sf::Color clickCol, auto&& func) {
   simButtons.push_back(CosmicButton(
   sf::Vector2f(menuX, currentY),
      sf::Vector2f(menuBtnW, menuBtnH),
     icon, label, normalCol, hoverCol,
          clickCol,
      font, func
            ));
currentY += menuBtnH + menuSpacing;
        };
    
    // ===== BOUTONS DU MENU AVEC PALETTE COSMIQUE =====
    
    // Contrôle vitesse - Couleurs solaires (jaune/orange)
    addToolButton(">>", "Accelerer", 
cosmic.solarCorona, cosmic.solarCoronaHover, cosmic.starWhite,
        [&]() { speedFactor = std::min(64.f, speedFactor * 2.f); });
    
    addToolButton("<<", "Ralentir", 
      cosmic.solarFlare, cosmic.solarFlareHover, cosmic.starWhite,
        [&]() { speedFactor = std::max(0.125f, speedFactor * 0.5f); });
    
 // Pause - Couleur Mars (rouge profond)
    addToolButton("||", "Pause", 
        cosmic.marsRust, cosmic.marsRustHover, cosmic.starWhite,
        [&]() { paused = !paused; });
    
    // Zoom - Couleurs Neptune/Uranus (bleus)
    addToolButton("+", "Zoom +", 
        cosmic.neptuneBlue, cosmic.neptuneBlueHover, cosmic.starWhite,
  [&]() { if (worldView.getSize().x > 200.f) worldView.zoom(0.9f); });
    
    addToolButton("-", "Zoom -", 
        cosmic.uranusAqua, cosmic.uranusAquaHover, cosmic.starWhite,
   [&]() { if (worldView.getSize().x < 5000.f) worldView.zoom(1.1f); });
    
    // Affichage - Couleurs Saturne/Jupiter (or/brun)
    addToolButton("O", "Orbites", 
 cosmic.saturnGold, cosmic.saturnGoldHover, cosmic.starWhite,
[&]() { showOrbits = !showOrbits; });
    
    addToolButton("~", "Trainees", 
        cosmic.jupiterBrown, cosmic.jupiterBrownHover, cosmic.starWhite,
        [&]() { showTrails = !showTrails; });
    
    addToolButton("T", "Labels", 
        cosmic.venusAmber, cosmic.venusAmberHover, cosmic.starWhite,
        [&]() { showLabels = !showLabels; });
    
    // Grille - Couleur Mercure (gris métallique)
    addToolButton("#", "Grille", 
        cosmic.mercuryGray, cosmic.mercuryGrayHover, cosmic.starWhite,
        [&]() { showGrid = !showGrid; });
  
    // Actions - Couleurs nébuleuse (violet/profond)
    addToolButton("C", "Effacer", 
        cosmic.nebulaViolet, cosmic.nebulaVioletHover, cosmic.starWhite,
        [&]() {
   for (auto& p : planets) p.clearTrail();
            comets.clear();
        });
    
    addToolButton("R", "Reset", 
   sf::Color(160, 60, 80, 230), sf::Color(200, 80, 100, 255), cosmic.starWhite,
    [&]() {
            worldView.setCenter(0.f, 0.f);
   worldView.setSize(1200.f, 800.f);
    focusedPlanet = -1;
     });
    
    addToolButton("M", "Menu", 
        cosmic.deepSpace, cosmic.deepSpaceHover, cosmic.starWhite,
        [&]() { currentState = AppState::MAIN_MENU; });
    
    //  NOUVEAU : Bouton Suivi Planétaire AVEC MESSAGE
    addToolButton("S", "Suivre",
        sf::Color(100, 200, 150, 230), sf::Color(130, 230, 180, 255), cosmic.starWhite,
        [&]() {
            //  AJOUTER CES LIGNES :
            showPlanetsOrderMessage = true;  // Afficher le message des planètes
            std::cout << "=== ORDRE DES PLANETES ===" << std::endl;
            std::cout << "1. Mercure (57.9 millions km)" << std::endl;
            std::cout << "2. Venus (108.2 millions km)" << std::endl;
            std::cout << "3. Terre (149.6 millions km)" << std::endl;
            std::cout << "4. Mars (227.9 millions km)" << std::endl;
            std::cout << "5. Jupiter (778.5 millions km)" << std::endl;
            std::cout << "6. Saturne (1.434 milliards km)" << std::endl;
            std::cout << "7. Uranus (2.873 milliards km)" << std::endl;
            std::cout << "8. Neptune (4.495 milliards km)" << std::endl;

            // Code existant pour le suivi
            if (focusedPlanet >= 0) {
                cameraFollowMode = !cameraFollowMode;
                if (cameraFollowMode) {
                    followedPlanetIndex = focusedPlanet;
                }
                else {
                    followedPlanetIndex = -1;
                }
            }
        });
    
    // Bouton Timeline Missions
    addToolButton("H", "Missions",
        sf::Color(200, 100, 200, 230), sf::Color(230, 130, 230, 255), cosmic.starWhite,
        [&]() {
            //  basculer l'affichage du panneau
            showMissionPanel = !showMissionPanel;

            // Réinitialiser la mission survolée quand on ferme le panneau
            if (!showMissionPanel) {
                hoveredMission = nullptr;
            }
        });
    //  Bouton Quiz Éducatif
    addToolButton("Q", "Quiz",
        sf::Color(255, 150, 0, 230), sf::Color(255, 180, 50, 255), cosmic.starWhite,
        [&]() {
            educationalMode.startQuiz();
        });

    //  Bouton Comparaison de Planètes
    addToolButton("P", "Compare",
        sf::Color(100, 180, 255, 230), sf::Color(140, 210, 255, 255), cosmic.starWhite,
        [&]() {
            // Réinitialiser la sélection
            comparisonPlanet1 = -1;
            comparisonPlanet2 = -1;
            showPlanetComparison = true;
        });

    // Textes d'information - PLUS VISIBLES
    sf::Text infoText, speedText;
    infoText.setFont(font);
    infoText.setCharacterSize(18);  
    infoText.setPosition(20, 20);
    infoText.setFillColor(sf::Color(150, 230, 255));  // Cyan clair
    infoText.setOutlineThickness(1.f);  // Contour pour lisibilité
    infoText.setOutlineColor(sf::Color(0, 50, 100, 180));

    speedText.setFont(font);
    speedText.setCharacterSize(16);  // Plus grand
    speedText.setPosition(20, 50);
    speedText.setFillColor(sf::Color(255, 230, 130));  // Jaune clair
    speedText.setOutlineThickness(1.f);
    speedText.setOutlineColor(sf::Color(100, 80, 0, 180));

    // Panneau de statistiques - PLUS CLAIR
    sf::RectangleShape statsPanel(sf::Vector2f(300.f, 500.f));
    statsPanel.setPosition(880.f, 20.f);
    statsPanel.setFillColor(sf::Color(20, 40, 80, 230));  
    statsPanel.setOutlineThickness(3);
    statsPanel.setOutlineColor(sf::Color(100, 180, 255));  

    sf::Text statsTitle("=== STATISTIQUES ===", font, 20);  
    statsTitle.setPosition(920.f, 30.f);
    statsTitle.setFillColor(sf::Color(150, 220, 255));  
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

                
                if (showPlanetComparison) {
                    // Créer la vue par défaut pour la détection
                    sf::View defaultView(sf::FloatRect(0.f, 0.f,
                        static_cast<float>(window.getSize().x),
                        static_cast<float>(window.getSize().y)));
                    sf::Vector2f defaultMousePos = window.mapPixelToCoords(mousePos, defaultView);

                    sf::Vector2u windowSize = window.getSize();
                    float boxWidth = 1000.f;  //  Taille augmentée
                    float boxX = (static_cast<float>(windowSize.x) - boxWidth) / 2.f;
                    float boxY = (static_cast<float>(windowSize.y) - 680.f) / 2.f;  

                    sf::FloatRect returnBtnBounds(boxX + 20.f, boxY + 15.f, 120.f, 40.f);
                    if (returnBtnBounds.contains(defaultMousePos)) {
                        showPlanetComparison = false;
                        comparisonPlanet1 = -1;
                        comparisonPlanet2 = -1;
                        continue;
                    }
                }

                sf::Vector2f uiPos = window.mapPixelToCoords(mousePos, uiView);

                if (showPlanetsOrderMessage) {
                    showPlanetsOrderMessage = false;
                    continue;
                }

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
       currentState == AppState::PLANET_STRUCTURE) {
   for (auto& b : backButtons) {
       if (b.isClicked(uiPos)) {
        b.executeAction();
 break;
      }
           }
     }
                else if (currentState == AppState::SIMULATION) {
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos, worldView);
    sf::Vector2f uiPos = window.mapPixelToCoords(mousePos, uiView);
    bool clicked = false;

    // Gérer les clics du quiz en PREMIER
    if (educationalMode.isQuizActive()) {
        //  Mettre à jour le bouton de retour
        educationalMode.updateReturnButton(uiPos);
        if (educationalMode.handleQuizClick(uiPos)) {
            clicked = true;
        }
    }

 // Vérifier le bouton principal OUTILS
 if (!clicked && toolsMainButton.isClicked(uiPos)) {
        toolsMainButton.executeAction();
        clicked = true;
    }

    // Vérifier les boutons du menu
    if (!clicked && toolMenuOpen) {
        for (auto& b : simButtons) {
            if (b.isClicked(uiPos)) {
                b.executeAction();
       clicked = true;
       break;
  }
  }
  }

    // Clic sur les planètes
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

     // Créer une comète si aucun clic
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
      cameraFollowMode = false;
                 followedPlanetIndex = -1;
   }
      //  Gestion du mode comparaison
      if (showPlanetComparison) {
          // Touches 1-8 pour sélectionner les planètes
          if (event.key.code >= sf::Keyboard::Num1 &&
              event.key.code <= sf::Keyboard::Num8) {
              int planetIdx = event.key.code - sf::Keyboard::Num1;

              if (comparisonPlanet1 < 0) {
                  comparisonPlanet1 = planetIdx;
              }
              else if (comparisonPlanet2 < 0 && planetIdx != comparisonPlanet1) {
                  comparisonPlanet2 = planetIdx;
              }
          }

          // Touche C pour recommencer
          if (event.key.code == sf::Keyboard::C) {
              comparisonPlanet1 = -1;
              comparisonPlanet2 = -1;
          }

          // ESC pour fermer
          if (event.key.code == sf::Keyboard::Escape) {
              showPlanetComparison = false;
              comparisonPlanet1 = -1;
              comparisonPlanet2 = -1;
          }
      }
  if (event.key.code == sf::Keyboard::Tab) {
       toolMenuOpen = !toolMenuOpen;
  }
      
   //  Touche F pour activer/désactiver le suivi
 if (event.key.code == sf::Keyboard::F) {
         if (focusedPlanet >= 0 && focusedPlanet < static_cast<int>(planets.size())) {
      cameraFollowMode = !cameraFollowMode;
         if (cameraFollowMode) {
      followedPlanetIndex = focusedPlanet;
     } else {
            followedPlanetIndex = -1;
              }
         }
}
 
         //   Touches 1-8 pour suivre directement une planète
    if (event.key.code >= sf::Keyboard::Num1 && event.key.code <= sf::Keyboard::Num8) {
  int planetIdx = event.key.code - sf::Keyboard::Num1;
            if (planetIdx < static_cast<int>(planets.size())) {
        cameraFollowMode = true;
       followedPlanetIndex = planetIdx;
    focusedPlanet = planetIdx;
         }
  }
   
      // Touche 0 pour revenir au Soleil
         if (event.key.code == sf::Keyboard::Num0) {
      cameraFollowMode = false;
          followedPlanetIndex = -1;
  focusedPlanet = -1;
    worldView.setCenter(Constants::SUN_CENTER);
      }
        
      //  Touche H pour afficher/masquer les missions
        if (event.key.code == sf::Keyboard::H) {
   showMissionPanel = !showMissionPanel;
    }
   }
     }
        }

        // UPDATE
        float dt = clock.restart().asSeconds();
        float time = clock.getElapsedTime().asSeconds();

  sf::Vector2i mouse = sf::Mouse::getPosition(window);
        sf::Vector2f mouseUI = window.mapPixelToCoords(mouse, uiView);

        // Mise à jour des étoiles
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
      currentState == AppState::PLANET_STRUCTURE) {
   for (auto& b : backButtons) {
      b.update(mouseUI);
    }
        }
        else if (currentState == AppState::SIMULATION) {
    // Mise à jour du bouton principal OUTILS
       toolsMainButton.update(mouseUI);
     
   // Animation d'ouverture/fermeture du menu
 if (toolMenuOpen) {
        toolMenuAnimation = std::min(1.f, toolMenuAnimation + dt * 5.f);
            } else {
      toolMenuAnimation = std::max(0.f, toolMenuAnimation - dt * 5.f);
            }

            // Mise à jour des boutons du menu (seulement si visible)
            if (toolMenuOpen) {
      for (auto& b : simButtons) {
  b.update(mouseUI);
   }
            }
           
            if (educationalMode.isQuizActive()) {
                educationalMode.updateReturnButton(mouseUI);
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
    
   // Si déplacement manuel, désactiver le suivi
            if (move.x != 0 || move.y != 0) {
       focusedPlanet = -1;
    cameraFollowMode = false;
    followedPlanetIndex = -1;
         worldView.move(move);
       }
            
            //  Mode suivi planétaire avec interpolation fluide
   if (cameraFollowMode && followedPlanetIndex >= 0 && 
    followedPlanetIndex < static_cast<int>(planets.size())) {
   
           sf::Vector2f targetPos = planets[followedPlanetIndex].getPosition();
     sf::Vector2f currentCenter = worldView.getCenter();
 
          // Interpolation linéaire (lerp) pour un mouvement fluide
       sf::Vector2f newCenter;
    newCenter.x = currentCenter.x + (targetPos.x - currentCenter.x) * smoothCameraLerp;
  newCenter.y = currentCenter.y + (targetPos.y - currentCenter.y) * smoothCameraLerp;
     
      worldView.setCenter(newCenter);
      
     // Mettre à jour l'infoText avec les infos de la planète suivie
         const auto& p = planets[followedPlanetIndex];
      float dist = std::hypot(p.getPosition().x - Constants::SUN_CENTER.x,
         p.getPosition().y - Constants::SUN_CENTER.y);
             float speed = std::hypot(p.velocity.x, p.velocity.y);
   std::ostringstream oss;
             oss << "[SUIVI] " << p.getName() << " | Dist: " << std::fixed
   << std::setprecision(1) << dist << " u | Vit: " << speed << " u/s";
                infoText.setString(oss.str());
     }

       // Simulation
            float deltaTime = dt * speedFactor * (paused ? 0 : 1);
   if (!paused)
   elapsedSimulationTime += deltaTime;

      // Mise à jour planètes
  for (auto& planet : planets) {
      planet.update(deltaTime, Constants::SUN_CENTER, Constants::G,
  Constants::MASS_SUN, showTrails);
      planet.updateMoons(deltaTime);
    }
    
  //  Mise à jour de la timeline des missions
  missionTimeline.update(deltaTime);

  //  Vérification si la souris survole une mission
  if (showMissionPanel && missionTimeline.isTimelineVisible()) {
      sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
      sf::Vector2f worldMousePos = window.mapPixelToCoords(mousePixel, worldView);
      hoveredMission = missionTimeline.checkMissionHover(worldMousePos, planets,
          Constants::SUN_CENTER);
  }
  else {
      hoveredMission = nullptr;
  }

  // Mise à jour comètes
            for (auto& c : comets) {
                c.update(deltaTime, Constants::SUN_CENTER, Constants::G,
       Constants::MASS_SUN, showTrails);
     }

       comets.erase(std::remove_if(comets.begin(), comets.end(),
           [](const Comet& c) { return !c.isActive(); }), comets.end());

      // Focus planète (mode ancien)
  if (!cameraFollowMode && focusedPlanet >= 0 && 
     focusedPlanet < static_cast<int>(planets.size())) {
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

        case AppState::PLANET_STRUCTURE:
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
                toolsMainButton, toolMenuOpen, toolMenuAnimation,
                infoText, speedText, showOrbits, showTrails,
                showLabels, showStats, showGrid, statsPanel,
                statsTitle, saturnRingTexture,
                cameraFollowMode, followedPlanetIndex,
                &missionTimeline, hoveredMission, showMissionPanel,
                &educationalMode);  
   break;
    }



  // Dans la boucle de rendu, APRÈS window.clear() et les autres dessins :

  if (showPlanetsOrderMessage) {
      //  la vue par défaut pour centrer correctement
      sf::View defaultView(sf::FloatRect(0.f, 0.f,
          static_cast<float>(window.getSize().x),
          static_cast<float>(window.getSize().y)));
      window.setView(defaultView);

      drawPlanetsOrderMessage(window, font);

      // Restaurer la vue UI après
      window.setView(uiView);
  }
  //  Afficher la comparaison de planètes
  if (showPlanetComparison) {

      drawPlanetComparison(window, font, planets,
          comparisonPlanet1, comparisonPlanet2,
          showPlanetComparison);
  }

  window.display();
    }

    return 0;
}