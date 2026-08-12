#include "Entities/Character/Enemy/KoopaAnimator.h"
#include "Entities/Character/Enemy/Koopa.h"
#include <cmath>

void KoopaAnimator::initAnimations(Koopa& koopa) {
    koopa.loadSpriteTexture("assets/textures/koopa.png", 1, 48.0f, 1);
    
    sf::Sprite& sprite = koopa.getSprite();
    // 1024 x 558 resolution -> walk frame height is ~133px
    float scale = 48.0f / 133.0f;
    sprite.setScale(scale, scale);

    // Exact sub-rectangle frames for new clean Koopa sheet
    koopa.getAnimator().addAnimation("walk", Animation({
        sf::IntRect(61,  47, 89, 133),
        sf::IntRect(225, 47, 89, 133),
        sf::IntRect(390, 47, 89, 133),
        sf::IntRect(550, 47, 89, 133),
        sf::IntRect(716, 47, 89, 133),
        sf::IntRect(879, 47, 89, 133)
    }, 0.12f));

    // Shell state: crouched shell
    koopa.getAnimator().addAnimation("shell", Animation({
        sf::IntRect(51, 289, 107, 66)
    }, 0.2f));

    // Spin animation: fast spinning shell (2 frames)
    koopa.getAnimator().addAnimation("spin", Animation({
        sf::IntRect(51,  289, 107, 66),
        sf::IntRect(213, 289, 108, 66)
    }, 0.08f));

    // Flipping death animation: shell tumbling upside down (3 frames)
    koopa.getAnimator().addAnimation("flippingDeath", Animation({
        sf::IntRect(55,  394, 99, 134),
        sf::IntRect(222, 396, 94, 132),
        sf::IntRect(386, 394, 110, 121)
    }, 0.1f));
}

void KoopaAnimator::applyAnimation(Koopa& koopa) {
    std::string sName = koopa.getStateName();
    sf::Sprite& sprite = koopa.getSprite();
    float absScaleX = std::abs(sprite.getScale().x);
    float absScaleY = std::abs(sprite.getScale().y);

    if (koopa.getDirection() == MoveDirection::Right) {
        sprite.setScale(absScaleX, absScaleY);
    } else {
        sprite.setScale(-absScaleX, absScaleY);
    }

    if (sName == "FlippingDeath") {
        koopa.getAnimator().playAnimation("flippingDeath", 0.016f);
    } else if (sName == "SpinningShell") {
        koopa.getShape().setFillColor(sf::Color(0, 255, 127));
        koopa.getAnimator().playAnimation("spin", 0.016f);
    } else if (sName == "Shell") {
        koopa.getShape().setFillColor(sf::Color(46, 139, 87));
        koopa.getAnimator().playAnimation("shell", 0.016f);
    } else {
        koopa.getShape().setFillColor(sf::Color(34, 139, 34));
        koopa.getAnimator().playAnimation("walk", 0.016f);
    }
}
