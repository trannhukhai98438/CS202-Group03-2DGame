#include "Entities/Character/Enemy/WitchAnimator.h"
#include "Entities/Character/Enemy/Witch.h"
#include <cmath>

void WitchAnimator::initAnimations(Witch& witch) {
    witch.loadSpriteTexture("assets/textures/witch.png", 1, 64.0f, 1);
    
    sf::Sprite& sprite = witch.getSprite();
    // 1024 x 558 resolution -> walk frame height is ~149px -> scale to 64px game height
    float scale = 64.0f / 149.0f;
    sprite.setScale(scale, scale);

    // Exact sub-rectangle frames for the new clean Witch sheet (no labels)
    witch.getAnimator().addAnimation("walk", Animation({
        sf::IntRect(33,  32, 87, 149),
        sf::IntRect(204, 32, 104, 149),
        sf::IntRect(371, 32, 107, 148),
        sf::IntRect(537, 32, 91, 148),
        sf::IntRect(684, 29, 105, 151),
        sf::IntRect(878, 32, 91, 148)
    }, 0.12f));

    // Hold: ready to throw (arm raised holding potion high)
    witch.getAnimator().addAnimation("hold", Animation({
        sf::IntRect(424, 383, 155, 155)
    }, 1.0f));

    // Throw: arm extended throwing potion forward
    witch.getAnimator().addAnimation("throw", Animation({
        sf::IntRect(654, 383, 162, 155)
    }, 1.0f));

    // Death frame: upside down tumbling (using first frame)
    witch.getAnimator().addAnimation("flippingDeath", Animation({
        sf::IntRect(33, 32, 87, 149)
    }, 1.0f));
}

void WitchAnimator::applyAnimation(Witch& witch) {
    if (witch.getStateName() == "Squished") return;

    sf::Sprite& sprite = witch.getSprite();
    float absScaleX = std::abs(sprite.getScale().x);
    float absScaleY = std::abs(sprite.getScale().y);

    // Match Koopa's correct facing/flip logic
    if (witch.getDirection() == MoveDirection::Right) {
        sprite.setScale(absScaleX, absScaleY);
    } else {
        sprite.setScale(-absScaleX, absScaleY);
    }

    std::string sName = witch.getStateName();
    if (sName == "FlippingDeath") {
        witch.getAnimator().playAnimation("flippingDeath", 0.016f);
    } else if (sName == "Throw") {
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
