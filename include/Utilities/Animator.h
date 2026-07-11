#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <unordered_map>

struct Animation {
    std::vector<sf::IntRect> frames;
    float frameTime;
    
    Animation() : frameTime(0.1f) {}
    Animation(std::vector<sf::IntRect> f, float t) : frames(f), frameTime(t) {}
};

class Animator {
private:
    sf::Sprite& targetSprite;
    std::unordered_map<std::string, Animation> animations;
    
    std::string currentAnimation;
    int currentFrameIndex;
    float currentFrameTime;

public:
    Animator(sf::Sprite& sprite);
    
    void addAnimation(const std::string& name, const Animation& animation);
    
    void playAnimation(const std::string& name, float deltaTime);
};
