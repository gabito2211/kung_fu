#include "Enemy.hpp"
#include "Constants.hpp"
#include <cmath>
#include <iostream>
#include <cstdlib> // Necesario para std::rand()

Enemy::Enemy(float startX, float startY) {
    position = sf::Vector2f(startX, startY);
    velocity = sf::Vector2f(0.f, 0.f);
    acceleration = sf::Vector2f(0.f, 0.f);
    z = 0.f;
    vz = 0.f;

    walkMaxSpeed = 3.0f;
    runMaxSpeed = 6.5f;
    maxSpeed = walkMaxSpeed;
    accRate = 0.35f;
    friction = 0.82f;

    isFallingIntoVoid = false;
    fallenToVoid = false;
    isRunning = false;
    isAttacking = false;
    isKicking = false;

    currentFrame = 0;
    frameTime = 0.12f;
    facingDirection = 1;

    if (!texture.loadFromFile("assets/sprites/alex.png")) {
        std::cerr << "Error al cargar la textura del enemigo\n";
    }

    sprite.setTexture(texture);
    sprite.setTextureRect(sf::IntRect(0, 0, 24, 32));
    sprite.setOrigin(12.f, 32.f);
    sprite.scale(2.5f, 2.5f);
}

sf::FloatRect Enemy::getBounds() const {
    return sf::FloatRect(position.x - 15.f, position.y - 10.f, 30.f, 20.f);
}

sf::FloatRect Enemy::getAttackBox() const {
    float attackWidth = 25.f;
    float attackHeight = 20.f;
    float attackX = (facingDirection == 1) ? position.x + 5.f : position.x - 5.f - attackWidth;
    float attackY = position.y - 30.f;
    return sf::FloatRect(attackX, attackY, attackWidth, attackHeight);
}

sf::FloatRect Enemy::getKickBox() const {
    float kickWidth = 30.f;
    float kickHeight = 20.f;
    float kickX = (facingDirection == 1) ? position.x + 5.f : position.x - 5.f - kickWidth;
    float kickY = position.y - 25.f;
    return sf::FloatRect(kickX, kickY, kickWidth, kickHeight);
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

    float distX = playerPos.x - position.x;
    float distY = playerPos.y - position.y;
    float distance = std::sqrt(distX * distX + distY * distY);

    if (distance > 200.f) {
        isRunning = true;
        maxSpeed = runMaxSpeed;
    } else {
        isRunning = false;
        maxSpeed = walkMaxSpeed;
    }

    // --- IA: DECIDIR SI ATACA (ALEATORIO ENTRE PUÑETAZO Y PATADA) ---
    if (distance < 45.f && z == 0.f && !isAttacking && !isKicking && attackCooldownClock.getElapsedTime().asSeconds() > 1.2f) {
        int choice = std::rand() % 2; // 0 = Puñetazo, 1 = Patada
        if (choice == 0) {
            isAttacking = true;
        } else {
            isKicking = true;
        }
        attackCooldownClock.restart();
        currentFrame = 0;
    }

    // Si está realizando un ataque, se detiene
    if (isAttacking || isKicking) {
        acceleration.x = 0.f;
        acceleration.y = 0.f;
        if (attackCooldownClock.getElapsedTime().asMilliseconds() > 350) {
            isAttacking = false;
            isKicking = false;
        }
    } else {
        if (distX < -5.f) {
            acceleration.x = -accRate;
            facingDirection = -1;
        } else if (distX > 5.f) {
            acceleration.x = accRate;
            facingDirection = 1;
        }

        if (distY < -5.f) {
            acceleration.y = -accRate;
        } else if (distY > 5.f) {
            acceleration.y = accRate;
        }
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
    TileType tileX = tilemap.getTileAtPixel(position.x, position.y);
    TileType prevTileX = tilemap.getTileAtPixel(oldX, position.y);

    if (tileX == TILE_BOX) {
        position.x = oldX;
        velocity.x = 0.f;
    } else if (prevTileX == TILE_FLOOR && tileX == TILE_HIGH_FLOOR && z < 15.f) {
        position.x = oldX;
        velocity.x = 0.f;
        if (z == 0.f) vz = 12.5f;
    }

    position.y += velocity.y;
    TileType tileY = tilemap.getTileAtPixel(position.x, position.y);
    TileType prevTileY = tilemap.getTileAtPixel(position.x, oldY);

    if (tileY == TILE_BOX) {
        position.y = oldY;
        velocity.y = 0.f;
    } else if (prevTileY == TILE_FLOOR && tileY == TILE_HIGH_FLOOR && z < 15.f) {
        position.y = oldY;
        velocity.y = 0.f;
        if (z == 0.f) vz = 12.5f;
    }

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

    // --- ANIMACIONES DEL ENEMIGO ---
    int frameWidth = 28;
    int frameHeight = 36;
    int startX = 0;
    int rowY = 0;
    int numFrames = 1;

    if (isKicking) {
        rowY = 36;     // Fila de patada
        startX = 84;   // Coordenada horizontal de la patada
        numFrames = 4;
    } else if (isAttacking) {
        rowY = 36;     // Fila de puñetazo
        startX = 0;
        numFrames = 3;
    } else if (z > 0.f) {
        rowY = 0;
        startX = 112;
        numFrames = 1;
    } else if (isRunning && (std::abs(velocity.x) > 0.5f || std::abs(velocity.y) > 0.5f)) {
        rowY = 0;
        startX = 140;
        numFrames = 2;
    } else if (std::abs(velocity.x) > 0.5f || std::abs(velocity.y) > 0.5f) {
        rowY = 0;
        startX = 0;
        numFrames = 4;
    } else {
        rowY = 0;
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