#pragma once
#include <SFML/Graphics.hpp>
#include "Core/CollisionTypes.h"

class Character;

enum class ProjectileFaction {
    Hero,
    Enemy
};

class Projectile {
protected:
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::RectangleShape shape;
    bool isAlive;
    int damage;
    ProjectileFaction faction;

public:
    Projectile(float startX, float startY, float velX, float velY,
               ProjectileFaction owner, int dmg = 1);
    virtual ~Projectile() = default;

    // Updates projectile-owned state, including acceleration and phase timers.
    // The world physics system owns position integration and solid collision.
    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) = 0;

    virtual std::string getProjectileType() const = 0;

    virtual float getGravityAcceleration() const { return 0.0f; }
    virtual bool usesWorldPhysics() const { return true; }
    virtual void onSolidCollision(SideType side, const sf::FloatRect& solidBounds) = 0;

    // Returns true only when this contact actually applies damage. The scene
    // uses the result to award score without knowing the concrete projectile.
    virtual bool onHitTarget(Character& target);
    virtual void onTargetResolutionComplete() {}

    bool getIsAlive() const { return isAlive; }
    void die() { isAlive = false; }
    sf::FloatRect getBounds() const { return shape.getGlobalBounds(); }
    int getDamage() const { return damage; }
    ProjectileFaction getFaction() const { return faction; }
    void setFaction(ProjectileFaction owner) { faction = owner; }

    sf::Vector2f getPosition() const { return position; }
    virtual void setPosition(const sf::Vector2f& pos) { position = pos; shape.setPosition(pos); }
    sf::Vector2f getVelocity() const { return velocity; }
    void setVelocity(const sf::Vector2f& vel) { velocity = vel; }
    void setVelocity(float vx, float vy) { velocity.x = vx; velocity.y = vy; }
    sf::RectangleShape& getHitbox() { return shape; }
};
