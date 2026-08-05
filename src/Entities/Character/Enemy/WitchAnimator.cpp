#include "Entities/Character/Enemy/WitchAnimator.h"
#include "Entities/Character/Enemy/Witch.h"
#include <cmath>

void WitchAnimator::initAnimations(Witch& witch) {
    sf::Vector2i frameSize = witch.loadSpriteTexture("assets/textures/witch.png", 6, 96.0f);
    if (frameSize.x > 0 && frameSize.y > 0) {
        int frameWidth = frameSize.x;
        int frameHeight = frameSize.y;
        
        witch.getAnimator().addAnimation("walk", Animation({
            sf::IntRect(frameWidth * 2, 0, frameWidth, frameHeight),
            sf::IntRect(frameWidth * 3, 0, frameWidth, frameHeight),
            sf::IntRect(frameWidth * 4, 0, frameWidth, frameHeight),
            sf::IntRect(frameWidth * 5, 0, frameWidth, frameHeight)
        }, 0.2f));
        witch.getAnimator().addAnimation("hold", Animation({
            sf::IntRect(frameWidth, 0, frameWidth, frameHeight)
        }, 1.0f));
        witch.getAnimator().addAnimation("throw", Animation({
            sf::IntRect(0, 0, frameWidth, frameHeight)
        }, 1.0f));
    }
}

void WitchAnimator::applyAnimation(Witch& witch) {
    if (witch.getStateName() == "Squished" || witch.getStateName() == "FlippingDeath") return;

    sf::Sprite& sprite = witch.getSprite();
    float absScaleX = std::abs(sprite.getScale().x);
    float absScaleY = std::abs(sprite.getScale().y);

    if (witch.getDirection() == MoveDirection::Left) {
        sprite.setScale(absScaleX, absScaleY);
    } else {
        sprite.setScale(-absScaleX, absScaleY);
    }

    std::string sName = witch.getStateName();
    if (sName == "Throw") {
        ThrowState* throwState = dynamic_cast<ThrowState*>(witch.getCurrentState());
        if (throwState && throwState->hasThrownFlag()) {
            witch.getAnimator().playAnimation("throw", 0.016f);
        } else {
            witch.getAnimator().playAnimation("hold", 0.016f);
        }
    } else {
        witch.getAnimator().playAnimation("walk", 0.016f);
    }
}
