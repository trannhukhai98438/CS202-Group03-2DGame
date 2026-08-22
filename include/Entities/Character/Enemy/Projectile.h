#pragma once
#include <SFML/Graphics.hpp>

class Character;

class Projectile {
protected:
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::RectangleShape shape;
    bool isAlive;
    int damage;

public:
    Projectile(float startX, float startY, float velX, float velY, int dmg = 1);
    virtual ~Projectile() = default;

    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    
    virtual void onHitPlayer(Character* player);

    bool getIsAlive() const { return isAlive; }
    void die() { isAlive = false; }
    sf::FloatRect getBounds() const { return shape.getGlobalBounds(); }
    int getDamage() const { return damage; }

    sf::Vector2f getPosition() const { return position; }
    virtual void setPosition(const sf::Vector2f& pos) { position = pos; shape.setPosition(pos); }
    sf::Vector2f getVelocity() const { return velocity; }
    void setVelocity(float vx, float vy) { velocity.x = vx; velocity.y = vy; }
    sf::RectangleShape& getHitbox() { return shape; }
    virtual bool shouldDieOnSolid() const { return false; }
};
