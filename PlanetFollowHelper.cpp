#include "PlanetFollowHelper.h"
#include <cmath>

PlanetFollowHelper::PlanetFollowHelper() : visible(false), animationTime(0.f) {}

void PlanetFollowHelper::show() {
    visible = true;
    animationTime = 0.f;
}

void PlanetFollowHelper::hide() {
    visible = false;
}

void PlanetFollowHelper::toggle() {
    visible = !visible;
    if (visible) animationTime = 0.f;
}

bool PlanetFollowHelper::isVisible() const {
    return visible;
}

bool PlanetFollowHelper::handleClick(const sf::Vector2f& mousePos) {
    if (!visible) return false;
    
    // Zone du bouton "Fermer"
    sf::FloatRect closeButtonBounds(510.f, 555.f, 180.f, 45.f);
    if (closeButtonBounds.contains(mousePos)) {
        hide();
        return true;
    }
    
    // Clic en dehors du panneau principal le ferme
    sf::FloatRect panelBounds(250.f, 125.f, 700.f, 550.f);
    if (!panelBounds.contains(mousePos)) {
        hide();
   return true;
    }
    
    return false;
}

void PlanetFollowHelper::draw(sf::RenderWindow& window, const sf::Font& font, 
               const std::vector<Planet>& planets, float time) {
    if (!visible) return;

    animationTime += 0.016f;  // Approximation ~60 FPS

    // Fond semi-transparent
    sf::RectangleShape overlay(sf::Vector2f(window.getSize().x, window.getSize().y));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);

 // Panneau principal avec animation d'entrée
    float slideOffset = std::max(0.f, 1.f - animationTime * 3.f) * 200.f;
    
    sf::RectangleShape mainPanel(sf::Vector2f(700.f, 550.f));
    mainPanel.setPosition(250.f, 125.f - slideOffset);
    mainPanel.setFillColor(sf::Color(20, 30, 50, 250));
    mainPanel.setOutlineThickness(4);
    mainPanel.setOutlineColor(sf::Color(100, 200, 255));
    window.draw(mainPanel);

    // En-tête animé
    float headerPulse = time * 1.5f;
 
    sf::RectangleShape header(sf::Vector2f(700.f, 70.f));
    header.setPosition(250.f, 125.f - slideOffset);
    float headerAlpha = 180 + 75 * std::sin(headerPulse);
    header.setFillColor(sf::Color(0, 100, 180, static_cast<sf::Uint8>(headerAlpha)));
    window.draw(header);

    // Titre principal
    sf::Text title("GUIDE DU SUIVI PLANETAIRE", font, 32);
    title.setPosition(600.f, 145.f - slideOffset);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.width / 2.f, 0);
    title.setFillColor(sf::Color::White);
  title.setStyle(sf::Text::Bold);
    title.setOutlineThickness(2);
    title.setOutlineColor(sf::Color(0, 50, 100));
    window.draw(title);

    // Instructions principales
    sf::Text instruction("Appuyez sur les touches ci-dessous pour suivre une planete :", font, 18);
    instruction.setPosition(280.f, 220.f - slideOffset);
    instruction.setFillColor(sf::Color(255, 255, 100));
    window.draw(instruction);

  // Liste des planètes avec touches
    float startY = 260.f - slideOffset;
    float lineHeight = 45.f;

    for (size_t i = 0; i < planets.size() && i < 8; ++i) {
  float currentY = startY + i * lineHeight;

        // Boîte pour chaque planète
        sf::RectangleShape planetBox(sf::Vector2f(640.f, 38.f));
   planetBox.setPosition(280.f, currentY);
        planetBox.setFillColor(sf::Color(40, 50, 80, 200));
        planetBox.setOutlineThickness(2);
        
        // Animation de bordure
 float borderAlpha = 100 + 80 * std::sin(time * 2.f + i * 0.5f);
      planetBox.setOutlineColor(sf::Color(
            planets[i].color.r, 
          planets[i].color.g, 
            planets[i].color.b, 
 static_cast<sf::Uint8>(borderAlpha)
        ));
        window.draw(planetBox);

  // Touche numérique avec effet 3D
        sf::RectangleShape keyShadow(sf::Vector2f(35.f, 30.f));
        keyShadow.setPosition(297.f, currentY + 6.f);
        keyShadow.setFillColor(sf::Color(0, 0, 0, 100));
        window.draw(keyShadow);
        
        sf::RectangleShape keyBox(sf::Vector2f(35.f, 30.f));
        keyBox.setPosition(295.f, currentY + 4.f);
        keyBox.setFillColor(sf::Color(60, 80, 120));
        keyBox.setOutlineThickness(2);
 keyBox.setOutlineColor(sf::Color(150, 200, 255));
        window.draw(keyBox);

    // Numéro de la touche
 sf::Text keyNum(std::to_string(i + 1), font, 22);
        keyNum.setPosition(312.5f, currentY + 7.f);
        sf::FloatRect keyBounds = keyNum.getLocalBounds();
    keyNum.setOrigin(keyBounds.width / 2.f, 0);
        keyNum.setFillColor(sf::Color::White);
     keyNum.setStyle(sf::Text::Bold);
        window.draw(keyNum);

        // Flèche
sf::Text arrow("->", font, 20);
        arrow.setPosition(345.f, currentY + 7.f);
        arrow.setFillColor(sf::Color(100, 200, 255));
        window.draw(arrow);

        // Cercle de couleur de la planète
        sf::CircleShape planetColorCircle(12.f);
 planetColorCircle.setPosition(380.f, currentY + 9.f);
        planetColorCircle.setFillColor(planets[i].color);
        planetColorCircle.setOutlineThickness(2);
        planetColorCircle.setOutlineColor(sf::Color::White);
        window.draw(planetColorCircle);

        // Nom de la planète
        sf::Text planetName(planets[i].getName(), font, 20);
        planetName.setPosition(410.f, currentY + 7.f);
        planetName.setFillColor(sf::Color::White);
        planetName.setStyle(sf::Text::Bold);
        window.draw(planetName);

        // Informations supplémentaires
        std::ostringstream info;
        info << "(" << planets[i].numMoons << " lune" 
   << (planets[i].numMoons > 1 ? "s" : "") << ")";
sf::Text planetInfo(info.str(), font, 14);
     planetInfo.setPosition(650.f, currentY + 10.f);
        planetInfo.setFillColor(sf::Color(180, 200, 220));
        window.draw(planetInfo);
    }

    // Section touches supplémentaires
    float bottomY = startY + 8 * lineHeight + 10.f;

    sf::RectangleShape extraKeysBox(sf::Vector2f(640.f, 60.f));
    extraKeysBox.setPosition(280.f, bottomY);
    extraKeysBox.setFillColor(sf::Color(50, 70, 100, 220));
  extraKeysBox.setOutlineThickness(2);
  extraKeysBox.setOutlineColor(sf::Color(255, 200, 100));
    window.draw(extraKeysBox);

 // Touche 0 - Retour au Soleil
    sf::Text key0Label("Touche [0]", font, 16);
    key0Label.setPosition(295.f, bottomY + 10.f);
 key0Label.setFillColor(sf::Color(255, 200, 100));
    key0Label.setStyle(sf::Text::Bold);
    window.draw(key0Label);

    sf::Text key0Desc("Revenir au Soleil (vue d'ensemble)", font, 15);
    key0Desc.setPosition(295.f, bottomY + 32.f);
    key0Desc.setFillColor(sf::Color(220, 230, 255));
