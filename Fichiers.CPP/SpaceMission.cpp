#include "SpaceMission.h"
#include "Planet.h"
#include <cmath>
#include <algorithm>

MissionTimeline::MissionTimeline()
    : isVisible(true), globalAnimationTime(0.f) {
    initializeMissions();
}

void MissionTimeline::initializeMissions() {
    missions.clear();

    // ===== MISSIONS VERS MERCURE =====
    missions.push_back(SpaceMission(
        "Mariner 10", "Mercure", 1974,
        "NASA", "Premier survol de Mercure",
        "Flyby", true, 0.5f
    ));

    missions.push_back(SpaceMission(
        "MESSENGER", "Mercure", 2011,
        "NASA", "Premier orbiteur de Mercure",
        "Orbiter", true, 1.8f
    ));

    missions.push_back(SpaceMission(
        "BepiColombo", "Mercure", 2025,
        "ESA/JAXA", "Mission en cours vers Mercure",
        "Orbiter", true, 3.5f
    ));

    // ===== MISSIONS VERS VÉNUS =====
    missions.push_back(SpaceMission(
        "Venera 7", "Venus", 1970,
        "ROSCOSMOS", "Premier atterrissage sur Venus",
        "Lander", true, 0.8f
    ));

    missions.push_back(SpaceMission(
        "Magellan", "Venus", 1990,
        "NASA", "Cartographie radar de Venus",
        "Orbiter", true, 2.2f
    ));

    missions.push_back(SpaceMission(
        "Akatsuki", "Venus", 2015,
        "JAXA", "Etude atmosphere venusienne",
        "Orbiter", true, 4.5f
    ));

    // ===== MISSIONS VERS LA TERRE =====
    missions.push_back(SpaceMission(
        "Hubble", "Terre", 1990,
        "NASA/ESA", "Telescope spatial iconique",
        "Orbiter", true, 1.2f
    ));

    missions.push_back(SpaceMission(
        "ISS", "Terre", 1998,
        "International", "Station Spatiale Internationale",
        "Orbiter", true, 3.8f
    ));

    // ===== MISSIONS VERS MARS =====
    missions.push_back(SpaceMission(
        "Viking 1", "Mars", 1976,
        "NASA", "Premier atterrisseur martien",
        "Lander", true, 0.6f
    ));

    missions.push_back(SpaceMission(
        "Pathfinder", "Mars", 1997,
        "NASA", "Premier rover Sojourner",
        "Rover", true, 1.5f
    ));

    missions.push_back(SpaceMission(
        "Spirit & Opportunity", "Mars", 2004,
        "NASA", "Rovers jumeaux exploration Mars",
        "Rover", true, 2.4f
    ));

    missions.push_back(SpaceMission(
        "Curiosity", "Mars", 2012,
        "NASA", "Rover nucleaire cratere Gale",
        "Rover", true, 3.6f
    ));

    missions.push_back(SpaceMission(
        "Perseverance", "Mars", 2021,
        "NASA", "Rover + helicoptere Ingenuity",
        "Rover", true, 4.8f
    ));

    missions.push_back(SpaceMission(
        "Tianwen-1", "Mars", 2021,
        "CNSA", "Premiere mission chinoise Mars",
        "Rover", true, 5.2f
    ));

    // ===== MISSIONS VERS JUPITER =====
    missions.push_back(SpaceMission(
        "Pioneer 10", "Jupiter", 1973,
        "NASA", "Premier survol de Jupiter",
        "Flyby", true, 0.7f
    ));

    missions.push_back(SpaceMission(
        "Voyager 1 & 2", "Jupiter", 1979,
        "NASA", "Survols historiques Jupiter",
        "Flyby", true, 1.9f
    ));

    missions.push_back(SpaceMission(
        "Galileo", "Jupiter", 1995,
        "NASA", "Premier orbiteur de Jupiter",
        "Orbiter", true, 3.1f
    ));

    missions.push_back(SpaceMission(
        "Juno", "Jupiter", 2016,
        "NASA", "Etude structure interne Jupiter",
        "Orbiter", true, 4.7f
    ));

    // ===== MISSIONS VERS SATURNE =====
    missions.push_back(SpaceMission(
        "Pioneer 11", "Saturne", 1979,
        "NASA", "Premier survol de Saturne",
        "Flyby", true, 1.0f
    ));

    missions.push_back(SpaceMission(
        "Voyager 1", "Saturne", 1980,
        "NASA", "Survol detaille anneaux Saturne",
        "Flyby", true, 2.3f
    ));

    missions.push_back(SpaceMission(
        "Cassini-Huygens", "Saturne", 2004,
        "NASA/ESA", "Mission orbitale + atterrisseur Titan",
        "Orbiter", true, 3.9f
    ));

    // ===== MISSIONS VERS URANUS =====
    missions.push_back(SpaceMission(
        "Voyager 2", "Uranus", 1986,
        "NASA", "Seul survol d'Uranus a ce jour",
        "Flyby", true, 2.1f
    ));

    // ===== MISSIONS VERS NEPTUNE =====
    missions.push_back(SpaceMission(
        "Voyager 2", "Neptune", 1989,
        "NASA", "Seul survol de Neptune",
        "Flyby", true, 2.7f
    ));
}

