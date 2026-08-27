#include "Entities/Projectile/MarioFireball.h"
#include "Entities/Character/Character.h"

MarioFireball::MarioFireball(float startX, float startY, float velocityX)
    : Projectile(startX, startY, velocityX, 0.0f, ProjectileFaction::Hero, 1),
      animator(sprite), lifetime(4.0f), explosionTimer(0.0f),
      isExploding(false) {
    shape.setSize({16.0f, 16.0f});
    shape.setFillColor(sf::Color(255, 110, 0));

    if (texture.loadFromFile("assets/textures/MarioFireball.png")) {
        texture.setSmooth(false);
        sprite.setTexture(texture);
        animator.addAnimation("Fireball", Animation({
            sf::IntRect(0, 0, 32, 32),
            sf::IntRect(32, 0, 32, 32),
            sf::IntRect(64, 0, 32, 32),
            sf::IntRect(96, 0, 32, 32)
        }, 0.08f));
        animator.addAnimation("Explosion", Animation({
            sf::IntRect(128, 0, 32, 32),
            sf::IntRect(160, 0, 32, 32),
            sf::IntRect(192, 0, 32, 32),
            sf::IntRect(224, 0, 32, 32)
        }, 0.06f));
        animator.playAnimation("Fireball", 0.0f);
    }

    setPosition({startX, startY});
}

void MarioFireball::update(float deltaTime) {
    if (!isAlive) return;

    if (isExploding) {
        explosionTimer -= deltaTime;
        if (explosionTimer <= 0.0f) {
            die();
            return;
        }
        animator.playAnimation("Explosion", deltaTime);
        setPosition(position);
        return;
    }

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
        enterExplosion();
    }
}

bool MarioFireball::onHitTarget(Character& target) {
    if (!isAlive || isExploding || !target.getIsAlive()) return false;
    target.takeDamage(damage);
    enterExplosion();
    return true;
}

void MarioFireball::enterExplosion() {
    if (isExploding) return;
    isExploding = true;
    explosionTimer = 0.24f;
    velocity = {0.0f, 0.0f};
    animator.playAnimation("Explosion", 0.0f);
}

void MarioFireball::setPosition(const sf::Vector2f& pos) {
    Projectile::setPosition(pos);
    sprite.setPosition(position.x + shape.getSize().x * 0.5f,
                       position.y + shape.getSize().y * 0.5f + 16.0f);
}
