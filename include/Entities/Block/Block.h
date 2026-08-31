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

    void setItemPrototype(std::unique_ptr<Item> itemProto, int itemCount = 1);
    sf::FloatRect getBounds() const;
    sf::RectangleShape& getHitbox();

    // Scene checks this to know when to remove block from the world.
    // Analogous to !isActive in Item. Default: never remove (QuestionBlock stays forever).
    virtual bool getIsActive() const { return isActive; }

    // Collision capabilities are separate from lifetime. A destroyed block can
    // remain active while its particles finish, without remaining solid.
    virtual bool isSolid() const { return true; }
    virtual bool canBeHitFromBelow() const { return isSolid(); }

    // Returns the current velocity of this block.
    // Stationary blocks return {0, 0}; moving blocks (e.g. Lifter) override this
    // so the physics system can carry riders at the platform's speed.
    virtual sf::Vector2f getVelocity() const { return {0.f, 0.f}; }

    bool getIsHit() const { return isHit; }
    void setIsHit(bool hit) { isHit = hit; }

protected:
    bool hasHiddenItems() const;
    std::unique_ptr<Item> releaseHiddenItem(Hero* hero);

    sf::Vector2f position;
    sf::Sprite sprite;
    sf::Texture texture;
    Animator animator;
    sf::RectangleShape hitbox;
    bool isHit;
    bool isActive; // false → scene removes this block

private:
    std::unique_ptr<Item> hiddenItemPrototype;
    int hiddenItemCount{0};
};
