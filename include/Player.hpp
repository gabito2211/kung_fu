#pragma once
#include <SFML/Graphics.hpp>
#include "Box.hpp"

class Player {
public:
    Player();
    void handleInput();
    void update(const Box& obstacle);
    void draw(sf::RenderWindow& window);

    float getDepthY() const { return position.y; }
    sf::FloatRect getBounds() const;
    
    // Método para saber si ya cayó completamente al vacío
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

    // Control de caída al vacío
    bool isFallingIntoVoid;
    bool fallenToVoid;

    sf::RectangleShape shape;
};