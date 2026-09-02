#pragma once

#include "Entities/Character/Hero/HeroFactory.h"

#include <string>

class GameWorld;
class SoundManager;

class LevelBuilder {
public:
    bool build(GameWorld& world,
               const std::string& mapPath,
               const std::string& tilesetPath,
               HeroType heroType) const;
};
