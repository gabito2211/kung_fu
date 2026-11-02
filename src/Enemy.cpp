#include "Enemy.hpp"
#include "Constants.hpp"
#include <cmath>

Enemy::Enemy(float startX, float startY) {
    position = sf::Vector2f(startX, startY);
    velocity = sf::Vector2f(0.f, 0.f);
    acceleration = sf::Vector2f(0.f, 0.f);
    z = 0.f;
    vz = 0.f;

    maxSpeed = 3.5f;
    accRate = 0.35f;
    friction = 0.82f;

    isFallingIntoVoid = false;
    fallenToVoid = false;

    shape.setSize(sf::Vector2f(40.f, 80.f));
    shape.setFillColor(sf::Color::Red);
    shape.setOrigin(20.f, 80.f);
}

sf::FloatRect Enemy::getBounds() const {
    return sf::FloatRect(position.x - 15.f, position.y - 10.f, 30.f, 20.f);
}

void Enemy::update(const Tilemap& tilemap, const Player& player) {
    // Si ya cayó al abismo, aplicamos caída libre hacia abajo
    if (isFallingIntoVoid) {
        vz -= GRAVITY;
        z += vz;
        if (z < -100.f) fallenToVoid = true;
        shape.setPosition(position.x, position.y - z);
        return;
    }

    acceleration = sf::Vector2f(0.f, 0.f);

    sf::Vector2f playerPos = player.getPosition();

    // Persecución en Eje X e Y hacia el jugador
    if (playerPos.x < position.x - 5.f) {
        acceleration.x = -accRate;
    } else if (playerPos.x > position.x + 5.f) {
        acceleration.x = accRate;
    }

    if (playerPos.y < position.y - 5.f) {
        acceleration.y = -accRate;
    } else if (playerPos.y > position.y + 5.f) {
        acceleration.y = accRate;
    }

    velocity += acceleration;

    if (acceleration.x == 0.f) {
        velocity.x *= friction;
        if (std::abs(velocity.x) < 0.05f) velocity.x = 0.f;
    }
    if (acceleration.y == 0.f) {
        velocity.y *= friction;
        if (std::abs(velocity.y) < 0.05f) velocity.y = 0.f;
    }

    float currentSpeedSq = (velocity.x * velocity.x) + (velocity.y * velocity.y);
    if (currentSpeedSq > (maxSpeed * maxSpeed)) {
        float currentSpeed = std::sqrt(currentSpeedSq);
        velocity.x = (velocity.x / currentSpeed) * maxSpeed;
        velocity.y = (velocity.y / currentSpeed) * maxSpeed;
    }

    float oldX = position.x;
    float oldY = position.y;

    // Colisión Eje X
    position.x += velocity.x;
    if (tilemap.getTileAtPixel(position.x, position.y) == TILE_BOX) {
        position.x = oldX;
        velocity.x = 0.f;
    }

    // Colisión Eje Y
    position.y += velocity.y;
    if (tilemap.getTileAtPixel(position.x, position.y) == TILE_BOX) {
        position.y = oldY;
        velocity.y = 0.f;
    }

    // --- DETECCIÓN ANTICIPADA DE ABISMO PARA SALTAR ---
    // Si está en el suelo, revisamos unos píxeles adelante en la dirección de movimiento
    if (z == 0.f) {
        float lookAheadX = position.x + (velocity.x > 0 ? 30.f : -30.f);
        if (tilemap.getTileAtPixel(lookAheadX, position.y) == TILE_VOID) {
            vz = 12.5f; // Salto automático para esquivar el foso
        }
    }

    // Física de salto (Eje Z)
    if (z > 0.f || vz != 0.f) {
        vz -= GRAVITY;
        z += vz;
        if (z <= 0.f) {
            z = 0.f;
            vz = 0.f;
        }
    }

    // Comprobar si pisa directamente el vacío estando en el suelo
    if (z == 0.f && tilemap.getTileAtPixel(position.x, position.y) == TILE_VOID) {
        isFallingIntoVoid = true;
        vz = -1.0f;
    }

    shape.setPosition(position.x, position.y - z);
}

void Enemy::draw(sf::RenderWindow& window) {
    window.draw(shape);
}