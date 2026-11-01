#pragma once
#include <SFML/Graphics.hpp>

class Box {
public:
    Box(float x, float y, float width, float height);
    void draw(sf::RenderWindow& window);

    // Retorna el área de colisión de la caja en el suelo
    sf::FloatRect getBounds() const;

private:
    sf::RectangleShape shape;
    sf::Vector2f position;
    sf::Vector2f size;
};