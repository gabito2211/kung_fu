#include "Enemy.hpp"
#include "Constants.hpp"
#include <cmath>
#include <iostream>

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

    currentFrame = 0;
    frameTime = 0.12f;
    facingDirection = 1;

    // Cargar la textura del enemigo (ajusta la ruta según tu archivo de sprites)
    if (!texture.loadFromFile("assets/sprites/alex.png")) {
        std::cerr << "Error al cargar la textura del enemigo desde assets/sprites/alex.png\n";
    }

    sprite.setTexture(texture);
    sprite.setTextureRect(sf::IntRect(0, 0, 24, 32)); // Tamaño base del fotograma
    sprite.setOrigin(12.f, 32.f); // Centrado en los pies
    sprite.scale(2.5f, 2.5f);
}

sf::FloatRect Enemy::getBounds() const {
    return sf::FloatRect(position.x - 15.f, position.y - 10.f, 30.f, 20.f);
}

void Enemy::update(const Tilemap& tilemap, const Player& player) {
    if (isFallingIntoVoid) {
        vz -= GRAVITY;
        z += vz;
        if (z < -100.f) fallenToVoid = true;
        sprite.setPosition(position.x, position.y - z);
        return;
    }

    acceleration = sf::Vector2f(0.f, 0.f);

    sf::Vector2f playerPos = player.getPosition();

    // Persecución y orientación horizontal hacia el jugador
    if (playerPos.x < position.x - 5.f) {
        acceleration.x = -accRate;
        facingDirection = -1; // Voltear a la izquierda
    } else if (playerPos.x > position.x + 5.f) {
        acceleration.x = accRate;
        facingDirection = 1;  // Mirar a la derecha
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

    position.x += velocity.x;
    if (tilemap.getTileAtPixel(position.x, position.y) == TILE_BOX) {
        position.x = oldX;
        velocity.x = 0.f;
    }

    position.y += velocity.y;
    if (tilemap.getTileAtPixel(position.x, position.y) == TILE_BOX) {
        position.y = oldY;
        velocity.y = 0.f;
    }

    // Detección anticipada de abismo para saltar automático
    if (z == 0.f) {
        float lookAheadX = position.x + (velocity.x > 0 ? 30.f : -30.f);
        if (tilemap.getTileAtPixel(lookAheadX, position.y) == TILE_VOID) {
            vz = 12.5f;
        }
    }

    if (z > 0.f || vz != 0.f) {
        vz -= GRAVITY;
        z += vz;
        if (z <= 0.f) {
            z = 0.f;
            vz = 0.f;
        }
    }

    if (z == 0.f && tilemap.getTileAtPixel(position.x, position.y) == TILE_VOID) {
        isFallingIntoVoid = true;
        vz = -1.0f;
    }

    // --- LÓGICA DE ANIMACIÓN DEL ENEMIGO ---
    int frameWidth = 28;
    int frameHeight = 36;
    int startX = 0;
    int rowY = 0;
    int numFrames = 1;

    if (z > 0.f) {
        rowY = 0; // Fila de salto en la hoja
        startX = 112;
        numFrames = 1;
    } else if (std::abs(velocity.x) > 0.5f || std::abs(velocity.y) > 0.5f) {
        rowY = 0; // Fila de movimiento
        startX = 0;
        numFrames = 4;
    } else {
        rowY = 0; // Estado reposo
        startX = 0;
        numFrames = 1;
    }

    if (animationClock.getElapsedTime().asSeconds() > frameTime) {
        currentFrame = (currentFrame + 1) % numFrames;
        animationClock.restart();
    }

    sprite.setTextureRect(sf::IntRect(startX + (currentFrame * frameWidth), rowY, frameWidth, frameHeight));
    sprite.setScale(facingDirection * 2.5f, 2.5f);
    sprite.setPosition(position.x, position.y - z);
}

void Enemy::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}