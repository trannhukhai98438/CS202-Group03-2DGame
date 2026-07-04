#pragma once
#include "Character.h"
#include "Mario.h"
#include <memory>

enum class HeroType{
    Mario
};

class HeroFactory {
public:
    static std::unique_ptr<Character> createHero(HeroType type, float startX, float startY);
};