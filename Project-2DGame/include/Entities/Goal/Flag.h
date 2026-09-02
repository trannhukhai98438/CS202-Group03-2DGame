#pragma once

#include "Entities/Goal/LevelGoal.h"

class Flag final : public LevelGoal {
public:
    Flag(const sf::FloatRect& triggerBounds,
         int minimumScore = 100,
         int maximumScore = 1000);

    GoalResult tryActivate(const Hero& hero) override;
    sf::FloatRect getTriggerBounds() const override;
    bool isActivated() const override;
    std::string getActivationSfx() const override;
    std::string getCompletionBgm() const override;

private:
    sf::FloatRect m_triggerBounds;
    bool m_activated{false};
    int m_minimumScore;
    int m_maximumScore;
};
