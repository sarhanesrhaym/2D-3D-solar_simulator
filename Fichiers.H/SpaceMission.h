#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

/**
 * @brief Représente une mission spatiale historique
 */
struct SpaceMission {
    std::string name;   // Nom de la mission
    std::string targetPlanet;       // Planète cible
    int year;       // Année de la mission
    std::string agency;      // Agence spatiale (NASA, ESA, etc.)
std::string description;      // Description courte
    sf::Color markerColor;          // Couleur du marqueur
    std::string missionType; // Type: "Flyby", "Orbiter", "Lander", "Rover"
    bool success;         // Mission réussie ou non
    
    // Position sur l'orbite (angle en radians)
    float orbitAngle;
    
    // Animation
    float pulsePhase;
    
    SpaceMission(const std::string& name, const std::string& target, int year,
            const std::string& agency, const std::string& desc,
        const std::string& type, bool success, float angle = 0.f)
        : name(name), targetPlanet(target), year(year), agency(agency),
          description(desc), missionType(type), success(success),
       orbitAngle(angle), pulsePhase(0.f) {
        
        // Couleur selon l'agence
     if (agency == "NASA") {
        markerColor = sf::Color(0, 123, 255);  // Bleu NASA
        } else if (agency == "ESA") {
        markerColor = sf::Color(0, 51, 153);   // Bleu foncé ESA
        } else if (agency == "ROSCOSMOS") {
  markerColor = sf::Color(255, 100, 100); // Rouge Russie
        } else if (agency == "JAXA") {
  markerColor = sf::Color(255, 50, 50);  // Rouge Japon
        } else if (agency == "CNSA") {
 markerColor = sf::Color(255, 215, 0);  // Or Chine
        } else {
 markerColor = sf::Color(150, 150, 150); // Gris par défaut
      }
        
        // Ajouter transparence pour les missions anciennes
 if (!success) {
         markerColor.a = 150;
        }
    }
};

/**
 * @brief Classe pour gérer l'affichage et l'interaction avec les missions spatiales
 */
class MissionTimeline {
public:
    MissionTimeline();
    
 // Initialiser les missions historiques
    void initializeMissions();
    
    // Mettre à jour les animations
    void update(float deltaTime);
    
    // Dessiner les marqueurs de missions sur les orbites
    void drawMissionMarkers(sf::RenderWindow& window, 
    const std::vector<class Planet>& planets,
      const sf::Vector2f& sunCenter);
    
    // Dessiner le panneau d'information d'une mission
    void drawMissionInfo(sf::RenderWindow& window, const sf::Font& font,
           const SpaceMission& mission, sf::Vector2f position);
    
    // Vérifier si la souris survole un marqueur de mission
    const SpaceMission* checkMissionHover(const sf::Vector2f& mousePos,
            const std::vector<class Planet>& planets,
                const sf::Vector2f& sunCenter);
    
    // Filtrer les missions par planète
    std::vector<SpaceMission> getMissionsForPlanet(const std::string& planetName) const;
    
    // Activer/désactiver l'affichage
    void setVisible(bool visible) { isVisible = visible; }
    bool isTimelineVisible() const { return isVisible; }
    
private:
    std::vector<SpaceMission> missions;
    bool isVisible;
    float globalAnimationTime;
    
    // Calculer la position d'un marqueur sur l'orbite
    sf::Vector2f calculateMarkerPosition(const class Planet& planet,
        float angle,
       const sf::Vector2f& sunCenter);
};

// Fonction helper pour obtenir toutes les missions historiques
std::vector<SpaceMission> getAllSpaceMissions();
