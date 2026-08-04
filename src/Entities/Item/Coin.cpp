#include "Coin.h"

Coin::Coin(float x, float y) : Item(x, y), bounceTimer(0.f), m_alreadyCollected(false) {
    if (texture.loadFromFile("assets/textures/Coin.png")) {
        sprite.setTexture(texture);
    }
    
    std::vector<sf::IntRect> coinFrames;
    for (int i = 0; i < 4; ++i) {
        coinFrames.push_back(sf::IntRect(i * 9 + 1, 1, 8, 14));
    }
    animator.addAnimation("Coin", Animation(coinFrames, 0.1f));
    animator.playAnimation("Coin", 0.f);

    hitbox.setSize(sf::Vector2f(8.f, 14.f));
    hitbox.setOrigin(4.f, 14.f); // Bottom-Center
}

void Coin::spawn(){
    isActive = true;
    velocity.y = -300.f; // bounce up
    bounceTimer = 0.5f; // time before disappearing
}

std::unique_ptr<Item> Coin::clone(Hero* hero) const {
    auto newCoin = std::make_unique<Coin>(position.x, position.y);
    // Auto-collect: award coin immediately when block is hit
    hero->collectCoin(); // direct call — avoids const violation (clone() is const)
    newCoin->m_alreadyCollected = true; // prevent double-count if CollisionSystem later detects overlap
    return newCoin;
}

void Coin::update(float deltatime){
    if (!isActive) return;

    bounceTimer -= deltatime;
    if (bounceTimer <= 0.f){
        isActive = false;
        return;
    }

    // Apply gravity on the bounce visual //temporary
    velocity.y += 1000.f * deltatime;
    position.y += velocity.y * deltatime;
    sprite.setPosition(position);
    hitbox.setPosition(position);
    
    animator.playAnimation("Coin", deltatime);
}

void Coin::render(sf::RenderWindow& window){
    if (isActive) window.draw(sprite);
}

void Coin::getCollected(Hero* hero){
    if (m_alreadyCollected) {
        // This coin was auto-collected when block was hit → only deactivate visual
        isActive = false;
        return;
    }
    hero->collectCoin();
    isActive = false;
}