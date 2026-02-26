// ===== EN-TÊTE POUR LES GUIDES D'INTERACTION DES PLANÈTES =====

#ifndef UI_PLANET_FOLLOW_GUIDE_H
#define UI_PLANET_FOLLOW_GUIDE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Planet.h"

/**
 * @brief Affiche un panneau intuitif pour sélectionner quelle planète suivre
 * @param window Fenêtre de rendu
 * @param font Police d'affichage
 * @param planets Vector des planètes
 * @param followedPlanetIndex Indice de la planète actuellement suivie (-1 si aucune)
 * @param time Temps écoulé pour les animations
 * 
 * Panneau principal montrant:
 * - Les 8 planètes dans une grille 2x4
 * - Le numéro de touche (1-8) à appuyer
 * - La couleur de la planète
 * - Un indicateur animé si la planète est suivie
 * - Une légende pour "0 = Centre"
 */
void drawPlanetFollowGuide(sf::RenderWindow& window, const sf::Font& font,
    const std::vector<Planet>& planets, int followedPlanetIndex, float time);

/**
 * @brief Affiche les raccourcis clavier disponibles
 * @param window Fenêtre de rendu
 * @param font Police d'affichage
 * @param time Temps écoulé pour les animations
 * 
 * Petit panneau dans le coin supérieur droit montrant:
 * - 1-8: Suivre une planète
 * - 0: Vue sur le soleil
 * - F: Mode suivi
 * - TAB: Menu d'outils
 * - R: Reset de la vue
 */
void drawKeyboardHints(sf::RenderWindow& window, const sf::Font& font, float time);

/**
 * @brief Affiche un badge contextuel quand la souris survole une planète
 * @param window Fenêtre de rendu
 * @param font Police d'affichage
 * @param planets Vector des planètes
 * @param worldView Vue du monde
 * @param uiView Vue de l'interface
 * 
 * Badge qui montre:
 * - Le numéro de la planète en gros chiffres
 * - La couleur de la planète
 * - Le texte "Touche X"
 */
void drawNumberKeyIndicator(sf::RenderWindow& window, const sf::Font& font,
    const std::vector<Planet>& planets, const sf::View& worldView, const sf::View& uiView);

#endif // UI_PLANET_FOLLOW_GUIDE_H
