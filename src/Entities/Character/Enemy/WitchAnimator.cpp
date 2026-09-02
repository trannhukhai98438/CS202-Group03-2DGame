#include "Entities/Character/Enemy/WitchAnimator.h"
#include "Entities/Character/Enemy/Witch.h"
#include <cmath>

void WitchAnimator::initAnimations(Witch& witch) {
    witch.loadSpriteTexture("assets/textures/witch.png", 1, 64.0f, 1);
    
    sf::Sprite& sprite = witch.getSprite();
    
    // Scale based on the Walk frame's actual visual height (165px)
    float scale = 64.0f / 165.0f;
    sprite.setScale(scale, scale);

    // Walk: 7 frames (Uniform W=130 centered on spine, H=165 to touch feet perfectly)
    witch.getAnimator().addAnimation("walk", Animation({
        sf::IntRect(35,  19, 130, 165),
        sf::IntRect(169, 19, 130, 165),
        sf::IntRect(302, 19, 130, 165),
        sf::IntRect(440, 19, 130, 165),
        sf::IntRect(580, 19, 130, 165),
        sf::IntRect(720, 19, 130, 165),
        sf::IntRect(858, 19, 130, 165)
    }, 0.10f));

    // Hold: ready to throw (Uniform W=180 centered on spine, H=184 to include feet at Y=370)
    witch.getAnimator().addAnimation("hold", Animation({
        sf::IntRect(46, 186, 180, 184)
    }, 1.0f));

    // Throw: arm extended tossing potion (Uniform W=180 centered on spine, H=184 to include feet)
    witch.getAnimator().addAnimation("throw", Animation({
        sf::IntRect(526, 186, 180, 184)
    }, 1.0f));

    // Squished: 2 frames lying flat / dizzy on the floor (Y=372 to cleanly cut off Attack frames)
    // H=158 so bottom is at Y=529 (the actual lowest pixel), preventing floating
    witch.getAnimator().addAnimation("squished", Animation({
        sf::IntRect(24,  372, 186, 158),
        sf::IntRect(229, 372, 186, 158)
    }, 0.2f));

    // Death animation: tumbling frames (Y=372, H=158)
    witch.getAnimator().addAnimation("flippingDeath", Animation({
        sf::IntRect(431, 372, 186, 158),
        sf::IntRect(619, 372, 186, 158),
        sf::IntRect(827, 372, 186, 158)
    }, 0.12f));
}

void WitchAnimator::applyAnimation(Witch& witch) {
    sf::Sprite& sprite = witch.getSprite();
    float absScaleX = std::abs(sprite.getScale().x);
    float absScaleY = std::abs(sprite.getScale().y);

    if (witch.getDirection() == MoveDirection::Right) {
        sprite.setScale(absScaleX, absScaleY);
    } else {
        sprite.setScale(-absScaleX, absScaleY);
    }

    std::string sName = witch.getStateName();
    if (sName == "FlippingDeath") {
        witch.getAnimator().playAnimation("flippingDeath", 0.016f);
    } else if (sName == "Squished") {
        witch.getAnimator().playAnimation("squished", 0.016f);
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

    // Anchor bottom-center of frame directly to feet on the floor
    // Because we aligned the spine to exactly width/2 for all frames, this prevents horizontal jitter
    // Because we ensured height reaches the exact feet pixels, this prevents vertical sinking
    sf::IntRect curRect = sprite.getTextureRect();
    sprite.setOrigin(curRect.width / 2.0f, static_cast<float>(curRect.height));
}
