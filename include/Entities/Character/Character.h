#ifndef CHARACTER_H
#define CHARACTER_H

#include <SFML/Graphics.hpp>
#include "Utilities/Animator.h"
#include <string>

class Character {
protected:
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::RectangleShape shape;
    sf::Sprite sprite;
    sf::Texture texture;
    Animator animator;
    bool isAlive;
    bool facingRight;
    bool isGrounded;

public:
    Character(float x, float y);
    virtual ~Character() = default;

    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) = 0;

    virtual void die();
    virtual void takeDamage(int damage);

    virtual float getSpeed() const { return 0.0f; }
    virtual int getHealth() const { return 1; }
    virtual int getDamageOnTouch() const { return 1; }
    virtual int getScoreValue() const { return 100; }
    virtual std::string getCharacterType() const { return "Character"; }

    virtual int interactWith(Character* other) = 0;
    virtual void onStomped(Character* attacker) { (void)attacker; die(); }
    virtual void onSideCollision(Character* attacker) { (void)attacker; }

    sf::Vector2f getPosition() const;
    virtual void setPosition(const sf::Vector2f& pos);
    
    // Unified Physics & Logic getters/setters
    sf::Vector2f getVelocity() const { return velocity; }
    void setVelocity(const sf::Vector2f& vel) { velocity = vel; }
    void setVelocity(float vx, float vy) { velocity.x = vx; velocity.y = vy; }
    
    bool getGrounded() const { return isGrounded; }
    void setGrounded(bool grounded) { isGrounded = grounded; }
    
    bool getFacingRight() const { return facingRight; }
    void setFacingRight(bool right) { facingRight = right; }

    sf::FloatRect getBounds() const;
    sf::RectangleShape& getHitbox() { return shape; } // For PhysicsEngine compatibility

    bool getIsAlive() const;
    bool isDead() const;
    sf::Sprite& getSprite() { return sprite; }
    sf::RectangleShape& getShape() { return shape; }
    Animator& getAnimator() { return animator; }
};

#endif
