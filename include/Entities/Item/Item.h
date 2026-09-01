#pragma once
#include <SFML/Graphics.hpp>
#include "Utilities/Animator.h"
#include <memory>

class Hero;

class Item{
public:
    Item(float x, float y);
    virtual ~Item()=default;

    virtual void update(float deltatime)=0;//usually for render or moving item
    virtual void render(sf::RenderWindow& window)=0;

    virtual void spawn()=0;
    
    virtual std::unique_ptr<Item> clone(Hero* hero) const = 0;

    virtual std::string getItemType() const = 0;

    bool isCollected() const;
    virtual void getCollected(Hero* hero)=0;
    void setActive(bool active) { isActive = active; }
    bool getIsActive() const { return isActive; }

    sf::FloatRect getBounds() const;
    sf::RectangleShape& getHitbox();
    virtual void getCollision(); //side collision with the brick/ pile -> reverse direction
    bool getIsGrounded() const;
    void setGrounded(bool grounded);
    sf::Vector2f getPosition() const;
    void setPosition(float x, float y);
    void setVelocity(float x, float y);
    sf::Vector2f getVelocity();
    virtual bool isColliable(){return false;};
protected:
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Sprite sprite;
    sf::Texture texture;
    Animator animator;
    sf::RectangleShape hitbox;
    bool isActive;
    bool isGrounded;
};