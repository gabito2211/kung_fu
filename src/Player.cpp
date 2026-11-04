#include "Player.hpp"
#include "Constants.hpp"
#include <cmath>
#include <iostream>

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

    currentFrame = 0;
    frameTime = 0.12f; // Tiempo de duración de cada fotograma en segundos
    facingDirection = 1; // Mirando por defecto a la derecha

    if (!texture.loadFromFile("assets/sprites/ryan.png")) {
        std::cerr << "Error al cargar la textura de Alex desde assets/sprites/alex.png\n";
    }

    sprite.setTexture(texture);
    
    // Fotograma inicial por defecto (Ancho de frame aprox. 18px, Alto 32px según el sheet de NES)
    sprite.setTextureRect(sf::IntRect(0, 0, 18, 32)); 
    
    // Centrar el origen horizontalmente y ponerlo en los pies verticalmente
    sprite.setOrigin(12.f, 32.f);
    sprite.scale(2.5f, 2.5f);
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

    if (currentLeft) {
        acceleration.x = -accRate;
        facingDirection = -1;
    }
    if (currentRight) {
        acceleration.x = accRate;
        facingDirection = 1;
    }

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
        sprite.setPosition(position.x, position.y - z);
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

    float oldX = position.x;
    float oldY = position.y;

    position.x += velocity.x;
    TileType tileX = tilemap.getTileAtPixel(position.x, position.y);
    TileType prevTileX = tilemap.getTileAtPixel(oldX, position.y);

    if (tileX == TILE_BOX) {
        position.x = oldX;
        velocity.x = 0.f;
        isRunning = false;
    } else if (prevTileX == TILE_FLOOR && tileX == TILE_HIGH_FLOOR && z < 15.f) {
        position.x = oldX;
        velocity.x = 0.f;
        isRunning = false;
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

    // --- LÓGICA DE SELECCIÓN DE ANIMACIÓN ---
    int frameWidth = 28;
    int frameHeight = 36;
    int startX = 0;
    int rowY = 0;
    int numFrames = 1;

    if (z > 0.f) {
        // Animación de Salto
        rowY = 0; 
        startX = 112; // Coordenada X del sprite de salto en tu sheet
        numFrames = 1;
    } 
    else if (isRunning && (std::abs(velocity.x) > 0.5f || std::abs(velocity.y) > 0.5f)) {
        // --- ANIMACIÓN DE CORRER ---
        rowY = 0;      // Fila correspondiente a correr (o cámbiala si está en otra fila vertical)
        startX = 140;  // Ajusta este valor X según dónde comiencen los sprites de correr en tu imagen
        numFrames = 2; // Cantidad de fotogramas que tiene la secuencia de correr
    } 
    else if (std::abs(velocity.x) > 0.5f || std::abs(velocity.y) > 0.5f) {
        // --- ANIMACIÓN DE CAMINAR ---
        rowY = 0; 
        startX = 0;    // Inicio horizontal de los fotogramas de caminar
        numFrames = 4; // Cantidad de fotogramas de caminar
    } 
    else {
        // Estado de Reposo (Idle)
        rowY = 0;
        startX = 0;
        numFrames = 1;
    }

    // Avanzar de fotograma según el tiempo transcurrido
    if (animationClock.getElapsedTime().asSeconds() > frameTime) {
        currentFrame = (currentFrame + 1) % numFrames;
        animationClock.restart();
    }

    // Asignar el rectángulo del fotograma actual en la textura
    sprite.setTextureRect(sf::IntRect(startX + (currentFrame * frameWidth), rowY, frameWidth, frameHeight));
    
    // Aplicar la dirección en la escala horizontal para voltear el sprite si va a la izquierda
    sprite.setScale(facingDirection * 2.5f, 2.5f);

    sprite.setPosition(position.x, position.y - z);
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}