#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class Hero;

struct GoalResult {
    bool activated{false};
    int scoreAwarded{0};
};

class LevelGoal {
public:
    virtual ~LevelGoal() = default;

    virtual GoalResult tryActivate(const Hero& hero) = 0;
    virtual sf::FloatRect getTriggerBounds() const = 0;
    virtual bool isActivated() const = 0;
    virtual std::string getActivationSfx() const = 0;
    virtual std::string getCompletionBgm() const = 0;
};
