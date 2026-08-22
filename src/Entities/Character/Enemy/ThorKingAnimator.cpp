#include "Entities/Character/Enemy/ThorKingAnimator.h"
#include "Entities/Character/Enemy/ThorKing.h"
#include <cmath>

void ThorKingAnimator::initAnimations(ThorKing& boss) {
    boss.loadSpriteTexture("assets/textures/boss.png", 1, 111.f, 1);
    boss.setSpriteOffsetY(0.f);

    sf::Sprite& sprite = boss.getSprite();
    float scale = 128.f / 111.f;
    sprite.setScale(scale, scale);

    auto& animator = boss.getAnimator();
    int H = 111;

    // Row 0 (Walk): Y = 0
    animator.addAnimation("walk", Animation({
        sf::IntRect(27, 0, 180, H),
        sf::IntRect(284, 0, 180, H),
        sf::IntRect(547, 0, 180, H),
        sf::IntRect(795, 0, 180, H)
    }, 0.15f));

    // Row 1 (Crouch): Y = 115
    animator.addAnimation("crouch", Animation({
        sf::IntRect(42, 115, 160, H),
        sf::IntRect(298, 115, 160, H),
        sf::IntRect(554, 115, 160, H),
        sf::IntRect(811, 115, 160, H)
    }, 0.12f));

    // Row 2 (Roll): Y = 230
    animator.addAnimation("roll", Animation({
        sf::IntRect(69, 230, 120, H),
        sf::IntRect(326, 230, 120, H),
        sf::IntRect(581, 230, 120, H),
        sf::IntRect(833, 230, 120, H)
    }, 0.05f));

    // Row 3 (Stunned / Death): Y = 339
    animator.addAnimation("stunned", Animation({
        sf::IntRect(23, 339, 220, H),
        sf::IntRect(278, 339, 220, H),
        sf::IntRect(526, 339, 220, H),
        sf::IntRect(782, 339, 220, H)
    }, 0.2f));
    animator.addAnimation("death", Animation({
        sf::IntRect(23, 339, 220, H),
        sf::IntRect(278, 339, 220, H),
        sf::IntRect(526, 339, 220, H),
        sf::IntRect(782, 339, 220, H)
    }, 0.15f));

    // Row 4 (Fire/Roar): Y = 460
    animator.addAnimation("fire", Animation({
        sf::IntRect(30, 460, 160, H),
        sf::IntRect(289, 460, 160, H),
        sf::IntRect(526, 460, 160, H),
        sf::IntRect(526, 460, 160, H),
        sf::IntRect(526, 460, 160, H),
        sf::IntRect(526, 460, 160, H)
    }, 0.15f));
}

void ThorKingAnimator::applyAnimation(ThorKing& boss) {
    std::string state = boss.getStateName();
    sf::Sprite& sprite = boss.getSprite();
    float absScaleX = std::abs(sprite.getScale().x);
    float absScaleY = std::abs(sprite.getScale().y);

    // The image naturally faces RIGHT.
    // If getFacingRight() is true, we keep it positive.
    // If getFacingRight() is false, we must FLIP it (negative X scale).
    if (boss.getFacingRight()) {
        sprite.setScale(absScaleX, absScaleY);
    } else {
        sprite.setScale(-absScaleX, absScaleY);
    }

    if (state == "TKPatrol") {
        boss.getAnimator().playAnimation("walk", 0.016f);
    } else if (state == "TKCrouch") {
        boss.getAnimator().playAnimation("crouch", 0.016f);
    } else if (state == "TKRolling") {
        boss.getAnimator().playAnimation("roll", 0.016f);
    } else if (state == "TKStunned") {
        boss.getAnimator().playAnimation("stunned", 0.016f);
    } else if (state == "TKFire") {
        boss.getAnimator().playAnimation("fire", 0.016f);
    } else if (state == "FlippingDeath") {
        boss.getAnimator().playAnimation("death", 0.016f);
    } else {
        boss.getAnimator().playAnimation("walk", 0.016f);
    }
}
