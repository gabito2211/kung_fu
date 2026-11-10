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

// --- Agregar dentro de handleInput() ---
void Player::handleInput() {
    if (isFallingIntoVoid) return;

    acceleration = sf::Vector2f(0.f, 0.f);

    bool currentLeft = sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
    bool currentRight = sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right);

    // Detección de la tecla Z (Puñetazo)
    bool currentZ = sf::Keyboard::isKeyPressed(sf::Keyboard::Z);
    if (currentZ && !prevZPressed && !isAttacking && !isKicking && z == 0.f) {
        isAttacking = true;
        attackClock.restart();
        currentFrame = 0;
    }
    prevZPressed = currentZ;

    // Detección de la tecla X (Patada)
    bool currentX = sf::Keyboard::isKeyPressed(sf::Keyboard::X);
    if (currentX && !prevXPressed && !isAttacking && !isKicking && z == 0.f) {
        isKicking = true;
        attackClock.restart();
        currentFrame = 0;
    }
    prevXPressed = currentX;

    // Si está atacando o pateando, bloqueamos el movimiento
    if (isAttacking || isKicking) {
        acceleration.x = 0.f;
    } else {
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
    }

    if (!currentLeft && !currentRight && std::abs(velocity.x) < 0.5f) isRunning = false;

    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Keyboard::isKeyPressed(sf::Keyboard::K)) && z == 0.f && !isAttacking && !isKicking) {
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

    // Control de duración de ataques (puñetazo o patada)
    if (isAttacking || isKicking) {
        if (attackClock.getElapsedTime().asMilliseconds() > 350) {
            isAttacking = false;
            isKicking = false;
        }
    }

    // --- LÓGICA DE SELECCIÓN DE ANIMACIÓN ---
    int frameWidth = 28;
    int frameHeight = 36;
    int startX = 0;
    int rowY = 0;
    int numFrames = 1;

    if (isKicking) {
        rowY = 36;     // Fila vertical donde se encuentra la patada en el spritesheet
        startX = 84;  // Coordenada X inicial de los fotogramas de patada
        numFrames = 4; // Número de fotogramas de la patada
    }
    else if (isAttacking) {
        // Animación de Golpe (Ajusta rowY y startX según tu hoja de sprites)
        rowY = 36;     // Fila vertical donde se encuentran los golpes en el spritesheet
        startX = 0;    // Coordenada X inicial de los fotogramas de golpe
        numFrames = 3; // Cantidad de fotogramas que dura el golpe
    }
    else if (z > 0.f) {
        rowY = 0; 
        startX = 112; 
        numFrames = 1;
    } 
    else if (isRunning && (std::abs(velocity.x) > 0.5f || std::abs(velocity.y) > 0.5f)) {
        rowY = 0;      
        startX = 140;  
        numFrames = 2; 
    } 
    else if (std::abs(velocity.x) > 0.5f || std::abs(velocity.y) > 0.5f) {
        rowY = 0; 
        startX = 0;    
        numFrames = 4; 
    } 
    else {
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

sf::FloatRect Player::getAttackBox() const {
    float attackWidth = 25.f;
    float attackHeight = 20.f;
    // Si mira a la derecha, la caja se genera adelante a la derecha; si mira a la izquierda, a la izquierda
    float attackX = (facingDirection == 1) ? position.x + 5.f : position.x - 5.f - attackWidth;
    float attackY = position.y - 30.f;
    return sf::FloatRect(attackX, attackY, attackWidth, attackHeight);
}

sf::FloatRect Player::getKickBox() const {
    float kickWidth = 30.f;
    float kickHeight = 20.f;
    float kickX = (facingDirection == 1) ? position.x + 5.f : position.x - 5.f - kickWidth;
    float kickY = position.y - 25.f;
    return sf::FloatRect(kickX, kickY, kickWidth, kickHeight);
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}