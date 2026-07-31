#pragma once
#include <SFML/Graphics.hpp>
#include <Animator.h>

class Character{
protected:
    sf::Sprite sprite;
    sf::Texture texture;
    Animator animator;
    sf::Vector2f velocity;
    sf::Vector2f position;
    sf::RectangleShape hitbox;
    bool isFacingRight;
    bool isGrounded;
    bool isAlive;
    int hp;
public:
    Character(float x, float y);
    virtual ~Character()=default;

    virtual void update(float deltatime)=0;
    virtual void render(sf::RenderWindow& window)=0;
    virtual void takedamage()=0;
    virtual void die()=0;

    // getters and setters
    bool isDead();
    sf::FloatRect getBounds();
    int getHp();
    void setSize(float x, float y);
    void setVelocity(float x, float y);
    sf::Vector2f getVelocity();
    void setPosition(float x, float y);
    sf::Vector2f getPosition();
    bool isGrounded();
    void setGrounded(bool grounded);
    void setFacingRight(bool facing);
    bool getFacingRight() const;
};