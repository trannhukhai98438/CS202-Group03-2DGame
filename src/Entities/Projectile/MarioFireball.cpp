#include "Entities/Projectile/MarioFireball.h"

MarioFireball::MarioFireball(float startX, float startY, float velocityX)
    : Projectile(startX, startY, velocityX, 0.0f, ProjectileFaction::Hero, 1),
      animator(sprite), lifetime(4.0f) {
    shape.setSize({16.0f, 16.0f});
    shape.setFillColor(sf::Color(255, 110, 0));

    sf::Image image;
    if (image.loadFromFile("assets/textures/FireMario.png")) {
        // FireMario.png uses this blue as its sprite-sheet background.
        image.createMaskFromColor(sf::Color(0, 41, 140));
        texture.loadFromImage(image);
        texture.setSmooth(false);
        sprite.setTexture(texture);
        animator.addAnimation("Fireball", Animation({
            sf::IntRect(172, 72, 16, 32),
            sf::IntRect(190, 72, 16, 32),
            sf::IntRect(208, 72, 16, 32)
        }, 0.08f));
        animator.playAnimation("Fireball", 0.0f);
    }

    setPosition({startX, startY});
}

void MarioFireball::update(float deltaTime) {
    if (!isAlive) return;
    lifetime -= deltaTime;
    if (lifetime <= 0.0f) {
        die();
        return;
    }
    animator.playAnimation("Fireball", deltaTime);
    setPosition(position);
}

void MarioFireball::render(sf::RenderWindow& window) {
    if (!isAlive) return;
    if (sprite.getTexture()) window.draw(sprite);
    else window.draw(shape);
}

void MarioFireball::onSolidCollision(SideType side, const sf::FloatRect&) {
    if (side == SideType::Top) {
        velocity.y = -300.0f;
    } else {
        die();
    }
}

void MarioFireball::setPosition(const sf::Vector2f& pos) {
    Projectile::setPosition(pos);
    sprite.setPosition(position.x + shape.getSize().x * 0.5f,
                       position.y + shape.getSize().y);
}
