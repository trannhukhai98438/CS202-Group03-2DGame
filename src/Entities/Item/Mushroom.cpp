#include "Mushroom.h"
#include "GrowState.h"

Mushroom::Mushroom(float x, float y) : Item(x, y), isSpawning(false) {
    if (texture.loadFromFile("assets/textures/Mushroom.png")) {
        sprite.setTexture(texture);
    }
    
    std::vector<sf::IntRect> mushFrames;
    mushFrames.push_back(sf::IntRect(0, 0, 16, 16));
    animator.addAnimation("Mushroom", Animation(mushFrames, 1.0f));
    animator.playAnimation("Mushroom", 0.f);

    hitbox.setSize(sf::Vector2f(16.f, 16.f));
    hitbox.setOrigin(8.f, 16.f); // Bottom-Center
    isSpawning = false;
    spawnStartY = y;
}

void Mushroom::spawn() {
    isActive = true;
    isSpawning = true;
    spawnStartY = position.y;
    velocity.y = -50.f; // slowly move up
    velocity.x = 0.f;
}

std::unique_ptr<Item> Mushroom::clone(Hero* hero) const {
    return std::make_unique<Mushroom>(position.x, position.y);
}

void Mushroom::update(float deltatime) {
    if (!isActive) return;

    if (isSpawning) {
        position.y += velocity.y * deltatime;
        // Assume block size is 16, stop spawning when moved up by 16 pixels
        if (spawnStartY - position.y >= 16.f) {
            position.y = spawnStartY - 16.f;
            isSpawning = false;
            velocity.y = 0.f;
            velocity.x = 100.f; // start moving right
        }
    } else {
        // Normal movement
        velocity.y += 1000.f * deltatime; // gravity //temp
        position.x += velocity.x * deltatime;
        position.y += velocity.y * deltatime;
        // Collision with ground is usually handled by a physics class
    }

    sprite.setPosition(position);
    hitbox.setPosition(position);
    animator.playAnimation("Mushroom", deltatime);
}

void Mushroom::render(sf::RenderWindow& window) {
    if (isActive) window.draw(sprite);
}

void Mushroom::getCollision() {
    velocity.x = -velocity.x; // bounce off wall
}

void Mushroom::getCollected(Hero* hero) {
    if (!isActive || isSpawning) return;
    hero->setState(std::make_unique<GrowState>()); // Small→Giant with SmallGrow animation
    isActive = false;
}
