#include "Flower.h"
#include "Hero.h"

Flower::Flower(float x, float y) : Item(x, y), isSpawning(false), spawnStartY(y) {
    if (texture.loadFromFile("assets/textures/Flower.png")) {
        sprite.setTexture(texture);
    }
    
    std::vector<sf::IntRect> flowerFrames;
    for (int i = 0; i < 4; ++i) {
        flowerFrames.push_back(sf::IntRect(i * 17 + 1, 0, 16, 16));
    }
    animator.addAnimation("Flower", Animation(flowerFrames, 0.15f));
    animator.playAnimation("Flower", 0.f);

    hitbox.setSize(sf::Vector2f(32.f, 32.f));
    hitbox.setFillColor(sf::Color::Red);
    sprite.setScale(2.f, 2.f);
    setPosition(x,y);
}

void Flower::spawn() {
    isActive = true;
    isSpawning = true;
    spawnStartY = position.y;
    velocity.x=0;
    velocity.y = -50.f; // slowly move up
}

std::unique_ptr<Item> Flower::clone(Hero* hero) const {
    return std::make_unique<Flower>(position.x, position.y);
}

void Flower::update(float deltatime) {
    if (!isActive) return;

    if (isSpawning) {
        // Position update is handled by PlayingState
        // Assume block size is 32, stop spawning when moved up by 32 pixels
        if (spawnStartY - position.y >= 32.f) {
            position.y = spawnStartY - 32.f;
            isSpawning = false;
            velocity.y = 0.f;
        }
    }


    animator.playAnimation("Flower", deltatime);
    // Note: playAnimation() updates sprite texture rect internally via sf::Sprite& reference
}

void Flower::render(sf::RenderWindow& window) {
    if (isActive) window.draw(sprite);
}

void Flower::getCollected(Hero* hero) {
    if (!isActive || isSpawning) return;

    // The special form requires the Mushroom upgrade first. In particular,
    // Flash must never enter the glitch-only Small Thunder row.
    const std::string form = hero->getFormName();
    if (form == "Small" || form.empty()) return;

    if (form == "Giant") {
        // Do not replace a transformation animation with coordinates from a
        // different sheet. The flower remains available after it finishes.
        if (hero->getStateName() == "Shrink"
            || hero->getStateName() == "Grow") {
            return;
        }
        hero->setForm(std::make_unique<FireForm>());
    }
    isActive = false;
}

bool Flower::isColliable(){
    return !isSpawning;
}