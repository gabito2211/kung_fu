#include "Box.hpp"

Box::Box(float x, float y, float width, float height) {
    position = sf::Vector2f(x, y);
    size = sf::Vector2f(width, height);

    shape.setSize(size);
    shape.setFillColor(sf::Color(139, 69, 19)); // Color marrón madera
    shape.setOutlineColor(sf::Color(80, 40, 10));
    shape.setOutlineThickness(2.f);
    shape.setPosition(position);
}

void Box::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

sf::FloatRect Box::getBounds() const {
    return sf::FloatRect(position.x, position.y, size.x, size.y);
}