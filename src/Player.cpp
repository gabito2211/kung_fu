#include "Player.hpp"
#include "Constants.hpp"
#include <cmath>

Player::Player() {
    position = sf::Vector2f(150.f, 460.f);
    velocity = sf::Vector2f(0.f, 0.f);
    acceleration = sf::Vector2f(0.f, 0.f);
    z = 0.f;
    vz = 0.f;

    // Configuración de las físicas de movimiento
    maxSpeed = 4.5f;
    accRate = 0.4f;   // Qué tan rápido acelera
    friction = 0.82f; // Factor de fricción (entre 0 y 1; más cercano a 1 = más resbalue/inercia)

    // Configurar la figura temporal del jugador (ancho 40, alto 80)
    shape.setSize(sf::Vector2f(40.f, 80.f));
    shape.setFillColor(sf::Color::Cyan);
    shape.setOrigin(20.f, 80.f); // Origin en los pies para que el eje Y apunte al suelo
}

void Player::handleInput() {
    // Reiniciar aceleración en cada frame antes de leer el teclado
    acceleration = sf::Vector2f(0.f, 0.f);

    // Movimiento horizontal (X)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        acceleration.x = -accRate;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        acceleration.x = accRate;

    // Movimiento de profundidad (Y)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        acceleration.y = -accRate;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        acceleration.y = accRate;

    // Salto (Espacio o tecla K) solo si está en el suelo (z == 0)
    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Keyboard::isKeyPressed(sf::Keyboard::K)) && z == 0.f) {
        vz = 12.5f; // Impulso positivo hacia arriba
    }
}

void Player::update() {
    // 1. Aplicar aceleración a la velocidad
    velocity += acceleration;

    // Aplicar fricción si NO se está presionando ninguna tecla de movimiento en ese eje
    if (acceleration.x == 0.f) {
        velocity.x *= friction;
        // Detener por completo si la velocidad es muy cercana a cero para evitar micro-movimientos
        if (std::abs(velocity.x) < 0.05f) velocity.x = 0.f;
    }
    if (acceleration.y == 0.f) {
        velocity.y *= friction;
        if (std::abs(velocity.y) < 0.05f) velocity.y = 0.f;
    }

    // Limitar la velocidad máxima combinada (magnitud del vector de velocidad)
    float currentSpeedSq = (velocity.x * velocity.x) + (velocity.y * velocity.y);
    if (currentSpeedSq > (maxSpeed * maxSpeed)) {
        float currentSpeed = std::sqrt(currentSpeedSq);
        velocity.x = (velocity.x / currentSpeed) * maxSpeed;
        velocity.y = (velocity.y / currentSpeed) * maxSpeed;
    }

    // 2. Actualizar posición en el suelo (X, Y)
    position += velocity;

    // Restringir el movimiento Y dentro de los límites de profundidad del escenario
    if (position.y < FLOOR_Y_MIN) {
        position.y = FLOOR_Y_MIN;
        velocity.y = 0.f; // Freno seco al chocar con los límites
    }
    if (position.y > FLOOR_Y_MAX) {
        position.y = FLOOR_Y_MAX;
        velocity.y = 0.f;
    }

    // 3. Aplicar gravedad y física del salto (Eje Z)
    if (z > 0.f || vz != 0.f) {
        vz -= GRAVITY; // La gravedad reduce la velocidad hacia arriba
        z += vz;       // Actualiza la altura

        // Aterrizaje en el suelo
        if (z <= 0.f) {
            z = 0.f;
            vz = 0.f;
        }
    }

    // 4. Posición final en pantalla
    shape.setPosition(position.x, position.y - z);
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(shape);
}