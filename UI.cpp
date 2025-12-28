#include "UI.h"
#include "Comet.h"
#include "Constants.h"
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <iomanip>

void createStarfield(std::vector<Star>& stars) {
    stars.clear();
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (int i = 0; i < 1000; ++i) {
        stars.push_back(Star());
    }
}

void createMenuSolarSystem(std::vector<MenuPlanet>& menuPlanets, const sf::Vector2f& center) {
    menuPlanets.clear();

    std::vector<std::pair<float, sf::Color>> planetData = {
        {80.f, sf::Color(169, 169, 169)},   // Mercure
        {120.f, sf::Color(255, 198, 73)},   // Venus
        {160.f, sf::Color(100, 149, 237)},  // Terre
        {200.f, sf::Color(205, 92, 92)},    // Mars
        {260.f, sf::Color(222, 184, 135)},  // Jupiter
        {320.f, sf::Color(238, 232, 170)},  // Saturne
        {380.f, sf::Color(175, 238, 238)},  // Uranus
        {440.f, sf::Color(65, 105, 225)}    // Neptune
    };

    for (const auto& data : planetData) {
        menuPlanets.push_back(MenuPlanet(data.first, data.second, center));
    }
}

void updateStars(std::vector<Star>& stars, float time) {
    for (auto& s : stars) {
        s.update(time);
    }
}

void updateMenuSolarSystem(std::vector<MenuPlanet>& menuPlanets, float deltaTime, const sf::Vector2f& center) {
    for (auto& mp : menuPlanets) {
        mp.update(deltaTime, center);
    }
}

void drawMainMenu(sf::RenderWindow& window, const sf::Font& font,
    std::vector<CosmicButton>& menuButtons,
    std::vector<Star>& stars,
    std::vector<MenuPlanet>& menuPlanets,
    const sf::Vector2f& center) {

    // Fond bleu foncé
    window.clear(sf::Color(0, 0, 50));



   

    // Fond étoilé
    for (auto& s : stars) {
        s.draw(window);
    }

    // Nébuleuses colorées
    sf::CircleShape nebula1(300.f);
    nebula1.setPosition(150.f, 100.f);
    nebula1.setFillColor(sf::Color(138, 43, 226, 30));
    window.draw(nebula1);

    sf::CircleShape nebula2(250.f);
    nebula2.setPosition(750.f, 400.f);
    nebula2.setFillColor(sf::Color(30, 144, 255, 25));
    window.draw(nebula2);

    // Système solaire du menu
    for (auto& mp : menuPlanets) {
        mp.draw(window);
    }

    // Soleil amélioré
    sf::CircleShape sunGlowOuter(65.f);
    sunGlowOuter.setOrigin(65.f, 65.f);
    sunGlowOuter.setPosition(center);
    sunGlowOuter.setFillColor(sf::Color(255, 165, 0, 60));
    window.draw(sunGlowOuter);

    sf::CircleShape sunGlowMiddle(55.f);
    sunGlowMiddle.setOrigin(55.f, 55.f);
    sunGlowMiddle.setPosition(center);
    sunGlowMiddle.setFillColor(sf::Color(255, 215, 0, 120));
    window.draw(sunGlowMiddle);

    sf::CircleShape sun(45.f);
    sun.setOrigin(45.f, 45.f);
    sun.setPosition(center);
    sun.setFillColor(sf::Color(255, 255, 100));
    sun.setOutlineThickness(4);
    sun.setOutlineColor(sf::Color(255, 100, 0));
    window.draw(sun);

    // Éruptions solaires animées
    static float solarFlareAngle = 0.f;
    solarFlareAngle += 0.015f;

    for (int i = 0; i < 8; ++i) {
        float angle = solarFlareAngle + i * 3.14159f / 4.f;
        sf::ConvexShape flare;
        flare.setPointCount(4);
        flare.setPoint(0, sf::Vector2f(0, -4));
        flare.setPoint(1, sf::Vector2f(70, -2));
        flare.setPoint(2, sf::Vector2f(70, 2));
        flare.setPoint(3, sf::Vector2f(0, 4));
        flare.setFillColor(sf::Color(255, 80, 0, 150));
        flare.setPosition(center);
        flare.setRotation(angle * 180.f / 3.14159f);
        window.draw(flare);
    }

    // Titre principal
    sf::Text titleShadow("COSMO UIT", font, 72);
    titleShadow.setPosition(604.f, 204.f);
    titleShadow.setOrigin(titleShadow.getLocalBounds().width / 2, titleShadow.getLocalBounds().height / 2);
    titleShadow.setFillColor(sf::Color(0, 0, 0, 150));
    titleShadow.setStyle(sf::Text::Bold);
    window.draw(titleShadow);

    sf::Text title("COSMO UIT", font, 72);
    title.setPosition(600.f, 200.f);
    title.setOrigin(title.getLocalBounds().width / 2, title.getLocalBounds().height / 2);
    title.setFillColor(sf::Color(0, 255, 255));
    title.setStyle(sf::Text::Bold);
    title.setOutlineThickness(4);
    title.setOutlineColor(sf::Color(0, 150, 255));
    window.draw(title);

    // Sous-titre animé
    static float subtitlePulse = 0.f;
    subtitlePulse += 0.04f;
    float alpha = 150 + 105 * std::sin(subtitlePulse);

    sf::Text subtitle("SIMULATEUR SOLAIRE 2D", font, 36);
    subtitle.setPosition(600.f, 290.f);
    subtitle.setOrigin(subtitle.getLocalBounds().width / 2, subtitle.getLocalBounds().height / 2);
    subtitle.setFillColor(sf::Color(255, 255, 0, static_cast<sf::Uint8>(alpha)));
    subtitle.setStyle(sf::Text::Bold);
    window.draw(subtitle);

    // Informations institutionnelles
    sf::Text university("UNIVERSITE IBN TOFAIL", font, 24);
    university.setPosition(600.f, 350.f);
    university.setOrigin(university.getLocalBounds().width / 2, university.getLocalBounds().height / 2);
    university.setFillColor(sf::Color(200, 200, 255));
    university.setStyle(sf::Text::Bold);
    window.draw(university);

    sf::Text faculty("Faculte des Sciences - Departement d'Informatique", font, 20);
    faculty.setPosition(600.f, 385.f);
    faculty.setOrigin(faculty.getLocalBounds().width / 2, faculty.getLocalBounds().height / 2);
    faculty.setFillColor(sf::Color(180, 180, 220));
    window.draw(faculty);



    // Boutons avec ombres
    for (auto& b : menuButtons) {
        sf::RectangleShape shadow = b.base;
        shadow.move(5, 5);
        shadow.setFillColor(sf::Color(0, 0, 0, 100));
        window.draw(shadow);

        b.draw(window);
    }
}

