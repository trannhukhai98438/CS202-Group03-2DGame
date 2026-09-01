#include "Coin.h"
#include "Hero.h"

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

    hitbox.setSize(sf::Vector2f(16.f, 28.f));
    hitbox.setFillColor(sf::Color::Yellow);
    sprite.setScale(2.f, 2.f);
    sprite.setOrigin(4.f, 14.f); // Base coin frame is 8x14
    hitbox.setPosition(position);
    sprite.setPosition(position.x + 8.f, position.y + 28.f);
}

void Coin::spawn(){
    isActive = true;
    velocity.y = -300.f; // bounce up
    bounceTimer = 0.6f;  // time before disappearing
}

// Activates the coin in-place without a bounce animation.
// Used for coins placed directly on the map (floating/ground coins).
// bounceTimer = -1 is a sentinel: update() skips the auto-deactivate timer.
void Coin::spawnAsGroundCoin() {
    isActive    = true;
    velocity    = { 0.f, 0.f };
    bounceTimer = -1.f; // sentinel: never auto-deactivate
    // Sync sprite to current position (Item::setPosition handles hitbox + sprite)
    setPosition(position.x, position.y);
}

std::unique_ptr<Item> Coin::clone(Hero* hero) const {
    auto newCoin = std::make_unique<Coin>(position.x, position.y);
    // Auto-collect: award coin immediately when block is hit
    hero->collectCoin(); // direct call — avoids const violation (clone() is const)
    newCoin->m_alreadyCollected = true; // prevent double-count if CollisionSystem later detects overlap
    return newCoin;
}

std::string Coin::getItemType() const {
    return "Coin";
}

void Coin::update(float deltatime){
    if (!isActive) return;

    // Ground coins (sentinel bounceTimer < 0): only animate, never auto-deactivate
    if (bounceTimer < 0.f) {
        animator.playAnimation("Coin", deltatime);
        return;
    }

    bounceTimer -= deltatime;
    if (bounceTimer <= 0.f){
        isActive = false;
        return;
    }

    // Apply gravity on the bounce visual (temporary)
    velocity.y += 1000.f * deltatime;
    // position update is handled by PlayingState

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

bool Coin::isColliable(){
    return false;
}
