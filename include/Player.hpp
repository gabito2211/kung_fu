#pragma once
#include <SFML/Graphics.hpp>
#include "Tilemap.hpp"

class Player {
public:
    Player();
    void handleInput();
    void update(const Tilemap& tilemap);
    void draw(sf::RenderWindow& window);

    float getDepthY() const { return position.y; }
    sf::FloatRect getBounds() const;
    bool hasFallen() const { return fallenToVoid; }
    sf::Vector2f getPosition() const { return position; }

    bool getIsAttacking() const { return isAttacking; }
    bool getIsKicking() const { return isKicking; }
    sf::FloatRect getAttackBox() const;
    sf::FloatRect getKickBox() const;

    bool getIsHurt() const { return isHurt; }
    void takeHit(float attackerX);

    int getHealth() const { return health; }
    bool isDead() const { return health <= 0; }
    void takeHit(float attackerX, int damage);

    bool getIsUppercut() const { return isUppercut; }
    bool getIsHighKick() const { return isHighKick; }
    sf::FloatRect getUppercutBox() const;
    sf::FloatRect getHighKickBox() const;

    bool isStunned() const { return stunned; }
    void applyStun();

    void takeHit(float attackerX, int damage, bool isFinisher);

private:

    int health;

    bool isHurt;
    sf::Clock hurtClock;

    sf::Vector2f position;
    sf::Vector2f velocity;
    float z;
    float vz;

    sf::Vector2f acceleration;
    float walkMaxSpeed;
    float runMaxSpeed;
    float maxSpeed;
    float accRate;
    float friction;

    sf::Clock doubleTapClock;
    int lastDirection;
    bool wasKeyReleased;
    bool isRunning;
    bool prevLeftPressed;
    bool prevRightPressed;

    bool isFallingIntoVoid;
    bool fallenToVoid;

    // Control de combate (Puñetazo con Z y Patada con B)
    bool isAttacking;
    bool isKicking;
    sf::Clock attackClock;
    bool prevZPressed;
    bool prevXPressed;

    int consecutivePunches = 0;
    int consecutiveKicks = 0;
    bool isUppercut = false;
    bool isHighKick = false;
    
    bool stunned = false;
    sf::Clock stunClock;

    sf::Texture texture;
    sf::Sprite sprite;
    
    sf::Clock animationClock;
    int currentFrame;
    float frameTime;
    int facingDirection;
};