void MissionTimeline::update(float deltaTime) {
    if (!isVisible) return;

    globalAnimationTime += deltaTime;

    for (auto& mission : missions) {
        mission.pulsePhase += deltaTime * 2.f;
        if (mission.pulsePhase > 6.28318f) {
            mission.pulsePhase -= 6.28318f;
        }
    }
}

//  Calculer la position du marqueur AUTOUR de la planète
sf::Vector2f MissionTimeline::calculateMarkerPosition(const Planet& planet,
    float angle,
    const sf::Vector2f& sunCenter) {
    //  Utiliser la position ACTUELLE de la planète
    sf::Vector2f planetPos = planet.shape.getPosition();
    float planetRadius = planet.shape.getRadius();

    // Distance du marqueur par rapport au centre de la planète
    float markerDistance = planetRadius + 20.f;

    // Position autour de la planète
    return sf::Vector2f(
        planetPos.x + markerDistance * std::cos(angle),
        planetPos.y + markerDistance * std::sin(angle)
    );
}

void MissionTimeline::drawMissionMarkers(sf::RenderWindow& window,
    const std::vector<Planet>& planets,
    const sf::Vector2f& sunCenter) {
    if (!isVisible) return;

    for (const auto& mission : missions) {
        // Trouver la planète cible
        const Planet* targetPlanet = nullptr;
        for (const auto& p : planets) {
            if (p.getName() == mission.targetPlanet) {
                targetPlanet = &p;
                break;
            }
        }

        if (!targetPlanet) continue;

        //  Position du marqueur AUTOUR de la planète
        sf::Vector2f planetPos = targetPlanet->shape.getPosition();
        float planetRadius = targetPlanet->shape.getRadius();
        float markerDistance = planetRadius + 20.f;

        sf::Vector2f markerPos(
            planetPos.x + markerDistance * std::cos(mission.orbitAngle),
            planetPos.y + markerDistance * std::sin(mission.orbitAngle)
        );

        // Animation de pulsation
        float pulseScale = 1.f + 0.3f * std::sin(mission.pulsePhase);
        float markerSize = 10.f;

        // Cercle externe (glow) 
        sf::CircleShape glow(markerSize * 2.f * pulseScale);
        glow.setOrigin(markerSize * 2.f * pulseScale, markerSize * 2.f * pulseScale);
        glow.setPosition(markerPos);
        glow.setFillColor(sf::Color(mission.markerColor.r, mission.markerColor.g,
            mission.markerColor.b, 80));
        window.draw(glow);

        //  Marqueur principal selon le type 
        if (mission.missionType == "Flyby") {
            sf::CircleShape marker(markerSize, 3);
            marker.setOrigin(markerSize, markerSize);
            marker.setPosition(markerPos);
            marker.setFillColor(mission.markerColor);
            marker.setOutlineThickness(2.f);
            marker.setOutlineColor(sf::Color::White);
            window.draw(marker);
        }
        else if (mission.missionType == "Orbiter") {
            sf::CircleShape marker(markerSize);
            marker.setOrigin(markerSize, markerSize);
            marker.setPosition(markerPos);
            marker.setFillColor(mission.markerColor);
            marker.setOutlineThickness(2.f);
            marker.setOutlineColor(sf::Color::White);
            window.draw(marker);
        }
        else if (mission.missionType == "Lander") {
            sf::RectangleShape marker(sf::Vector2f(markerSize * 2, markerSize * 2));
            marker.setOrigin(markerSize, markerSize);
            marker.setPosition(markerPos);
            marker.setFillColor(mission.markerColor);
            marker.setOutlineThickness(2.f);
            marker.setOutlineColor(sf::Color::White);
            window.draw(marker);
        }
        else if (mission.missionType == "Rover") {
            sf::ConvexShape marker;
            marker.setPointCount(4);
            marker.setPoint(0, sf::Vector2f(0, -markerSize));
            marker.setPoint(1, sf::Vector2f(markerSize, 0));
            marker.setPoint(2, sf::Vector2f(0, markerSize));
            marker.setPoint(3, sf::Vector2f(-markerSize, 0));
            marker.setPosition(markerPos);
            marker.setFillColor(mission.markerColor);
            marker.setOutlineThickness(2.f);
            marker.setOutlineColor(sf::Color::White);
            window.draw(marker);
        }
    }
}