void drawPresentation(sf::RenderWindow& window, const sf::Font& font,
    std::vector<CosmicButton>& backButtons,
    std::vector<Star>& stars, float time) {

    // Fond spatial professionnel
    window.clear(sf::Color(8, 12, 30));

    // Fond étoilé
    for (auto& s : stars) {
        s.draw(window);
    }

    // Panneau principal avec effet moderne
    sf::RectangleShape mainPanel(sf::Vector2f(1100.f, 650.f));
    mainPanel.setPosition(50.f, 70.f);
    mainPanel.setFillColor(sf::Color(15, 25, 45, 230));
    mainPanel.setOutlineThickness(4);
    mainPanel.setOutlineColor(sf::Color(0, 180, 255));
    window.draw(mainPanel);

    // En-tête dégradé
    sf::RectangleShape header(sf::Vector2f(1100.f, 90.f));
    header.setPosition(50.f, 70.f);
    header.setFillColor(sf::Color(0, 80, 160, 180));
    window.draw(header);

    // Titre principal avec effet
    sf::Text title("PRESENTATION DU PROJET COSMOUIT", font, 38);
    title.setPosition(600.f, 90.f);
    title.setOrigin(title.getLocalBounds().width / 2, title.getLocalBounds().height / 2);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    title.setOutlineThickness(3);
    title.setOutlineColor(sf::Color(0, 120, 255));
    window.draw(title);

    // Sous-titre
    sf::Text subtitle("Simulateur Solaire 2D - Universite Ibn Tofail - 2025/2026", font, 18);
    subtitle.setPosition(600.f, 135.f);
    subtitle.setOrigin(subtitle.getLocalBounds().width / 2, subtitle.getLocalBounds().height / 2);
    subtitle.setFillColor(sf::Color(180, 220, 255));
    window.draw(subtitle);

    // CONTENU DÉTAILLÉ ET BIEN ORGANISÉ
    float leftColumnX = 80.f;
    float rightColumnX = 580.f;
    float currentY = 190.f;

    // ===== COLONNE GAUCHE =====

    // Section Introduction
    sf::Text introTitle("INTRODUCTION GENERALE", font, 22);
    introTitle.setPosition(leftColumnX, currentY);
    introTitle.setFillColor(sf::Color(255, 215, 0));
    introTitle.setStyle(sf::Text::Bold);
    window.draw(introTitle);

    currentY += 35.f;

    std::vector<std::string> introLines = {
        "CosmoUIT represente une avancee significative dans la",
        "visualisation educative du systeme solaire. Ce simulateur",
        "2D offre une experience immersive pour explorer les",
        "mecanismes celestes qui regissent notre systeme solaire.",
        "",
        "Developpe dans le cadre d'un projet universitaire d'excellence,",
        "cette application combine rigueur scientifique et interface",
        "utilisateur intuitive pour l'apprentissage de l'astronomie."
    };

    for (const auto& line : introLines) {
        sf::Text text(line, font, 15);
        text.setPosition(leftColumnX, currentY);
        text.setFillColor(sf::Color(220, 240, 255));
        window.draw(text);
        currentY += 22.f;
    }

    // Section Caractéristiques Techniques
    currentY += 25.f;
    sf::Text featuresTitle("CARACTERISTIQUES TECHNIQUES", font, 22);
    featuresTitle.setPosition(leftColumnX, currentY);
    featuresTitle.setFillColor(sf::Color(0, 255, 200));
    featuresTitle.setStyle(sf::Text::Bold);
    window.draw(featuresTitle);

    currentY += 35.f;

    std::vector<std::string> technicalFeatures = {
        "> Simulation physique basee sur les lois de Kepler",
        "> Integration numerique pour le calcul des orbites",
        "> Gestion realiste de la gravitation newtonienne",
        "> Systeme de coordonnees heliocentriques",
        "> Calcul precis des vitesses orbitales",
        "> Modelisation des excentricites planetaires",
        "> Gestion des collisions et interactions",
        "> Systeme de particules pour les cometes"
    };

    for (const auto& feature : technicalFeatures) {
        sf::Text text(feature, font, 14);
        text.setPosition(leftColumnX + 10.f, currentY);
        text.setFillColor(sf::Color(180, 255, 180));
        window.draw(text);
        currentY += 20.f;
    }

    // ===== COLONNE DROITE =====
    currentY = 190.f;

    // Section Fonctionnalités Utilisateur
    sf::Text userFeaturesTitle("FONCTIONNALITES UTILISATEUR", font, 22);
    userFeaturesTitle.setPosition(rightColumnX, currentY);
    userFeaturesTitle.setFillColor(sf::Color(255, 100, 255));
    userFeaturesTitle.setStyle(sf::Text::Bold);
    window.draw(userFeaturesTitle);

    currentY += 35.f;

    std::vector<std::string> userFeatures = {
        "> Navigation fluide dans le systeme solaire",
        "> Zoom avance (x0.1 a x50)",
        "> Controle de la vitesse de simulation",
        "> Affichage selectif des elements",
        "> Systeme de coordonnees en temps reel",
        "> Informations detaillees des planetes",
        "> Visualisation des trajectoires",
        "> Mode plein ecran disponible"
    };

    for (const auto& feature : userFeatures) {
        sf::Text text(feature, font, 14);
        text.setPosition(rightColumnX + 10.f, currentY);
        text.setFillColor(sf::Color(255, 200, 200));
        window.draw(text);
        currentY += 20.f;
    }

    // Section Technologies
    currentY += 25.f;
    sf::Text techTitle("TECHNOLOGIES ET OUTILS", font, 22);
    techTitle.setPosition(rightColumnX, currentY);
    techTitle.setFillColor(sf::Color(255, 150, 50));
    techTitle.setStyle(sf::Text::Bold);
    window.draw(techTitle);

    currentY += 35.f;

    std::vector<std::pair<std::string, std::string>> technologies = {
        {"C++ 17", "Standard moderne pour performances"},
        {"SFML 2.5", "Multimedia & gestion fenetres"},
        {"OpenGL", "Acceleration graphique materielle"},
        {"CMake", "Gestion de build multi-plateforme"},
        {"Git", "Controle de version collaboratif"},
        {"Visual Studio", "Environnement de developpement"}
    };

    for (const auto& tech : technologies) {
        sf::Text techName(tech.first, font, 15);
        techName.setPosition(rightColumnX, currentY);
        techName.setFillColor(sf::Color(255, 220, 100));
        techName.setStyle(sf::Text::Bold);
        window.draw(techName);

        sf::Text techDesc(tech.second, font, 13);
        techDesc.setPosition(rightColumnX + 120.f, currentY);
        techDesc.setFillColor(sf::Color(200, 230, 255));
        window.draw(techDesc);

        currentY += 22.f;
    }


    // Pied de page informatif
    sf::Text footer("Developpe avec C++17 | SFML 2.5 | CMake | OpenGL | Universite Ibn Tofail 2025", font, 14);
    footer.setPosition(600.f, 690.f);
    footer.setOrigin(footer.getLocalBounds().width / 2, footer.getLocalBounds().height / 2);
    footer.setFillColor(sf::Color(150, 180, 220));
    window.draw(footer);

    // Bouton retour avec effet
    for (auto& b : backButtons) {
        if (b.hover) {
            sf::RectangleShape glow(sf::Vector2f(b.base.getSize().x + 12, b.base.getSize().y + 12));
            glow.setPosition(b.base.getPosition().x - 6, b.base.getPosition().y - 6);
            glow.setFillColor(sf::Color(255, 50, 50, 40));
            window.draw(glow);
        }
        b.draw(window);
    }
}