window.draw(key0Desc);

    // Bouton de fermeture animé
    float closePulse = 1.f + 0.05f * std::sin(time * 3.f);
    
    sf::RectangleShape closeBtn(sf::Vector2f(180.f * closePulse, 45.f));
    closeBtn.setPosition(510.f, bottomY + 70.f);
    closeBtn.setOrigin(90.f * (closePulse - 1.f), 0);
    closeBtn.setFillColor(sf::Color(220, 50, 50, 220));
    closeBtn.setOutlineThickness(2);
    closeBtn.setOutlineColor(sf::Color(255, 100, 100));
    window.draw(closeBtn);

    sf::Text closeText("FERMER [ESC]", font, 18);
    closeText.setPosition(600.f, bottomY + 80.f);
    sf::FloatRect closeBounds = closeText.getLocalBounds();
    closeText.setOrigin(closeBounds.width / 2.f, 0);
    closeText.setFillColor(sf::Color::White);
    closeText.setStyle(sf::Text::Bold);
    window.draw(closeText);

    // Note en bas
    sf::Text note("Astuce : Vous pouvez aussi cliquer directement sur une planete pour voir ses details", 
   font, 12);
  note.setPosition(600.f, bottomY + 130.f);
    sf::FloatRect noteBounds = note.getLocalBounds();
    note.setOrigin(noteBounds.width / 2.f, 0);
    note.setFillColor(sf::Color(150, 180, 220));
    note.setStyle(sf::Text::Italic);
    window.draw(note);
}
