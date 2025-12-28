#include "CosmicButton.h"

// Constructeur avec icône
CosmicButton::CosmicButton(const sf::Vector2f& position, const sf::Vector2f& size,
    const std::string& iconStr, const std::string& labelStr,
    sf::Color normalCol, sf::Color hoverCol,
    const sf::Font& font, std::function<void()> callback) {
    hasIcon = true;
    hover = false;
    normalColor = normalCol;
    hoverColor = hoverCol;
    action = callback;

    // Configuration du rectangle de base
    base = sf::RectangleShape(size);
    base.setPosition(position);
    base.setFillColor(normalColor);
    base.setOutlineThickness(2);
    base.setOutlineColor(sf::Color(255, 255, 255, 100));

    // Configuration de l'icône
    icon = sf::Text(iconStr, font, 20);
    icon.setPosition(position.x + 10, position.y + 12);
    icon.setFillColor(sf::Color::White);
    icon.setStyle(sf::Text::Bold);

    // Configuration du label
    label = sf::Text(labelStr, font, 16);
    label.setPosition(position.x + 45, position.y + 15);
    label.setFillColor(sf::Color::White);
    label.setStyle(sf::Text::Bold);
}

// Constructeur simple (menu principal)
CosmicButton::CosmicButton(const sf::Vector2f& position, const sf::Vector2f& size,
    const std::string& labelStr, sf::Color normalCol, sf::Color hoverCol,
    const sf::Font& font, std::function<void()> callback) {
    hasIcon = false;
    hover = false;
    normalColor = normalCol;
    hoverColor = hoverCol;
    action = callback;

    // Configuration du rectangle de base
    base = sf::RectangleShape(size);
    base.setPosition(position);
    base.setFillColor(normalColor);
    base.setOutlineThickness(2);
    base.setOutlineColor(sf::Color::White);

    // Configuration du label (centré)
    label = sf::Text(labelStr, font, 20);
    label.setPosition(position.x + 20, position.y + 15);
    label.setFillColor(sf::Color::White);
}

void CosmicButton::update(const sf::Vector2f& mousePos) {
    hover = base.getGlobalBounds().contains(mousePos);
    base.setFillColor(hover ? hoverColor : normalColor);
    base.setOutlineThickness(hover ? 3 : 2);
}

void CosmicButton::draw(sf::RenderWindow& window) {
    window.draw(base);
    if (hasIcon) {
        window.draw(icon);
    }
    window.draw(label);
}

bool CosmicButton::isClicked(const sf::Vector2f& mousePos) {
    return base.getGlobalBounds().contains(mousePos);
}

void CosmicButton::executeAction() {
    if (action) {
        action();
    }
}