void MissionTimeline::drawMissionInfo(sf::RenderWindow& window, const sf::Font& font,
    const SpaceMission& mission, sf::Vector2f position) {
    float panelW = 300.f;
    float panelH = 160.f;

    //  Ombre du panneau
    sf::RectangleShape shadow(sf::Vector2f(panelW + 4.f, panelH + 4.f));
    shadow.setPosition(position.x + 3.f, position.y + 3.f);
    shadow.setFillColor(sf::Color(0, 0, 0, 100));
    window.draw(shadow);

    //  Panneau de fond - PLUS VISIBLE
    sf::RectangleShape panel(sf::Vector2f(panelW, panelH));
    panel.setPosition(position);
    panel.setFillColor(sf::Color(20, 35, 60, 250));
    panel.setOutlineThickness(3.f);
    panel.setOutlineColor(mission.markerColor);
    window.draw(panel);

    //  Barre supérieure colorée
    sf::RectangleShape topBar(sf::Vector2f(panelW, 6.f));
    topBar.setPosition(position);
    topBar.setFillColor(mission.markerColor);
    window.draw(topBar);

    float yPos = position.y + 15.f;

    //  Nom de la mission - PLUS GRAND
    sf::Text missionName(mission.name, font, 20);
    missionName.setPosition(position.x + 15.f, yPos);
    missionName.setFillColor(sf::Color(255, 255, 100));
    missionName.setStyle(sf::Text::Bold);
    window.draw(missionName);

    yPos += 30.f;

    //  Planète cible
    sf::Text targetText("Cible: " + mission.targetPlanet, font, 14);
    targetText.setPosition(position.x + 15.f, yPos);
    targetText.setFillColor(sf::Color(180, 220, 255));
    window.draw(targetText);

    yPos += 22.f;

    //  Année
    sf::Text yearText("Annee: " + std::to_string(mission.year), font, 14);
    yearText.setPosition(position.x + 15.f, yPos);
    yearText.setFillColor(sf::Color(200, 220, 255));
    window.draw(yearText);

    yPos += 22.f;

    //  Agence
    sf::Text agencyText("Agence: " + mission.agency, font, 14);
    agencyText.setPosition(position.x + 15.f, yPos);
    agencyText.setFillColor(sf::Color(180, 200, 220));
    window.draw(agencyText);

    yPos += 22.f;

    //  Type
    sf::Text typeText("Type: " + mission.missionType, font, 14);
    typeText.setPosition(position.x + 15.f, yPos);
    typeText.setFillColor(sf::Color(180, 200, 220));
    window.draw(typeText);

    yPos += 22.f;

    // Description
    sf::Text descText(mission.description, font, 12);
    descText.setPosition(position.x + 15.f, yPos);
    descText.setFillColor(sf::Color(200, 220, 240));
    descText.setStyle(sf::Text::Italic);
    window.draw(descText);
}

const SpaceMission* MissionTimeline::checkMissionHover(const sf::Vector2f& mousePos,
    const std::vector<Planet>& planets,
    const sf::Vector2f& sunCenter) {
    if (!isVisible) return nullptr;

    //  RAYON DE DÉTECTION TRÈS AUGMENTÉ pour le plein écran
    float hoverRadius = 35.f;  

    for (const auto& mission : missions) {
        // Trouver la planète cible
        const Planet* targetPlanet = nullptr;
        for (const auto& p : planets) {
            if (p.getName() == mission.targetPlanet) {
                targetPlanet = &p;
                break;
            }
        }

        if (!targetPlanet) continue;

        // Position ACTUELLE de la planète
        sf::Vector2f planetPos = targetPlanet->shape.getPosition();
        float planetRadius = targetPlanet->shape.getRadius();

        // Position du marqueur autour de la planète
        float markerDistance = planetRadius + 20.f;
        sf::Vector2f markerPos(
            planetPos.x + markerDistance * std::cos(mission.orbitAngle),
            planetPos.y + markerDistance * std::sin(mission.orbitAngle)
        );

        // Distance entre souris et marqueur
        float dx = mousePos.x - markerPos.x;
        float dy = mousePos.y - markerPos.y;
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist < hoverRadius) {
            return &mission;
        }
    }

    return nullptr;
}

std::vector<SpaceMission> MissionTimeline::getMissionsForPlanet(const std::string& planetName) const {
    std::vector<SpaceMission> result;

    for (const auto& mission : missions) {
        if (mission.targetPlanet == planetName) {
            result.push_back(mission);
        }
    }

    std::sort(result.begin(), result.end(),
        [](const SpaceMission& a, const SpaceMission& b) {
            return a.year < b.year;
        });

    return result;
}

std::vector<SpaceMission> getAllSpaceMissions() {
    MissionTimeline timeline;
    return timeline.getMissionsForPlanet("");
}