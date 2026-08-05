#ifndef CHARACTER_H
#define CHARACTER_H

#include <SFML/Graphics.hpp>
#include "Utilities/Animator.h"
#include <string>

class BaseEntity {
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
    BaseEntity(float x, float y);
    virtual ~BaseEntity() = default;

    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) = 0;

    virtual void die();
    virtual void takeDamage(int damage);

    virtual float getSpeed() const { return 0.0f; }
    virtual int getHealth() const { return 1; }
    virtual int getDamageOnTouch() const { return 1; }
    virtual int getScoreValue() const { return 100; }
    virtual std::string getCharacterType() const { return "Character"; }

    virtual void interactWith(BaseEntity* other) = 0;
    virtual void onStomped(BaseEntity* attacker) { (void)attacker; die(); }
    virtual void onSideCollision(BaseEntity* attacker) { (void)attacker; }

    sf::Vector2f getPosition() const;
    virtual void setPosition(const sf::Vector2f& pos);
    sf::FloatRect getBounds() const;
    bool getIsAlive() const;
    bool isDead() const;
    sf::Sprite& getSprite() { return sprite; }
    sf::RectangleShape& getShape() { return shape; }
    Animator& getAnimator() { return animator; }
};

#endif
