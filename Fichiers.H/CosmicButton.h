#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

class CosmicButton {
public:
    //  Constructeur simple (3 couleurs + clickCol)
    CosmicButton(const sf::Vector2f& position, const sf::Vector2f& size,
        const std::string& labelStr, sf::Color normalCol, sf::Color hoverCol,
        sf::Color clickCol, const sf::Font& font, std::function<void()> callback);

    //  Constructeur avec icône (3 couleurs + clickCol)
    CosmicButton(const sf::Vector2f& position, const sf::Vector2f& size,
        const std::string& iconStr, const std::string& labelStr,
        sf::Color normalCol, sf::Color hoverCol, sf::Color clickCol,
        const sf::Font& font, std::function<void()> callback);

    void update(const sf::Vector2f& mousePos);
    void draw(sf::RenderWindow& window);
    bool isClicked(const sf::Vector2f& mousePos);
    void executeAction();
    void resetState();

    // Membres publics
    sf::RectangleShape base;
    sf::Text label;
    sf::Text icon;
    std::function<void()> action;
    bool hover;
    bool clicked;  //  État du clic
    sf::Color normalColor;
    sf::Color hoverColor;
    sf::Color clickColor;  //  Nouvelle couleur
    bool hasIcon;
    float clickAnimationTimer;  //  Timer pour l'animation
};