#include "Entities/Projectile/MarioFireball.h"
#include "Entities/Character/Character.h"

MarioFireball::MarioFireball(float startX, float startY, float velocityX)
    : Projectile(startX, startY, velocityX, 0.0f, ProjectileFaction::Hero, 1),
      animator(sprite), lifetime(4.0f), explosionTimer(0.0f),
      isExploding(false) {
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
        animator.addAnimation("Explosion", Animation({
            sf::IntRect(631, 417, 129, 127)
        }, 0.24f));
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
                       position.y + shape.getSize().y);
}
