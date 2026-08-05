#include "Entities/Character/Enemy/KoopaAnimator.h"
#include "Entities/Character/Enemy/Koopa.h"
#include <cmath>

void KoopaAnimator::initAnimations(Koopa& koopa) {
    sf::Vector2i frameSize = koopa.loadSpriteTexture("assets/textures/koopa.png", 6, 50.0f);
    if (frameSize.x > 0 && frameSize.y > 0) {
        int frameWidth = frameSize.x;
        int frameHeight = frameSize.y;

        koopa.getAnimator().addAnimation("walk", Animation({
            sf::IntRect(0, 0, frameWidth, frameHeight),
            sf::IntRect(frameWidth, 0, frameWidth, frameHeight),
            sf::IntRect(frameWidth * 2, 0, frameWidth, frameHeight),
            sf::IntRect(frameWidth * 3, 0, frameWidth, frameHeight)
        }, 0.15f));
        koopa.getAnimator().addAnimation("shell", Animation({
            sf::IntRect(frameWidth * 4, 0, frameWidth, frameHeight)
        }, 0.2f));
        koopa.getAnimator().addAnimation("spin", Animation({
            sf::IntRect(frameWidth * 5, 0, frameWidth, frameHeight)
        }, 0.1f));
    }
}

void KoopaAnimator::applyAnimation(Koopa& koopa) {
    if (koopa.getStateName() == "FlippingDeath") return;

    std::string sName = koopa.getStateName();
    sf::Sprite& sprite = koopa.getSprite();
    float absScaleX = std::abs(sprite.getScale().x);
    float absScaleY = std::abs(sprite.getScale().y);

    if (sName != "Shell" && sName != "SpinningShell") {
        if (koopa.getDirection() == MoveDirection::Right) {
            sprite.setScale(absScaleX, absScaleY);
        } else {
            sprite.setScale(-absScaleX, absScaleY);
        }
    }

    if (sName == "SpinningShell") {
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
