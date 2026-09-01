#include "Entities/Projectile/MarioFireball.h"

MarioFireball::MarioFireball(float startX, float startY, float velocityX)
    : Projectile(startX, startY, velocityX, 0.0f, ProjectileFaction::Hero, 1),
      animator(sprite), lifetime(4.0f) {
    shape.setSize({18.0f, 18.0f});
    shape.setFillColor(sf::Color(255, 110, 0));

    if (texture.loadFromFile("assets/textures/FireMario.png")) {
        texture.setSmooth(true);
        sprite.setTexture(texture);
        sprite.setScale(0.24f, 0.24f);
        animator.addAnimation("Fireball", Animation({
            sf::IntRect(250, 446, 91, 75),
            sf::IntRect(379, 438, 75, 90),
            sf::IntRect(491, 442, 91, 79)
        }, 0.07f));
        animator.playAnimation("Fireball", 0.0f);
    }

    setPosition({startX, startY});
}

void MarioFireball::update(float deltaTime) {
    if (!isAlive) return;
    velocity.y += getGravityAcceleration() * deltaTime;
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
