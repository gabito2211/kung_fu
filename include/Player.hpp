#pragma once
#include <SFML/Graphics.hpp>
#include "Tilemap.hpp"

class Player {
public:
    Player();
    void handleInput();
    void update(const Tilemap& tilemap);
    void draw(sf::RenderWindow& window);

    float getDepthY() const { return position.y; }
    sf::FloatRect getBounds() const;
    bool hasFallen() const { return fallenToVoid; }

private:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float z;
    float vz;

    sf::Vector2f acceleration;
    float walkMaxSpeed;
    float runMaxSpeed;
    float maxSpeed;
    float accRate;
    float friction;

    sf::Clock doubleTapClock;
    int lastDirection;
    bool wasKeyReleased;
    bool isRunning;
    bool prevLeftPressed;
    bool prevRightPressed;

    bool isFallingIntoVoid;
    bool fallenToVoid;

    sf::RectangleShape shape;
};