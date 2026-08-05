#include "Entities/Character/Enemy/GoombaAnimator.h"
#include "Entities/Character/Enemy/Goomba.h"
#include <cmath>

void GoombaAnimator::initAnimations(Goomba& goomba) {
    sf::Vector2i frameSize = goomba.loadSpriteTexture("assets/textures/goomba.png", 6, 50.0f);
    if (frameSize.x > 0 && frameSize.y > 0) {
        int frameWidth = frameSize.x;
        int frameHeight = frameSize.y;

        goomba.getAnimator().addAnimation("walk", Animation({
            sf::IntRect(0, 0, frameWidth, frameHeight),
            sf::IntRect(frameWidth, 0, frameWidth, frameHeight),
            sf::IntRect(frameWidth * 2, 0, frameWidth, frameHeight),
            sf::IntRect(frameWidth * 3, 0, frameWidth, frameHeight)
        }, 0.15f));
        goomba.getAnimator().addAnimation("squished", Animation({
            sf::IntRect(frameWidth * 4, 0, frameWidth, frameHeight)
        }, 0.2f));
    }
}

void GoombaAnimator::applyAnimation(Goomba& goomba) {
    if (goomba.getStateName() == "FlippingDeath") return;

    sf::Sprite& sprite = goomba.getSprite();
    float absScaleX = std::abs(sprite.getScale().x);
    float absScaleY = std::abs(sprite.getScale().y);

    if (goomba.getDirection() == MoveDirection::Left) {
        sprite.setScale(absScaleX, absScaleY);
    } else {
        sprite.setScale(-absScaleX, absScaleY);
    }

    if (goomba.getIsSquished()) {
        goomba.getShape().setFillColor(sf::Color(100, 30, 30));
        goomba.getAnimator().playAnimation("squished", 0.016f);
    } else {
        goomba.getShape().setFillColor(sf::Color(165, 42, 42));
        goomba.getAnimator().playAnimation("walk", 0.016f);
    }
}