void drawTeamInfo(sf::RenderWindow& window, const sf::Font& font,
    std::vector<CosmicButton>& backButtons,
    std::vector<Star>& stars, float time) {

    // Fond spatial élégant
    window.clear(sf::Color(12, 8, 35));

    // Fond étoilé
    for (auto& s : stars) {
        s.draw(window);
    }

    // Panneau principal avec style professionnel
    sf::RectangleShape mainPanel(sf::Vector2f(1100.f, 650.f));
    mainPanel.setPosition(50.f, 70.f);
    mainPanel.setFillColor(sf::Color(25, 15, 50, 230));
    mainPanel.setOutlineThickness(4);
    mainPanel.setOutlineColor(sf::Color(180, 0, 255));
    window.draw(mainPanel);

    // En-tête dégradé
    sf::RectangleShape header(sf::Vector2f(1100.f, 90.f));
    header.setPosition(50.f, 70.f);
    header.setFillColor(sf::Color(100, 0, 180, 180));
    window.draw(header);

    // Titre principal
    sf::Text title("EQUIPE DE DEVELOPPEMENT COSMOUIT", font, 36);
    title.setPosition(600.f, 90.f);
    title.setOrigin(title.getLocalBounds().width / 2, title.getLocalBounds().height / 2);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    title.setOutlineThickness(3);
    title.setOutlineColor(sf::Color(200, 0, 255));
    window.draw(title);

    // Sous-titre
    sf::Text subtitle("Projet Universitaire - Annee 2025/2026", font, 18);
    subtitle.setPosition(600.f, 135.f);
    subtitle.setOrigin(subtitle.getLocalBounds().width / 2, subtitle.getLocalBounds().height / 2);
    subtitle.setFillColor(sf::Color(220, 180, 255));
    window.draw(subtitle);

    // CONTENU DÉTAILLÉ DE L'ÉQUIPE
    float leftColumnX = 80.f;
    float rightColumnX = 580.f;
    float currentY = 190.f;

    // ===== COLONNE GAUCHE - ÉQUIPE =====

    // Titre section équipe
    sf::Text teamTitle("EQUIPE DE DEVELOPPEMENT", font, 24);
    teamTitle.setPosition(leftColumnX, currentY);
    teamTitle.setFillColor(sf::Color(255, 200, 0));
    teamTitle.setStyle(sf::Text::Bold);
    window.draw(teamTitle);

    currentY += 45.f;

    // Membres de l'équipe avec rôles détaillés
    std::vector<std::tuple<std::string, std::string, std::string>> teamMembers = {
        {"EL GHAYATI SARA", "Chef de Projet", "Coordination, Architecture, Developpement principal"},
        {"SANGARE NFALY", "Expert Physique", "Algorithmes, Simulation, Calculs scientifiques"},
        {"SARHANE AYMANE", "Designer UI/UX", "Interface, Experience utilisateur, Graphismes"},
        {"BAJOUDI ALAE", "Developpeur", "Optimisation, Fonctionnalites, Tests"},
        {"MIDINI HAFSA", "Responsable Qualite", "Documentation, Tests, Validation"}
    };

    for (const auto& member : teamMembers) {
        // Nom
        sf::Text name(std::get<0>(member), font, 17);
        name.setPosition(leftColumnX, currentY);
        name.setFillColor(sf::Color(100, 255, 255));
        name.setStyle(sf::Text::Bold);
        window.draw(name);

        // Rôle principal
        sf::Text role(std::get<1>(member), font, 14);
        role.setPosition(leftColumnX, currentY + 22.f);
        role.setFillColor(sf::Color(255, 255, 150));
        window.draw(role);

        // Responsabilités
        sf::Text responsibilities(std::get<2>(member), font, 12);
        responsibilities.setPosition(leftColumnX, currentY + 40.f);
        responsibilities.setFillColor(sf::Color(180, 255, 180));
        window.draw(responsibilities);

        currentY += 70.f;
    }

    // ===== COLONNE DROITE - ENCADREMENT & INSTITUTION =====
    currentY = 190.f;

    // Section Encadrement
    sf::Text supervisionTitle("ENCADREMENT ACADEMIQUE", font, 24);
    supervisionTitle.setPosition(rightColumnX, currentY);
    supervisionTitle.setFillColor(sf::Color(0, 255, 255));
    supervisionTitle.setStyle(sf::Text::Bold);
    window.draw(supervisionTitle);

    currentY += 45.f;

    // Encadrant principal
    sf::Text supervisorName("Pr. MOHAMED DAOUDI", font, 20);
    supervisorName.setPosition(rightColumnX, currentY);
    supervisorName.setFillColor(sf::Color(255, 255, 100));
    supervisorName.setStyle(sf::Text::Bold);
    window.draw(supervisorName);

    currentY += 30.f;

    sf::Text supervisorRole("Professeur Encadrant Principal", font, 16);
    supervisorRole.setPosition(rightColumnX, currentY);
    supervisorRole.setFillColor(sf::Color(200, 230, 255));
    window.draw(supervisorRole);

    currentY += 25.f;

    sf::Text supervisorDept("Departement d'Informatique", font, 15);
    supervisorDept.setPosition(rightColumnX, currentY);
    supervisorDept.setFillColor(sf::Color(180, 200, 255));
    window.draw(supervisorDept);


    // Section Informations Projet
    currentY += 35.f;
    sf::Text projectTitle("INFORMATIONS DU PROJET", font, 22);
    projectTitle.setPosition(rightColumnX, currentY);
    projectTitle.setFillColor(sf::Color(255, 150, 50));
    projectTitle.setStyle(sf::Text::Bold);
    window.draw(projectTitle);

    currentY += 40.f;

    std::vector<std::pair<std::string, std::string>> projectInfo = {
        {"Nom", "CosmoUIT - Simulateur Solaire 2D"},
        {"Version", "2.0 (Release Finale)"},
        {"Duree", "2 Mois"},
        {"Langage", "C++ 17 avec SFML"},
        {"Plateformes", "Windows"},
        {"Licence", "Educative - Universitaire"}
    };

    for (const auto& info : projectInfo) {
        sf::Text label(info.first + ":", font, 14);
        label.setPosition(rightColumnX, currentY);
        label.setFillColor(sf::Color(200, 220, 255));
        label.setStyle(sf::Text::Bold);
        window.draw(label);

        sf::Text value(info.second, font, 14);
        value.setPosition(rightColumnX + 120.f, currentY);
        value.setFillColor(sf::Color(255, 255, 180));
        window.draw(value);

        currentY += 24.f;
    }

    // Citation inspirante en bas de page
    sf::Text quote("L'astronomie nous apprend a regarder au-dela de nous-memes", font, 16);
    quote.setPosition(600.f, 650.f);
    quote.setOrigin(quote.getLocalBounds().width / 2, quote.getLocalBounds().height / 2);
    quote.setFillColor(sf::Color(150, 255, 150));
    quote.setStyle(sf::Text::Italic);
    window.draw(quote);

    sf::Text quoteAuthor("- Carl Sagan", font, 14);
    quoteAuthor.setPosition(600.f, 670.f);
    quoteAuthor.setOrigin(quoteAuthor.getLocalBounds().width / 2, quoteAuthor.getLocalBounds().height / 2);
    quoteAuthor.setFillColor(sf::Color(150, 255, 150));
    window.draw(quoteAuthor);

    // Copyright
    sf::Text copyright("(c) 2025 Universite Ibn Tofail - Tous droits reserves - Version 2.0", font, 12);
    copyright.setPosition(600.f, 695.f);
    copyright.setOrigin(copyright.getLocalBounds().width / 2, copyright.getLocalBounds().height / 2);
    copyright.setFillColor(sf::Color(150, 160, 200));
    window.draw(copyright);

    // Bouton retour avec effet
    for (auto& b : backButtons) {
        if (b.hover) {
            sf::RectangleShape glow(sf::Vector2f(b.base.getSize().x + 12, b.base.getSize().y + 12));
            glow.setPosition(b.base.getPosition().x - 6, b.base.getPosition().y - 6);
            glow.setFillColor(sf::Color(255, 50, 50, 40));
            window.draw(glow);
        }
        b.draw(window);
    }
}

