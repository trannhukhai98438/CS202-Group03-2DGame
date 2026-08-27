#include "Entities/Character/Enemy/GoombaAnimator.h"
#include "Entities/Character/Enemy/Goomba.h"
#include <cmath>
#include <iostream>

void GoombaAnimator::initAnimations(Goomba& goomba) {
    goomba.loadSpriteTexture("assets/textures/goomba.png", 1, 32.0f, 1);
    
    sf::Sprite& sprite = goomba.getSprite();
    // 1024 x 558 resolution -> walk frame height is ~103px
    float scale = 32.0f / 103.0f;
    sprite.setScale(scale, scale);

    // Exact sub-rectangle frames for new clean Goomba sheet
    goomba.getAnimator().addAnimation("walk", Animation({
        sf::IntRect(21,  21, 91, 103),
        sf::IntRect(158, 21, 90, 103),
        sf::IntRect(307, 21, 95, 103),
        sf::IntRect(464, 21, 91, 103),
        sf::IntRect(627, 21, 90, 103),
        sf::IntRect(772, 21, 94, 103),
        sf::IntRect(913, 21, 90, 103)
    }, 0.10f));

    goomba.getAnimator().addAnimation("squished", Animation({
        sf::IntRect(16, 196, 104, 69)
    }, 0.2f));

    goomba.getAnimator().addAnimation("flippingDeath", Animation({
        sf::IntRect(25,  307, 87, 99),
        sf::IntRect(166, 307, 95, 90),
        sf::IntRect(311, 307, 99, 90),
        sf::IntRect(460, 307, 95, 90)
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
