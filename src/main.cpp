#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Enemy.hpp"
#include "Tilemap.hpp"
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Kung Fu - Combat System");
    window.setFramerateLimit(60);

    Player player;
    Enemy enemy(600.f, 480.f);
    Tilemap tilemap;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                int gridX = event.mouseButton.x / TILE_SIZE;
                int gridY = event.mouseButton.y / TILE_SIZE;
                TileType current = tilemap.getTileAtPixel(event.mouseButton.x, event.mouseButton.y);
                TileType next = static_cast<TileType>((static_cast<int>(current) + 1) % 4);
                tilemap.setTile(gridX, gridY, next);
            }
        }

        // 1. Actualización
        if (!player.isDead()) {
            player.handleInput();
            player.update(tilemap);
        }
        
        if (!enemy.isDead()) {
            enemy.update(tilemap, player);
        }

        // --- 2. SISTEMA DE COMBATE Y COLISIONES ---
        
        // A. Jugador ataca al Enemigo
        if (!player.isDead() && !enemy.isDead() && !enemy.getIsHurt()) {
            bool attacking = player.getIsAttacking() || player.getIsKicking() || player.getIsUppercut() || player.getIsHighKick();
            
            if (attacking) {
                sf::FloatRect hitbox;
                int damage = 5;
                bool isFinisher = false;

                if (player.getIsAttacking()) { hitbox = player.getAttackBox(); damage = 5; }
                else if (player.getIsKicking()) { hitbox = player.getKickBox(); damage = 10; }
                else if (player.getIsUppercut()) { hitbox = player.getUppercutBox(); damage = 25; isFinisher = true; }
                else if (player.getIsHighKick()) { hitbox = player.getHighKickBox(); damage = 30; isFinisher = true; }

                if (hitbox.intersects(enemy.getBounds())) {
                    std::cout << "¡IMPACTO al enemigo! Daño: " << damage << (isFinisher ? " [REMATE ESPECIAL]\n" : "\n");
                    enemy.takeHit(player.getPosition().x, damage, isFinisher);
                }
            }
        }

        // B. Enemigo ataca al Jugador
        if (!player.isDead() && !enemy.isDead() && !player.getIsHurt()) {
            bool attacking = enemy.getIsAttacking() || enemy.getIsKicking() || enemy.getIsUppercut() || enemy.getIsHighKick();
            
            if (attacking) {
                sf::FloatRect hitbox;
                int damage = 5;
                bool isFinisher = false;

                if (enemy.getIsAttacking()) { hitbox = enemy.getAttackBox(); damage = 5; }
                else if (enemy.getIsKicking()) { hitbox = enemy.getKickBox(); damage = 10; }
                else if (enemy.getIsUppercut()) { hitbox = enemy.getUppercutBox(); damage = 25; isFinisher = true; }
                else if (enemy.getIsHighKick()) { hitbox = enemy.getHighKickBox(); damage = 30; isFinisher = true; }

                if (hitbox.intersects(player.getBounds())) {
                    std::cout << "¡Ouch, daño recibido! Daño: " << damage << (isFinisher ? " [REMATE RECIBIDO]\n" : "\n");
                    player.takeHit(enemy.getPosition().x, damage, isFinisher);
                }
            }
        }   

        // 3. Renderizado
        window.clear(sf::Color(10, 10, 10));

        tilemap.draw(window);
        
        if (!enemy.isDead()) {
            enemy.draw(window);
        }
        
        if (!player.isDead()) {
            player.draw(window);
        }

        window.display();
    }

    return 0;
}