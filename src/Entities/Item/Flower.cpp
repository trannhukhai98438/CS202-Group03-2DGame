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

    hitbox.setSize(sf::Vector2f(16.f, 16.f));
    hitbox.setFillColor(sf::Color::Red);
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
        // Assume block size is 16, stop spawning when moved up by 16 pixels
        if (spawnStartY - position.y >= 16.f) {
            position.y = spawnStartY - 16.f;
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
    hero->setForm(std::make_unique<FireForm>());
    isActive = false;
}

bool Flower::isColliable(){
    return !isSpawning;
}