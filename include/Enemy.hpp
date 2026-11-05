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

    bool isFallingIntoVoid;
    bool fallenToVoid;

    // Control de IA para correr y atacar
    bool isRunning;
    bool isAttacking;
    sf::Clock attackCooldownClock; // Tiempo entre ataques

    // Elementos gráficos y de animación
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Clock animationClock;
    int currentFrame;
    float frameTime;
    int facingDirection;
};