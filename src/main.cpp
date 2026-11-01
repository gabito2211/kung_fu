#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Box.hpp"
#include "Constants.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Kung Fu - Void Fall Test");
    window.setFramerateLimit(60);

    Player player;
    Box obstacle(250.f, 440.f, 50.f, 50.f); // Caja movida a la izquierda del foso

    // Suelo principal a la izquierda y derecha
    sf::RectangleShape floorLeft(sf::Vector2f(500.f, FLOOR_Y_MAX - FLOOR_Y_MIN));
    floorLeft.setPosition(0.f, FLOOR_Y_MIN);
    floorLeft.setFillColor(sf::Color(70, 70, 70));

    sf::RectangleShape floorRight(sf::Vector2f(150.f, FLOOR_Y_MAX - FLOOR_Y_MIN));
    floorRight.setPosition(650.f, FLOOR_Y_MIN);
    floorRight.setFillColor(sf::Color(70, 70, 70));

    // Representación visual del foso/vacío (color negro oscuro)
    sf::RectangleShape voidPit(sf::Vector2f(150.f, FLOOR_Y_MAX - FLOOR_Y_MIN));
    voidPit.setPosition(500.f, FLOOR_Y_MIN);
    voidPit.setFillColor(sf::Color(10, 10, 10));

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        player.handleInput();
        player.update(obstacle);

        // Si el jugador cayó completamente al vacío, cerramos el juego
        if (player.hasFallen()) {
            window.close();
        }

        window.clear(sf::Color(30, 30, 30));

        // Dibujar elementos del escenario
        window.draw(floorLeft);
        window.draw(floorRight);
        window.draw(voidPit); // El foso negro

        obstacle.draw(window);
        player.draw(window);

        window.display();
    }

    return 0;
}