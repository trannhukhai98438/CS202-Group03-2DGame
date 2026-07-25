#ifndef CHARACTER_H
#define CHARACTER_H

#include <SFML/Graphics.hpp>
#include <string>

class Character {
protected:
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::RectangleShape shape;
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

    virtual void interactWith(Character* other) = 0;
    virtual void onStomped(Character* attacker) { (void)attacker; die(); }
    virtual void onSideCollision(Character* attacker) { (void)attacker; }

    sf::Vector2f getPosition() const;
    void setPosition(const sf::Vector2f& pos);
    sf::FloatRect getBounds() const;
    bool getIsAlive() const;
    bool isDead() const;
};

#endif
