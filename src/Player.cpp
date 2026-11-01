#include "Player.hpp"
#include "Constants.hpp"
#include <cmath>

Player::Player() {
    position = sf::Vector2f(150.f, 480.f);
    velocity = sf::Vector2f(0.f, 0.f);
    acceleration = sf::Vector2f(0.f, 0.f);
    z = 0.f;
    vz = 0.f;

    walkMaxSpeed = 4.0f;
    runMaxSpeed = 8.0f;
    maxSpeed = walkMaxSpeed;
    accRate = 0.4f;
    friction = 0.82f;

    lastDirection = 0;
    wasKeyReleased = true;
    isRunning = false;
    prevLeftPressed = false;
    prevRightPressed = false;

    isFallingIntoVoid = false;
    fallenToVoid = false;

    shape.setSize(sf::Vector2f(40.f, 80.f));
    shape.setFillColor(sf::Color::Cyan);
    shape.setOrigin(20.f, 80.f);
}

sf::FloatRect Player::getBounds() const {
    return sf::FloatRect(position.x - 15.f, position.y - 10.f, 30.f, 20.f);
}

void Player::handleInput() {
    if (isFallingIntoVoid) return;

    acceleration = sf::Vector2f(0.f, 0.f);

    bool currentLeft = sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
    bool currentRight = sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right);

    if (currentLeft && !prevLeftPressed) {
        if (lastDirection == -1 && wasKeyReleased && doubleTapClock.getElapsedTime().asMilliseconds() < 300) {
            isRunning = true;
        } else {
            lastDirection = -1;
            isRunning = false;
        }
        doubleTapClock.restart();
        wasKeyReleased = false;
    }
    if (!currentLeft && prevLeftPressed) wasKeyReleased = true;

    if (currentRight && !prevRightPressed) {
        if (lastDirection == 1 && wasKeyReleased && doubleTapClock.getElapsedTime().asMilliseconds() < 300) {
            isRunning = true;
        } else {
            lastDirection = 1;
            isRunning = false;
        }
        doubleTapClock.restart();
        wasKeyReleased = false;
    }
    if (!currentRight && prevRightPressed) wasKeyReleased = true;

    prevLeftPressed = currentLeft;
    prevRightPressed = currentRight;

    maxSpeed = isRunning ? runMaxSpeed : walkMaxSpeed;

    if (currentLeft) acceleration.x = -accRate;
    if (currentRight) acceleration.x = accRate;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        acceleration.y = -accRate;
        isRunning = false;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        acceleration.y = accRate;
        isRunning = false;
    }

    if (!currentLeft && !currentRight && std::abs(velocity.x) < 0.5f) isRunning = false;

    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Keyboard::isKeyPressed(sf::Keyboard::K)) && z == 0.f) {
        vz = 12.5f;
    }
}

void Player::update(const Tilemap& tilemap) {
    if (isFallingIntoVoid) {
        vz -= GRAVITY;
        z += vz;
        if (z < -100.f) fallenToVoid = true;
        shape.setPosition(position.x, position.y - z);
        return;
    }

    velocity += acceleration;

    if (acceleration.x == 0.f) {
        velocity.x *= friction;
        if (std::abs(velocity.x) < 0.05f) { velocity.x = 0.f; isRunning = false; }
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

    // Guardar posiciones anteriores antes de movernos
    float oldX = position.x;
    float oldY = position.y;

    // 1. Mover y comprobar Eje X (Laterales horizontales)
    position.x += velocity.x;
    TileType tileX = tilemap.getTileAtPixel(position.x, position.y);
    TileType prevTileX = tilemap.getTileAtPixel(oldX, position.y);

    if (tileX == TILE_BOX) {
        position.x = oldX;
        velocity.x = 0.f;
        isRunning = false;
    } else if (prevTileX == TILE_FLOOR && tileX == TILE_HIGH_FLOOR && z < 15.f) {
        // Bloquear entrada lateral (X) si no hay suficiente salto
        position.x = oldX;
        velocity.x = 0.f;
        isRunning = false;
    }

    // 2. Mover y comprobar Eje Y (Laterales verticales / profundidad)
    position.y += velocity.y;
    TileType tileY = tilemap.getTileAtPixel(position.x, position.y);
    TileType prevTileY = tilemap.getTileAtPixel(position.x, oldY);

    if (tileY == TILE_BOX) {
        position.y = oldY;
        velocity.y = 0.f;
    } else if (prevTileY == TILE_FLOOR && tileY == TILE_HIGH_FLOOR && z < 15.f) {
        // Bloquear entrada vertical (Y) si no hay suficiente salto
        position.y = oldY;
        velocity.y = 0.f;
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

    // Comprobar caída al vacío (TILE_VOID) si está en el suelo (z == 0)
    if (z == 0.f && tilemap.getTileAtPixel(position.x, position.y) == TILE_VOID) {
        isFallingIntoVoid = true;
        vz = -1.0f;
    }

    shape.setPosition(position.x, position.y - z);
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(shape);
}