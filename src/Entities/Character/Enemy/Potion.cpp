#include "Entities/Character/Enemy/Potion.h"
#include "Entities/BaseEntity.h"

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
        
        animator.addAnimation("flying", Animation({
            sf::IntRect(0, 0, texture.getSize().x, texture.getSize().y)
        }, 1.0f));
    }

    sf::Image puddleImg;
    if (puddleImg.loadFromFile("assets/textures/puddle.png")) {
        for (unsigned int y = 0; y < puddleImg.getSize().y; ++y) {
            for (unsigned int x = 0; x < puddleImg.getSize().x; ++x) {
                sf::Color c = puddleImg.getPixel(x, y);
                if (c.r > 230 && c.g > 230 && c.b > 230) {
                    puddleImg.setPixel(x, y, sf::Color::Transparent);
                }
            }
        }
        puddleTexture.loadFromImage(puddleImg);
        
        animator.addAnimation("puddle", Animation({
            sf::IntRect(0, 0, puddleTexture.getSize().x, puddleTexture.getSize().y)
        }, 1.0f));
    }

    float scale = 32.0f / static_cast<float>(texture.getSize().y > 0 ? texture.getSize().y : 32.0f);
    sprite.setScale(scale, scale);
    
    animator.playAnimation("flying", 0.0f);
}

void Potion::shatter() {
    isPuddle = true;
    puddleTimer = 1.5f; // Puddle lasts 1.5 seconds
    velocity = sf::Vector2f(0.0f, 0.0f);
    shape.setSize(sf::Vector2f(80.0f, 24.0f)); // Puddle is wider and flatter
    
    // Position puddle exactly on ground
    position.y = groundY - shape.getSize().y;
    position.x -= 24.0f; // Center the wider puddle
    shape.setPosition(position);
    
    // Change texture and scale for puddle
    sprite.setTexture(puddleTexture);
    float scaleX = 80.0f / static_cast<float>(puddleTexture.getSize().x > 0 ? puddleTexture.getSize().x : 80.0f);
    float scaleY = 24.0f / static_cast<float>(puddleTexture.getSize().y > 0 ? puddleTexture.getSize().y : 24.0f);
    sprite.setScale(scaleX, scaleY);

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
        // Keep shape in sync for collision detection
        shape.setPosition(position);
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

void Potion::onHitPlayer(BaseEntity* player) {
    if (player && isAlive) {
        player->takeDamage(damage);
        die();
    }
}