void drawPlanetDetails(sf::RenderWindow& window, const sf::Font& font,
    std::vector<CosmicButton>& backButtons,
    std::vector<Star>& stars, float time,
    const Planet& planet) {

    window.clear(sf::Color(8, 12, 35));

    // Fond étoilé animé
    for (auto& s : stars) {
        s.draw(window);
    }

    // Nébuleuse d'arrière-plan colorée selon la planète
    sf::CircleShape nebula(350.f);
    nebula.setPosition(150.f, 100.f);
    nebula.setFillColor(sf::Color(planet.color.r, planet.color.g, planet.color.b, 15));
    window.draw(nebula);

    // Panneau principal avec effet moderne
    sf::RectangleShape mainPanel(sf::Vector2f(1100.f, 650.f));
    mainPanel.setPosition(50.f, 75.f);
    mainPanel.setFillColor(sf::Color(20, 25, 45, 240));
    mainPanel.setOutlineThickness(3);
    mainPanel.setOutlineColor(sf::Color(planet.color.r, planet.color.g, planet.color.b, 180));
    window.draw(mainPanel);

    // En-tête dégradé
    sf::RectangleShape header(sf::Vector2f(1100.f, 100.f));
    header.setPosition(50.f, 75.f);
    header.setFillColor(sf::Color(planet.color.r / 3, planet.color.g / 3, planet.color.b / 3, 200));
    window.draw(header);

    // Titre principal avec effet
    sf::Text titleShadow(planet.name, font, 46);
    titleShadow.setPosition(604.f, 104.f);
    titleShadow.setOrigin(titleShadow.getLocalBounds().width / 2, 0);
    titleShadow.setFillColor(sf::Color(0, 0, 0, 120));
    titleShadow.setStyle(sf::Text::Bold);
    window.draw(titleShadow);

    sf::Text title(planet.name, font, 46);
    title.setPosition(600.f, 100.f);
    title.setOrigin(title.getLocalBounds().width / 2, 0);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    title.setOutlineThickness(1);
    title.setOutlineColor(planet.color);
    window.draw(title);

    // Sous-titre avec type de planète
    sf::Text subtitle(planet.planetType, font, 22);
    subtitle.setPosition(600.f, 155.f);
    subtitle.setOrigin(subtitle.getLocalBounds().width / 2, 0);
    subtitle.setFillColor(planet.color);
    subtitle.setStyle(sf::Text::Italic);
    window.draw(subtitle);

    // ===== COLONNE GAUCHE - VISUEL PLANÈTE AVEC TEXTURE =====

    // Cercle d'arrière-plan orbital
    sf::CircleShape orbitCircle(100.f);
    orbitCircle.setOrigin(100.f, 100.f);
    orbitCircle.setPosition(250.f, 350.f);
    orbitCircle.setFillColor(sf::Color::Transparent);
    orbitCircle.setOutlineThickness(1);
    orbitCircle.setOutlineColor(sf::Color(planet.color.r, planet.color.g, planet.color.b, 80));
    window.draw(orbitCircle);

    // Représentation visuelle avec texture
    if (planet.hasTexture()) {
        // Utiliser un sprite avec la texture de la planète
        sf::Sprite planetSprite;
        planetSprite.setTexture(*planet.getTexture());

        // Ajuster la taille pour qu'elle rentre dans le cercle
        sf::FloatRect bounds = planetSprite.getLocalBounds();
        float scale = 140.f / std::max(bounds.width, bounds.height);
        planetSprite.setScale(scale, scale);
        planetSprite.setOrigin(bounds.width / 2, bounds.height / 2);
        planetSprite.setPosition(250.f, 350.f);

        // Effet de rotation lent
        planetSprite.setRotation(time * 20.f);

        window.draw(planetSprite);

        // Cercle de bordure pour un effet plus propre
        sf::CircleShape planetBorder(70.f);
        planetBorder.setOrigin(70.f, 70.f);
        planetBorder.setPosition(250.f, 350.f);
        planetBorder.setFillColor(sf::Color::Transparent);
        planetBorder.setOutlineThickness(2);
        planetBorder.setOutlineColor(sf::Color(255, 255, 255, 100));
        window.draw(planetBorder);
    }
    else {
        // Fallback vers le cercle coloré si pas de texture
        sf::CircleShape planetVisual(70.f);
        planetVisual.setOrigin(70.f, 70.f);
        planetVisual.setPosition(250.f, 350.f);
        planetVisual.setFillColor(planet.color);
        planetVisual.setOutlineThickness(3);
        planetVisual.setOutlineColor(sf::Color(255, 255, 255, 150));
        window.draw(planetVisual);
    }

    // Effet de lumière
    sf::CircleShape planetHighlight(50.f);
    planetHighlight.setOrigin(50.f, 50.f);
    planetHighlight.setPosition(220.f, 320.f);
    planetHighlight.setFillColor(sf::Color(255, 255, 255, 60));
    window.draw(planetHighlight);

    // Panneau fait intéressant
    sf::RectangleShape factPanel(sf::Vector2f(320.f, 100.f));
    factPanel.setPosition(90.f, 470.f);
    factPanel.setFillColor(sf::Color(30, 35, 60, 220));
    factPanel.setOutlineThickness(2);
    factPanel.setOutlineColor(sf::Color(planet.color.r, planet.color.g, planet.color.b, 120));
    window.draw(factPanel);

    sf::Text factTitle("FAIT MARQUANT", font, 16);
    factTitle.setPosition(250.f, 485.f);
    factTitle.setOrigin(factTitle.getLocalBounds().width / 2, 0);
    factTitle.setFillColor(sf::Color(255, 255, 150));
    factTitle.setStyle(sf::Text::Bold);
    window.draw(factTitle);

    // Texte du fait avec ajustement automatique
    std::string factText = planet.fact;
    if (factText.length() > 40) {
        // Trouver un espace pour couper le texte
        size_t spacePos = factText.find(' ', 35);
        if (spacePos != std::string::npos) {
            factText.insert(spacePos + 1, "\n");
        }
    }

    sf::Text factContent(factText, font, 14);
    factContent.setPosition(250.f, 510.f);
    factContent.setOrigin(factContent.getLocalBounds().width / 2, 0);
    factContent.setFillColor(sf::Color(200, 230, 255));
    factContent.setStyle(sf::Text::Italic);
    window.draw(factContent);

    // ===== COLONNE DROITE - INFORMATIONS DÉTAILLÉES =====
    float rightColumnX = 450.f;
    float currentY = 230.f;

    // Section Caractéristiques Principales
    sf::Text mainTitle("CARACTÉRISTIQUES", font, 20);
    mainTitle.setPosition(rightColumnX, currentY);
    mainTitle.setFillColor(sf::Color(100, 255, 255));
    mainTitle.setStyle(sf::Text::Bold);
    window.draw(mainTitle);

    currentY += 35.f;

    // Récupération des informations formatées
    auto planetInfo = planet.getFormattedInfo();

    // Affichage des caractéristiques dans un design organisé
    for (size_t i = 0; i < planetInfo.size(); ++i) {
        const auto& info = planetInfo[i];

        // Ligne séparatrice pour les paires
        if (i % 2 == 0 && i < planetInfo.size() - 1) {
            sf::RectangleShape line(sf::Vector2f(580.f, 1.f));
            line.setPosition(rightColumnX + 10.f, currentY + 42.f);
            line.setFillColor(sf::Color(60, 70, 100, 100));
            window.draw(line);
        }

        // Conteneur pour chaque paire d'informations
        float pairWidth = 280.f;
        float xOffset = (i % 2 == 0) ? 0.f : pairWidth + 20.f;

        sf::RectangleShape infoBox(sf::Vector2f(pairWidth, 35.f));
        infoBox.setPosition(rightColumnX + xOffset, currentY);
        infoBox.setFillColor(sf::Color(35, 45, 70, 180));
        infoBox.setOutlineThickness(1);
        infoBox.setOutlineColor(sf::Color(70, 80, 110, 100));
        window.draw(infoBox);

        // Nom de la caractéristique
        sf::Text label(info.first + ":", font, 14);
        label.setPosition(rightColumnX + xOffset + 10.f, currentY + 8.f);
        label.setFillColor(sf::Color(180, 200, 255));
        label.setStyle(sf::Text::Bold);
        window.draw(label);

        // Valeur
        sf::Text value(info.second, font, 14);
        value.setPosition(rightColumnX + xOffset + 120.f, currentY + 8.f);
        value.setFillColor(sf::Color(255, 255, 180));
        window.draw(value);

        // Passer à la ligne suivante après chaque paire
        if (i % 2 == 1 || i == planetInfo.size() - 1) {
            currentY += 45.f;
        }
    }

    // Section Composition Atmosphérique
    currentY += 10.f;
    sf::Text compTitle("COMPOSITION", font, 20);
    compTitle.setPosition(rightColumnX, currentY);
    compTitle.setFillColor(sf::Color(255, 200, 100));
    compTitle.setStyle(sf::Text::Bold);
    window.draw(compTitle);

    currentY += 35.f;

    // Panneau de composition
    sf::RectangleShape compPanel(sf::Vector2f(580.f, 70.f));
    compPanel.setPosition(rightColumnX, currentY);
    compPanel.setFillColor(sf::Color(40, 35, 60, 200));
    compPanel.setOutlineThickness(2);
    compPanel.setOutlineColor(sf::Color(200, 150, 100, 150));
    window.draw(compPanel);

    sf::Text compText(planet.composition, font, 14);
    compText.setPosition(rightColumnX + 20.f, currentY + 25.f);
    compText.setFillColor(sf::Color(220, 210, 180));
    compText.setStyle(sf::Text::Italic);
    window.draw(compText);

    // Barre de séparation décorative en bas
    sf::RectangleShape bottomBar(sf::Vector2f(1000.f, 3.f));
    bottomBar.setPosition(100.f, 640.f);
    bottomBar.setFillColor(sf::Color(planet.color.r, planet.color.g, planet.color.b, 120));
    window.draw(bottomBar);

    // Note éducative
    sf::Text educationalNote("Données scientifiques basées sur les observations astronomiques modernes", font, 11);
    educationalNote.setPosition(600.f, 655.f);
    educationalNote.setOrigin(educationalNote.getLocalBounds().width / 2, 0);
    educationalNote.setFillColor(sf::Color(150, 170, 200));
    educationalNote.setStyle(sf::Text::Italic);
    window.draw(educationalNote);

    // Bouton retour avec effet
    for (auto& b : backButtons) {
        if (b.hover) {
            sf::RectangleShape hoverEffect(b.base.getSize());
            hoverEffect.setPosition(b.base.getPosition());
            hoverEffect.setFillColor(sf::Color(255, 255, 255, 30));
            window.draw(hoverEffect);
        }
        b.draw(window);
    }
}

