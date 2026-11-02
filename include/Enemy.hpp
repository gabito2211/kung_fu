#pragma once
#include <SFML/Graphics.hpp>
#include "Tilemap.hpp"
#include "Player.hpp"

class Enemy {
public:
    Enemy(float startX, float startY);
    void update(const Tilemap& tilemap, const Player& player);
    void draw(sf::RenderWindow& window);

    sf::FloatRect getBounds() const;
    bool hasFallen() const { return fallenToVoid; }

private:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float z;
    float vz;

    sf::Vector2f acceleration;
    float maxSpeed;
    float accRate;
    float friction;

    // Control de caídas y saltos
    bool isFallingIntoVoid;
    bool fallenToVoid;

    sf::RectangleShape shape;
};