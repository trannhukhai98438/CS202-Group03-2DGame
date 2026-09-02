#include "Entities/Goal/Flag.h"

#include "Entities/Character/Hero/Hero.h"

#include <algorithm>
#include <cmath>

Flag::Flag(const sf::FloatRect& triggerBounds,
           int minimumScore,
           int maximumScore)
    : m_triggerBounds(triggerBounds),
      m_minimumScore(std::min(minimumScore, maximumScore)),
      m_maximumScore(std::max(minimumScore, maximumScore)) {
}

GoalResult Flag::tryActivate(const Hero& hero) {
    if (m_activated
        || !m_triggerBounds.intersects(hero.getBounds())) {
        return {};
    }

    const sf::FloatRect heroBounds = hero.getBounds();
    const float heroFeetY = heroBounds.top + heroBounds.height;
    const float flagBottom = m_triggerBounds.top + m_triggerBounds.height;
    const float heightRatio = m_triggerBounds.height > 0.0f
        ? std::clamp((flagBottom - heroFeetY) / m_triggerBounds.height,
                     0.0f, 1.0f)
        : 0.0f;

    const float scoreRange = static_cast<float>(m_maximumScore - m_minimumScore);
    const int score = m_minimumScore
        + static_cast<int>(std::lround(heightRatio * scoreRange));

    m_activated = true;
    return {true, score};
}

sf::FloatRect Flag::getTriggerBounds() const {
    return m_triggerBounds;
}

bool Flag::isActivated() const {
    return m_activated;
}

std::string Flag::getActivationSfx() const {
    return "flagpole";
}

std::string Flag::getCompletionBgm() const {
    return "course_clear";
}
