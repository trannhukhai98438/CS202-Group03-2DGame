#include "Entities/Character/Enemy/KoopaAnimator.h"
#include "Entities/Character/Enemy/Koopa.h"
#include <cmath>

void KoopaAnimator::initAnimations(Koopa& koopa) {
    koopa.loadSpriteTexture("assets/textures/koopa.png", 1, 48.0f, 1);
    
    sf::Sprite& sprite = koopa.getSprite();
    // 1024 x 558 modern 2.5D spritesheet -> walk frame height is 160px
    float scale = 48.0f / 160.0f;
    sprite.setScale(scale, scale);

    // Exact sub-rectangles for modern 2.5D Koopa sheet
    // Walk: 6 frames (Row 0)
    koopa.getAnimator().addAnimation("walk", Animation({
        sf::IntRect(44,  29, 97,  160),
        sf::IntRect(210, 29, 97,  160),
        sf::IntRect(381, 29, 96,  160),
        sf::IntRect(551, 29, 96,  160),
        sf::IntRect(717, 29, 100, 160),
        sf::IntRect(885, 29, 97,  160)
    }, 0.12f));

    // Shell state: static shell (Row 1)
    koopa.getAnimator().addAnimation("shell", Animation({
        sf::IntRect(47, 255, 100, 79)
    }, 0.2f));

    // Spin: 3 frames fast spinning shell with motion trails (Row 1)
    koopa.getAnimator().addAnimation("spin", Animation({
        sf::IntRect(460, 247, 113, 80),
        sf::IntRect(668, 229, 99,  105),
        sf::IntRect(874, 229, 94,  107)
    }, 0.06f));

    // FlippingDeath: 4 frames tumbling sequence (Row 2)
    koopa.getAnimator().addAnimation("flippingDeath", Animation({
        sf::IntRect(55,  382, 151, 137),
        sf::IntRect(332, 374, 129, 123),
        sf::IntRect(564, 391, 161, 116),
        sf::IntRect(833, 390, 118, 148)
    }, 0.08f));
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
