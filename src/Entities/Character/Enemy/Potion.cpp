#include "Entities/Character/Enemy/Potion.h"
#include "Entities/Character/Character.h"

Potion::Potion(float startX, float startY, float velX, float velY)
    : Projectile(startX, startY, velX, velY, ProjectileFaction::Enemy, 1),
      animator(sprite), isPuddle(false), puddleTimer(0.0f) {
    
    shape.setSize(sf::Vector2f(32.0f, 32.0f));
    shape.setPosition(position);
    shape.setFillColor(sf::Color::Green); // fallback

    sf::Image img;
    if (img.loadFromFile("assets/textures/potion.png")) {
        for (unsigned int y = 0; y < img.getSize().y; ++y) {
            for (unsigned int x = 0; x < img.getSize().x; ++x) {
                sf::Color c = img.getPixel(x, y);
                if (c.r > 190 && c.g > 190 && c.b > 190) {
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
                if (c.r > 190 && c.g > 190 && c.b > 190) {
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

void Potion::shatterOnTile(float tileY) {
    if (isPuddle) return;
    isPuddle = true;
    puddleTimer = 1.5f; // Puddle lasts 1.5 seconds
    velocity = sf::Vector2f(0.0f, 0.0f);
    shape.setSize(sf::Vector2f(80.0f, 24.0f)); // Puddle is wider and flatter
    
    // Snap puddle exactly on TOP of the tile surface
    position.y = tileY - shape.getSize().y;
    position.x -= 24.0f; // Center the wider puddle
    shape.setPosition(position);
    // Switch to puddle texture
    sprite.setTexture(puddleTexture);
    float scaleX = 80.0f / static_cast<float>(puddleTexture.getSize().x > 0 ? puddleTexture.getSize().x : 80.0f);
    float scaleY = 24.0f / static_cast<float>(puddleTexture.getSize().y > 0 ? puddleTexture.getSize().y : 24.0f);
    sprite.setScale(scaleX, scaleY);

    animator.playAnimation("puddle", 0.0f);
    setPosition(position);
}

void Potion::shatterAtImpact(SideType side, const sf::FloatRect& solidBounds) {
    if (side == SideType::Top) {
        shatterOnTile(solidBounds.top);
        return;
    }

    sf::Vector2f impactCenter(position.x + shape.getSize().x * 0.5f,
                              position.y + shape.getSize().y * 0.5f);
    isPuddle = true;
    puddleTimer = 1.5f;
    velocity = {0.0f, 0.0f};
    shape.setSize({80.0f, 24.0f});
    sprite.setTexture(puddleTexture);
    float scaleX = 80.0f / static_cast<float>(puddleTexture.getSize().x > 0 ? puddleTexture.getSize().x : 80);
    float scaleY = 24.0f / static_cast<float>(puddleTexture.getSize().y > 0 ? puddleTexture.getSize().y : 24);
    sprite.setScale(scaleX, scaleY);
    animator.playAnimation("puddle", 0.0f);
    setPosition({impactCenter.x - 40.0f, impactCenter.y - 12.0f});
}

void Potion::onSolidCollision(SideType side, const sf::FloatRect& solidBounds) {
    if (!isPuddle) shatterAtImpact(side, solidBounds);
}

void Potion::update(float deltaTime) {
    if (!isAlive) return;

    velocity.y += getGravityAcceleration() * deltaTime;

    if (isPuddle) {
        puddleTimer -= deltaTime;
        if (puddleTimer <= 0.0f) {
            die();
        }
    }

    animator.playAnimation(isPuddle ? "puddle" : "flying", deltaTime);
    setPosition(position);
}

void Potion::render(sf::RenderWindow& window) {
    if (!isAlive) return;
    if (sprite.getTexture() != nullptr) {
        window.draw(sprite);
    } else {
        window.draw(shape);
    }
}

void Potion::setPosition(const sf::Vector2f& pos) {
    Projectile::setPosition(pos);
    sprite.setPosition(position.x + shape.getSize().x * 0.5f,
                       position.y + shape.getSize().y);
}
