#include "Entities/Potion.h"
#include "Entities/Character.h"

Potion::Potion(float startX, float startY, float velX, float velY)
    : Projectile(startX, startY, velX, velY, 1), animator(sprite), isPuddle(false), puddleTimer(0.0f) {
    
    shape.setSize(sf::Vector2f(32.0f, 32.0f));
    shape.setPosition(position);
    shape.setFillColor(sf::Color::Green); // fallback

    sf::Image img;
    if (img.loadFromFile("assets/textures/potion.png")) {
        for (unsigned int y = 0; y < img.getSize().y; ++y) {
            for (unsigned int x = 0; x < img.getSize().x; ++x) {
                sf::Color c = img.getPixel(x, y);
                if (c.r > 230 && c.g > 230 && c.b > 230) {
                    img.setPixel(x, y, sf::Color::Transparent);
                }
            }
        }
        texture.loadFromImage(img);
        sprite.setTexture(texture);
        
        int frameWidth = texture.getSize().x / 2;
        int frameHeight = texture.getSize().y;
        
        animator.addAnimation("flying", Animation({
            sf::IntRect(0, 0, frameWidth, frameHeight)
        }, 1.0f));
        
        animator.addAnimation("puddle", Animation({
            sf::IntRect(frameWidth, 0, frameWidth, frameHeight)
        }, 1.0f));

        float scale = 32.0f / static_cast<float>(frameHeight);
        sprite.setScale(scale, scale);
        
        animator.playAnimation("flying", 0.0f);
    }
}

void Potion::shatter() {
    isPuddle = true;
    puddleTimer = 1.5f; // Puddle lasts 1.5 seconds
    velocity = sf::Vector2f(0.0f, 0.0f);
    shape.setSize(sf::Vector2f(64.0f, 16.0f)); // Puddle is wider and flatter
    
    // Position puddle exactly on ground
    position.y = groundY - shape.getSize().y;
    position.x -= 16.0f; // Center the wider puddle
    shape.setPosition(position);
    
    animator.playAnimation("puddle", 0.0f);
}

void Potion::update(float deltaTime) {
    if (!isAlive) return;

    if (!isPuddle) {
        velocity.y += gravity * deltaTime;
        position += velocity * deltaTime;
        shape.setPosition(position);
        
        // Align sprite bottom-center to shape bottom-center
        sprite.setPosition(position.x + shape.getSize().x / 2.0f, position.y + shape.getSize().y);

        if (position.y + shape.getSize().y >= groundY) {
            shatter();
        }
    } else {
        puddleTimer -= deltaTime;
        if (puddleTimer <= 0.0f) {
            die();
        }
        // Update puddle sprite position (bottom-center alignment)
        sprite.setPosition(position.x + shape.getSize().x / 2.0f, position.y + shape.getSize().y);
    }
    
    animator.playAnimation(isPuddle ? "puddle" : "flying", deltaTime);
}

void Potion::render(sf::RenderWindow& window) {
    if (!isAlive) return;
    if (sprite.getTexture() != nullptr) {
        window.draw(sprite);
    } else {
        window.draw(shape);
    }
}

void Potion::onHitPlayer(Character* player) {
    if (player && isAlive) {
        player->takeDamage(damage);
        die();
    }
}
