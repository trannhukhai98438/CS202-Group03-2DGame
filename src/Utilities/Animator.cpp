#include "../../include/Utilities/Animator.h"

Animator::Animator(sf::Sprite& sprite)
    : targetSprite(sprite), currentFrameIndex(0), currentFrameTime(0.0f) {
}

void Animator::addAnimation(const std::string& name, const Animation& animation) {
    animations[name] = animation;
}

void Animator::playAnimation(const std::string& name, float deltaTime) {
    if (animations.find(name) == animations.end()) {
        return; // Animation not found
    }

    if (currentAnimation != name) {
        currentAnimation = name;
        currentFrameIndex = 0;
        currentFrameTime = 0.0f;
    }

    const Animation& anim = animations[currentAnimation];
    if (anim.frames.empty()) return;

    currentFrameTime += deltaTime;

    if (currentFrameTime >= anim.frameTime) {
        currentFrameTime = 0.0f; // Reset or subtract anim.frameTime
        currentFrameIndex = (currentFrameIndex + 1) % anim.frames.size();
    }

    targetSprite.setTextureRect(anim.frames[currentFrameIndex]);
    targetSprite.setOrigin(anim.frames[currentFrameIndex].width / 2.0f, (float)anim.frames[currentFrameIndex].height);
}
