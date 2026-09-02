#include "Entities/Character/Enemy/GoombaAnimator.h"
#include "Entities/Character/Enemy/Goomba.h"
#include <cmath>
#include <iostream>

void GoombaAnimator::initAnimations(Goomba& goomba) {
    goomba.loadSpriteTexture("assets/textures/goomba.png", 1, 32.0f, 1);
    
    sf::Sprite& sprite = goomba.getSprite();
    // 1024 x 557 modern 2.5D spritesheet -> walk frame height is 146px
    float scale = 32.0f / 146.0f;
    sprite.setScale(scale, scale);

    // Exact sub-rectangle frames for user's transparent 2.5D Goomba sheet
    goomba.getAnimator().addAnimation("walk", Animation({
        sf::IntRect(71,  18, 119, 146),
        sf::IntRect(237, 18, 119, 146),
        sf::IntRect(407, 18, 118, 146),
        sf::IntRect(569, 18, 118, 146),
        sf::IntRect(735, 18, 116, 146),
        sf::IntRect(890, 18, 118, 146)
    }, 0.10f));

    goomba.getAnimator().addAnimation("squished", Animation({
        sf::IntRect(198, 236, 162, 104),
        sf::IntRect(390, 277, 186, 63)
    }, 0.15f));

    goomba.getAnimator().addAnimation("flippingDeath", Animation({
        sf::IntRect(212, 382, 133, 167),
        sf::IntRect(405, 386, 161, 133),
        sf::IntRect(619, 386, 138, 157),
        sf::IntRect(817, 386, 154, 163)
    }, 0.1f));
}

void GoombaAnimator::applyAnimation(Goomba& goomba) {
    sf::Sprite& sprite = goomba.getSprite();
    float absScaleX = std::abs(sprite.getScale().x);
    float absScaleY = std::abs(sprite.getScale().y);

    if (goomba.getDirection() == MoveDirection::Right) {
        sprite.setScale(absScaleX, absScaleY);
    } else {
        sprite.setScale(-absScaleX, absScaleY);
    }

    if (goomba.getStateName() == "FlippingDeath") {
        goomba.getAnimator().playAnimation("flippingDeath", 0.016f);
    } else if (goomba.getIsSquished()) {
        goomba.getAnimator().playAnimation("squished", 0.016f);
    } else {
        goomba.getAnimator().playAnimation("walk", 0.016f);
    }
}
