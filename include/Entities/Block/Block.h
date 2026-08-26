#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "Entities/Item/Item.h"
#include "Animator.h"

class Block {
public:
    Block(float x, float y);
    virtual ~Block() = default;

    virtual void update(float deltatime) = 0;
    virtual void render(sf::RenderWindow& window) = 0;

    // Called when Hero hits the block from below
    // Returns the spawned Item if any, otherwise nullptr
    virtual std::unique_ptr<Item> hit(Hero* hero) = 0;

    void setItemPrototype(std::unique_ptr<Item> itemProto);
    sf::FloatRect getBounds() const;
    sf::RectangleShape& getHitbox();

    // Scene checks this to know when to remove block from the world.
    // Analogous to !isActive in Item. Default: never remove (QuestionBlock stays forever).
    virtual bool getIsActive() const { return isActive; }

    // Collision capabilities are separate from lifetime. A destroyed block can
    // remain active while its particles finish, without remaining solid.
    virtual bool isSolid() const { return true; }
    virtual bool canBeHitFromBelow() const { return isSolid(); }

protected:
    sf::Vector2f position;
    sf::Sprite sprite;
    sf::Texture texture;
    Animator animator;
    sf::RectangleShape hitbox;
    std::unique_ptr<Item> hiddenItemPrototype;
    bool isHit;
    bool isActive; // false → scene removes this block
};
