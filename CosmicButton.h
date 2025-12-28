#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

class CosmicButton {
public:
    // Constructeur pour bouton avec icône et label
    CosmicButton(const sf::Vector2f& position, const sf::Vector2f& size,
        const std::string& iconStr, const std::string& labelStr,
        sf::Color normalCol, sf::Color hoverCol,
        const sf::Font& font, std::function<void()> callback);

    // Constructeur pour bouton simple (menu principal)
    CosmicButton(const sf::Vector2f& position, const sf::Vector2f& size,
        const std::string& labelStr, sf::Color normalCol, sf::Color hoverCol,
        const sf::Font& font, std::function<void()> callback);

    // Méthodes
    void update(const sf::Vector2f& mousePos);
    void draw(sf::RenderWindow& window);
    bool isClicked(const sf::Vector2f& mousePos);
    void executeAction();

    // Membres publics (pour compatibilité avec le code existant)
    sf::RectangleShape base;
    sf::Text label;
    sf::Text icon;
    std::function<void()> action;
    bool hover;
    sf::Color normalColor;
    sf::Color hoverColor;

private:
    bool hasIcon;
};

