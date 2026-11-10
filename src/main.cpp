#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Enemy.hpp"
#include "Tilemap.hpp"
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Kung Fu - Enemy Pursuit Test");
    window.setFramerateLimit(60);

    Player player;
    Enemy enemy(600.f, 480.f); // Aparece al otro lado del mapa
    Tilemap tilemap;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // EDITAR EL TILEMAP CON EL RATÓN:
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                int gridX = event.mouseButton.x / TILE_SIZE;
                int gridY = event.mouseButton.y / TILE_SIZE;

                // Rotar el tipo de Tile en la casilla cliqueada: 0 -> 1 -> 2 -> 3 -> 0
                TileType current = tilemap.getTileAtPixel(event.mouseButton.x, event.mouseButton.y);
                TileType next = static_cast<TileType>((static_cast<int>(current) + 1) % 4);

                tilemap.setTile(gridX, gridY, next);
            }
        }

        // 1. Actualizar entradas y físicas
        player.handleInput();
        player.update(tilemap);
        
        // Actualizar la inteligencia artificial del enemigo
        enemy.update(tilemap, player);

        if (player.hasFallen()) {
            window.close();
        }
// 1. Validar si el Jugador ataca al Enemigo
        if ((player.getIsAttacking() || player.getIsKicking()) && !enemy.getIsHurt()) {
            sf::FloatRect playerHitbox = player.getIsAttacking() ? player.getAttackBox() : player.getKickBox();
            
            if (playerHitbox.intersects(enemy.getBounds())) {
                std::cout << "¡IMPACTO! El jugador golpeó al enemigo.\n";
                enemy.takeHit(player.getPosition().x);
            }
        }

        // 2. Validar si el Enemigo ataca al Jugador
        if ((enemy.getIsAttacking() || enemy.getIsKicking()) && !player.getIsHurt()) {
            sf::FloatRect enemyHitbox = enemy.getIsAttacking() ? enemy.getAttackBox() : enemy.getKickBox();
            
            if (enemyHitbox.intersects(player.getBounds())) {
                std::cout << "¡Ouch! El enemigo golpeó al jugador.\n";
                player.takeHit(enemy.getPosition().x);
            }
        }
        // 3. Renderizado en pantalla
        window.clear(sf::Color(10, 10, 10));

        tilemap.draw(window);
        enemy.draw(window);
        player.draw(window);

        window.display();
    }

    return 0;
}