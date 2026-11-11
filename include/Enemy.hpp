#pragma once
#include <SFML/Graphics.hpp>
#include "Tilemap.hpp"
#include "Player.hpp"

class Enemy {
public:
    Enemy(float startX, float startY);
    void update(const Tilemap& tilemap, const Player& player);
    void draw(sf::RenderWindow& window);

    sf::FloatRect getBounds() const;
    bool hasFallen() const { return fallenToVoid; }
    
    bool getIsAttacking() const { return isAttacking; }
    bool getIsKicking() const { return isKicking; }
    bool getIsUppercut() const { return isUppercut; }
    bool getIsHighKick() const { return isHighKick; }

    sf::FloatRect getAttackBox() const;
    sf::FloatRect getKickBox() const;
    sf::FloatRect getUppercutBox() const;
    sf::FloatRect getHighKickBox() const;

    bool getIsHurt() const { return isHurt; }
    bool isStunned() const { return stunned; }
    sf::Vector2f getPosition() const { return position; }

    int getHealth() const { return health; }
    bool isDead() const { return health <= 0; }
    void takeHit(float attackerX, int damage, bool isFinisher);

private:
    int health;

    bool isHurt;
    sf::Clock hurtClock;

    bool stunned = false;
    sf::Clock stunClock;

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

    bool isFallingIntoVoid;
    bool fallenToVoid;

    // Control de IA para combos y ataques
    bool isRunning;
    bool isAttacking;
    bool isKicking;
    bool isUppercut;
    bool isHighKick;
    int consecutivePunches = 0;
    int consecutiveKicks = 0;
    sf::Clock attackCooldownClock;

    sf::Texture texture;
    sf::Sprite sprite;
    sf::Clock animationClock;
    int currentFrame;
    float frameTime;
    int facingDirection;
};