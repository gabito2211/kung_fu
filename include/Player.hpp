#pragma once
#include <SFML/Graphics.hpp>

class Player {
public:
    Player();
    void handleInput();
    void update();
    void draw(sf::RenderWindow& window);

    // Getters necesarios para el orden de dibujado (Y-sorting)
    float getDepthY() const { return position.y; }

private:
    sf::Vector2f position; // Posición en el suelo (X, Y)
    sf::Vector2f velocity; // Velocidad en X y Y
    float z;               // Altura actual del salto (eje Z)
    float vz;              // Velocidad vertical del salto

    // Nuevas variables de física de movimiento
    sf::Vector2f acceleration;
    float maxSpeed;        // Velocidad máxima permitida
    float accRate;         // Tasa de aceleración al arrancar
    float friction;        // Factor de desaceleración (resbalamiento)

    sf::RectangleShape shape; // Placeholder visual provisional
};
