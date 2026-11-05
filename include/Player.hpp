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
    sf::Vector2f getPosition() const { return position; }

    // Nuevos métodos de combate
    bool getIsAttacking() const { return isAttacking; }
    sf::FloatRect getAttackBox() const;

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

    // Control de combate / ataque
    bool isAttacking;
    sf::Clock attackClock;
    bool prevZPressed;

    // Elementos gráficos y de animación
    sf::Texture texture;
    sf::Sprite sprite;
    
    sf::Clock animationClock;
    int currentFrame;
    float frameTime;
    int facingDirection; // 1 para derecha, -1 para izquierda
};