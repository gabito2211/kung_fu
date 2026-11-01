#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Constants.hpp"
#include <cmath>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Kung Fu - 2.5D Engine Test");
    window.setFramerateLimit(60);

    Player player;

    // Líneas guía visuales para el suelo y la profundidad
    sf::RectangleShape floorBoundary(sf::Vector2f(800.f, FLOOR_Y_MAX - FLOOR_Y_MIN));
    floorBoundary.setPosition(0.f, FLOOR_Y_MIN);
    floorBoundary.setFillColor(sf::Color(70, 70, 70));

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // 1. Manejo de entrada
        player.handleInput();

        // 2. Actualización de lógicas
        player.update();

        // 3. Dibujado
        window.clear(sf::Color(30, 30, 30));

        // Dibujar área de suelo/profundidad
        window.draw(floorBoundary);

        // Dibujar jugador
        player.draw(window);

        window.display();
    }

    return 0;
}