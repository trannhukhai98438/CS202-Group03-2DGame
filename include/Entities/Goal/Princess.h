#pragma once

#include "Entities/Goal/LevelGoal.h"

class Princess final : public LevelGoal {
public:
    explicit Princess(const sf::FloatRect& triggerBounds);

    GoalResult tryActivate(const Hero& hero) override;
    sf::FloatRect getTriggerBounds() const override;
    bool isActivated() const override;
    std::string getActivationSfx() const override;
    std::string getCompletionBgm() const override;

private:
    static constexpr int SCORE_AWARD = 500;

    sf::FloatRect m_triggerBounds;
    bool m_activated{false};
};