void drawSimulation(sf::RenderWindow& window, const sf::Font& font,
    sf::View& worldView, sf::View& uiView,
    std::vector<Star>& stars, float time,
    const sf::CircleShape& sun, const sf::CircleShape& sunGlow,
    std::vector<Planet>& planets,
    std::vector<class Comet>& comets,
    std::vector<CosmicButton>& simButtons,
    const sf::Text& infoText, const sf::Text& speedText,
    bool showOrbits, bool showTrails, bool showLabels,
    bool showStats, bool showGrid,
    const sf::RectangleShape& statsPanel,
    const sf::Text& statsTitle,
    const sf::Texture& saturnRingTexture) {

    window.setView(worldView);

    // Étoiles
    for (auto& s : stars) {
        s.draw(window);
    }

    // Grille
    if (showGrid) {
        for (int i = -10; i <= 10; ++i) {
            sf::VertexArray vLine(sf::Lines, 2);
            vLine[0].position = sf::Vector2f(i * 100.f, -1000.f);
            vLine[1].position = sf::Vector2f(i * 100.f, 1000.f);
            vLine[0].color = vLine[1].color = sf::Color(50, 50, 50, 100);
            window.draw(vLine);

            sf::VertexArray hLine(sf::Lines, 2);
            hLine[0].position = sf::Vector2f(-1000.f, i * 100.f);
            hLine[1].position = sf::Vector2f(1000.f, i * 100.f);
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

    // Planètes
    for (auto& p : planets) {
        p.draw(window, showTrails, showLabels);
        if (p.getName() == "Saturne") {
            p.drawRings(window, saturnRingTexture);
        }
    }

    // Comètes
    for (auto& c : comets) {
        c.draw(window, showTrails);
    }

    // UI
    window.setView(uiView);
    for (auto& b : simButtons) {
        b.draw(window);
    }
    window.draw(infoText);
    window.draw(speedText);

    // Statistiques
    if (showStats) {
        window.draw(statsPanel);
        window.draw(statsTitle);

        float yPos = 60.f;
        for (size_t i = 0; i < planets.size(); ++i) {
            const auto& p = planets[i];

            sf::CircleShape colorDot(5.f);
            colorDot.setPosition(895.f, yPos + 5.f);
            colorDot.setFillColor(p.color);
            window.draw(colorDot);

            std::ostringstream oss;
            oss << p.getName() << " (" << p.numMoons << " lunes)";
            sf::Text planetStat(oss.str(), font, 13);
            planetStat.setPosition(910.f, yPos);
            planetStat.setFillColor(sf::Color::White);
            window.draw(planetStat);

            yPos += 58.f;
        }
    }
}