#include "Entities/Character/Enemy/KoopaAnimator.h"
#include "Entities/Character/Enemy/Koopa.h"
#include <cmath>

void KoopaAnimator::initAnimations(Koopa& koopa) {
    koopa.loadSpriteTexture("assets/textures/koopa.png", 1, 48.0f, 1);
    
    sf::Sprite& sprite = koopa.getSprite();
    // 1024 x 558 resolution -> walk frame height is ~142px
    float scale = 48.0f / 142.0f;
    sprite.setScale(scale, scale);

    // Color-scanned exact sub-rectangles from new Koopa sheet
    // Walk: 6 frames (Row 0)
    koopa.getAnimator().addAnimation("walk", Animation({
        sf::IntRect(40,  19, 97, 139),
        sf::IntRect(208, 19, 96, 139),
        sf::IntRect(375, 19, 97, 142),
        sf::IntRect(547, 19, 98, 139),
        sf::IntRect(719, 19, 93, 142),
        sf::IntRect(883, 19, 95, 142)
    }, 0.12f));

    // Shell state: static shell (Row 1, Col 2)
    koopa.getAnimator().addAnimation("shell", Animation({
        sf::IntRect(385, 249, 87, 82)
    }, 0.2f));

    // Spin: 3 frames fast spinning shell with motion blur and sparks (Row 1, Col 3, 4, 5)
    koopa.getAnimator().addAnimation("spin", Animation({
        sf::IntRect(551, 236, 96, 93),
        sf::IntRect(712, 230, 110, 97),
        sf::IntRect(866, 206, 118, 128)
    }, 0.05f));

    // FlippingDeath: 7 frames full tumbling sequence (Row 2, Col 0..6)
    koopa.getAnimator().addAnimation("flippingDeath", Animation({
        sf::IntRect(43,  394, 94, 137),
        sf::IntRect(208, 396, 100, 116),
        sf::IntRect(350, 379, 88, 110),
        sf::IntRect(448, 391, 113, 141),
        sf::IntRect(584, 398, 86, 129),
        sf::IntRect(716, 401, 100, 115),
        sf::IntRect(889, 392, 85, 139)
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
