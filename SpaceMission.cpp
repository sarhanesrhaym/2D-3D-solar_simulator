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
        "Venera 7", "Vénus", 1970,
  "ROSCOSMOS", "Premier atterrissage sur Venus",
        "Lander", true, 0.8f
));
    
    missions.push_back(SpaceMission(
        "Magellan", "Vénus", 1990,
    "NASA", "Cartographie radar de Venus",
 "Orbiter", true, 2.2f
    ));
    
    missions.push_back(SpaceMission(
 "Akatsuki", "Vénus", 2015,
  "JAXA", "Etude atmosphere venusienne",
        "Orbiter", true, 4.5f
    ));
    
  // ===== MISSIONS VERS LA TERRE (Satellites remarquables) =====
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
    
    // Mettre à jour les phases de pulsation de chaque mission
  for (auto& mission : missions) {
    mission.pulsePhase += deltaTime * 2.f;
      if (mission.pulsePhase > 6.28318f) {
   mission.pulsePhase -= 6.28318f;
        }
}
}

sf::Vector2f MissionTimeline::calculateMarkerPosition(const Planet& planet,
      float angle, const sf::Vector2f& sunCenter) {
    float r = planet.semiMajorAxis * (1 - planet.eccentricity * planet.eccentricity) /
   (1 + planet.eccentricity * std::cos(angle));
    
    return sf::Vector2f(
        sunCenter.x + r * std::cos(angle),
  sunCenter.y + r * std::sin(angle)
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
        
   // Calculer la position du marqueur sur l'orbite
        sf::Vector2f markerPos = calculateMarkerPosition(*targetPlanet,
        mission.orbitAngle, sunCenter);
    
        // Animation de pulsation
        float pulseScale = 1.f + 0.2f * std::sin(mission.pulsePhase);
   
    // Dessiner l'icône de la mission
    float markerSize = 8.f;
        
    // Cercle externe (glow)
 sf::CircleShape glow(markerSize * 1.5f * pulseScale);
 glow.setOrigin(markerSize * 1.5f * pulseScale, markerSize * 1.5f * pulseScale);
        glow.setPosition(markerPos);
        glow.setFillColor(sf::Color(mission.markerColor.r, mission.markerColor.g,
     mission.markerColor.b, 60));
   window.draw(glow);
        
   // Marqueur principal
  if (mission.missionType == "Flyby") {
            // Triangle pour flyby
      sf::CircleShape marker(markerSize, 3);
       marker.setOrigin(markerSize, markerSize);
  marker.setPosition(markerPos);
marker.setFillColor(mission.markerColor);
      marker.setOutlineThickness(1.f);
      marker.setOutlineColor(sf::Color::White);
  window.draw(marker);
        } else if (mission.missionType == "Orbiter") {
     // Cercle pour orbiter
     sf::CircleShape marker(markerSize);
      marker.setOrigin(markerSize, markerSize);
  marker.setPosition(markerPos);
   marker.setFillColor(mission.markerColor);
       marker.setOutlineThickness(2.f);
       marker.setOutlineColor(sf::Color::White);
       window.draw(marker);
        } else if (mission.missionType == "Lander") {
   // Carré pour lander
            sf::RectangleShape marker(sf::Vector2f(markerSize * 2, markerSize * 2));
     marker.setOrigin(markerSize, markerSize);
    marker.setPosition(markerPos);
    marker.setFillColor(mission.markerColor);
       marker.setOutlineThickness(1.f);
      marker.setOutlineColor(sf::Color::White);
       window.draw(marker);
        } else if (mission.missionType == "Rover") {
    // Losange pour rover
       sf::ConvexShape marker;
   marker.setPointCount(4);
   marker.setPoint(0, sf::Vector2f(0, -markerSize));
  marker.setPoint(1, sf::Vector2f(markerSize, 0));
   marker.setPoint(2, sf::Vector2f(0, markerSize));
marker.setPoint(3, sf::Vector2f(-markerSize, 0));
  marker.setPosition(markerPos);
       marker.setFillColor(mission.markerColor);
       marker.setOutlineThickness(1.f);
       marker.setOutlineColor(sf::Color::White);
            window.draw(marker);
   }
    }
}

void MissionTimeline::drawMissionInfo(sf::RenderWindow& window, const sf::Font& font,
      const SpaceMission& mission, sf::Vector2f position) {
    float panelW = 280.f;
    float panelH = 140.f;
    
    // Panneau de fond
    sf::RectangleShape panel(sf::Vector2f(panelW, panelH));
    panel.setPosition(position);
    panel.setFillColor(sf::Color(15, 25, 45, 240));
    panel.setOutlineThickness(2.f);
    panel.setOutlineColor(mission.markerColor);
    window.draw(panel);
    
    // Barre supérieure avec couleur de l'agence
    sf::RectangleShape topBar(sf::Vector2f(panelW, 4.f));
    topBar.setPosition(position);
    topBar.setFillColor(mission.markerColor);
    window.draw(topBar);
    
    float yPos = position.y + 12.f;
    
    // Nom de la mission
    sf::Text missionName(mission.name, font, 16);
    missionName.setPosition(position.x + 10.f, yPos);
    missionName.setFillColor(sf::Color(255, 255, 100));
    missionName.setStyle(sf::Text::Bold);
 window.draw(missionName);
 
    yPos += 25.f;
    
    // Année
    sf::Text yearText("Annee: " + std::to_string(mission.year), font, 13);
    yearText.setPosition(position.x + 10.f, yPos);
 yearText.setFillColor(sf::Color(200, 220, 255));
    window.draw(yearText);
    
    yPos += 20.f;
    
    // Agence
 sf::Text agencyText("Agence: " + mission.agency, font, 12);
    agencyText.setPosition(position.x + 10.f, yPos);
    agencyText.setFillColor(sf::Color(180, 200, 220));
    window.draw(agencyText);
    
    yPos += 20.f;
    
    // Type
    sf::Text typeText("Type: " + mission.missionType, font, 12);
    typeText.setPosition(position.x + 10.f, yPos);
    typeText.setFillColor(sf::Color(180, 200, 220));
    window.draw(typeText);
    
    yPos += 20.f;
    
    // Description
  sf::Text descText(mission.description, font, 11);
    descText.setPosition(position.x + 10.f, yPos);
    descText.setFillColor(sf::Color(200, 220, 240));
    window.draw(descText);
}

const SpaceMission* MissionTimeline::checkMissionHover(const sf::Vector2f& mousePos,
               const std::vector<Planet>& planets,
     const sf::Vector2f& sunCenter) {
    if (!isVisible) return nullptr;
    
    float hoverRadius = 12.f;
    
    for (const auto& mission : missions) {
        const Planet* targetPlanet = nullptr;
        for (const auto& p : planets) {
 if (p.getName() == mission.targetPlanet) {
       targetPlanet = &p;
                break;
            }
        }
        
        if (!targetPlanet) continue;
      
        sf::Vector2f markerPos = calculateMarkerPosition(*targetPlanet,
    mission.orbitAngle, sunCenter);
        
    float dist = std::hypot(mousePos.x - markerPos.x, mousePos.y - markerPos.y);
  
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
    
    // Trier par année
    std::sort(result.begin(), result.end(),
        [](const SpaceMission& a, const SpaceMission& b) {
    return a.year < b.year;
        });
 
    return result;
}

std::vector<SpaceMission> getAllSpaceMissions() {
    MissionTimeline timeline;
    return timeline.getMissionsForPlanet(""); // Retourner toutes
}
