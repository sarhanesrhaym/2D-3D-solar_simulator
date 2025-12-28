#include "Star.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>

Star::Star() {
    float x = (std::rand() % 2400 - 1200);
    float y = (std::rand() % 1600 - 800);
    float radius = (std::rand() % 100) / 100.f * 1.5f + 0.3f;

    shape = sf::CircleShape(radius);
    shape.setPosition(x, y);
    shape.setFillColor(sf::Color::White);

    twinkleSpeed = (std::rand() % 100) / 100.f * 3.f + 1.f;
    baseAlpha = 150 + (std::rand() % 105);
}

void Star::update(float time) {
    float alpha = baseAlpha + 50 * std::sin(time * twinkleSpeed);
    alpha = std::max(0.f, std::min(255.f, alpha));
    shape.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
}

void Star::draw(sf::RenderWindow& window) {
    window.draw(shape);
}