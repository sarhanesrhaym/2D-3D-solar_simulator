#include "CosmicButton.h"
#include <cmath>

// ✅ Constructeur simple (3 couleurs)
CosmicButton::CosmicButton(const sf::Vector2f& position, const sf::Vector2f& size,
    const std::string& labelStr, sf::Color normalCol, sf::Color hoverCol,
    sf::Color clickCol, const sf::Font& font, std::function<void()> callback)
    : normalColor(normalCol), hoverColor(hoverCol), clickColor(clickCol),
    hasIcon(false), hover(false), clicked(false), clickAnimationTimer(0.f),
    action(callback) {

    base.setSize(size);
    base.setPosition(position);
    base.setFillColor(normalCol);
    base.setOutlineThickness(2.f);
    base.setOutlineColor(sf::Color(255, 255, 255, 100));

    label.setString(labelStr);
    label.setFont(font);
    label.setCharacterSize(18);
    label.setFillColor(sf::Color::White);
    label.setStyle(sf::Text::Bold);

    sf::FloatRect labelBounds = label.getLocalBounds();
    label.setPosition(
        position.x + (size.x - labelBounds.width) / 2.f,
        position.y + (size.y - labelBounds.height) / 2.f - 5.f
    );
}

// ✅ Constructeur avec icône (3 couleurs)
CosmicButton::CosmicButton(const sf::Vector2f& position, const sf::Vector2f& size,
    const std::string& iconStr, const std::string& labelStr,
    sf::Color normalCol, sf::Color hoverCol, sf::Color clickCol,
    const sf::Font& font, std::function<void()> callback)
    : normalColor(normalCol), hoverColor(hoverCol), clickColor(clickCol),
    hasIcon(true), hover(false), clicked(false), clickAnimationTimer(0.f),
    action(callback) {

    base.setSize(size);
    base.setPosition(position);
    base.setFillColor(normalCol);
    base.setOutlineThickness(2.f);
    base.setOutlineColor(sf::Color(255, 255, 255, 100));

    icon.setString(iconStr);
    icon.setFont(font);
    icon.setCharacterSize(24);
    icon.setFillColor(sf::Color::White);
    icon.setStyle(sf::Text::Bold);

    sf::FloatRect iconBounds = icon.getLocalBounds();
    icon.setPosition(
        position.x + 10.f,
        position.y + (size.y - iconBounds.height) / 2.f - 5.f
    );

    label.setString(labelStr);
    label.setFont(font);
    label.setCharacterSize(16);
    label.setFillColor(sf::Color::White);

    sf::FloatRect labelBounds = label.getLocalBounds();
    label.setPosition(
        position.x + 50.f,
        position.y + (size.y - labelBounds.height) / 2.f - 5.f
    );
}

// ✅ CORRIGÉ : Update utilise maintenant les couleurs passées au constructeur
void CosmicButton::update(const sf::Vector2f& mousePos) {
    sf::FloatRect bounds = base.getGlobalBounds();
    hover = bounds.contains(mousePos);

    // Diminuer le timer d'animation
    if (clickAnimationTimer > 0.f) {
        clickAnimationTimer -= 0.016f;
    }
    else {
        clicked = false;
    }

    // ✅ UTILISER LES COULEURS DU CONSTRUCTEUR
    if (clicked) {
        // Couleur de clic avec effet lumineux
        base.setFillColor(clickColor);
        base.setOutlineThickness(3.f);
        // Créer un halo basé sur la couleur de clic
        base.setOutlineColor(sf::Color(
            std::min(255, clickColor.r + 50),
            std::min(255, clickColor.g + 50),
            std::min(255, clickColor.b + 50),
            200
        ));
    }
    else if (hover) {
        // Couleur de survol avec effet lumineux
        base.setFillColor(hoverColor);
        base.setOutlineThickness(2.5f);
        // Créer un halo basé sur la couleur de survol
        base.setOutlineColor(sf::Color(
            std::min(255, hoverColor.r + 30),
            std::min(255, hoverColor.g + 30),
            std::min(255, hoverColor.b + 30),
            180
        ));
    }
    else {
        // Couleur normale
        base.setFillColor(normalColor);
        base.setOutlineThickness(2.f);
        base.setOutlineColor(sf::Color(255, 255, 255, 100));
    }
}

void CosmicButton::draw(sf::RenderWindow& window) {
    // ✅ Lueur pulsante autour du bouton au survol/clic
    if (hover || clicked) {
        sf::RectangleShape glow = base;
        glow.setFillColor(sf::Color::Transparent);

        if (clicked) {
            // Glow basé sur clickColor
            glow.setOutlineThickness(6.f);
            glow.setOutlineColor(sf::Color(clickColor.r, clickColor.g, clickColor.b, 100));
        }
        else {
            // Glow basé sur hoverColor
            glow.setOutlineThickness(5.f);
            glow.setOutlineColor(sf::Color(hoverColor.r, hoverColor.g, hoverColor.b, 80));
        }
        window.draw(glow);
    }

    // Dessiner le bouton
    window.draw(base);

    if (hasIcon) {
        window.draw(icon);
    }
    window.draw(label);
}

bool CosmicButton::isClicked(const sf::Vector2f& mousePos) {
    sf::FloatRect bounds = base.getGlobalBounds();
    return bounds.contains(mousePos);
}

// ✅ Execute action et marque le bouton comme cliqué
void CosmicButton::executeAction() {
    if (action) {
        clicked = true;
        clickAnimationTimer = 0.2f;  // Animation de 200ms
        action();
    }
}

void CosmicButton::resetState() {
    clicked = false;
    hover = false;
    clickAnimationTimer = 0.f;
    base.setFillColor(normalColor);
}