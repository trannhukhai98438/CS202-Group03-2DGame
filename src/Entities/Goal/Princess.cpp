#include "Entities/Goal/Princess.h"

#include "Entities/Character/Hero/Hero.h"

Princess::Princess(const sf::FloatRect& triggerBounds)
    : m_triggerBounds(triggerBounds) {
}

GoalResult Princess::tryActivate(const Hero& hero) {
    if (m_activated
        || !m_triggerBounds.intersects(hero.getBounds())) {
        return {};
    }

    m_activated = true;
    return {true, SCORE_AWARD};
}

sf::FloatRect Princess::getTriggerBounds() const {
    return m_triggerBounds;
}

bool Princess::isActivated() const {
    return m_activated;
}

std::string Princess::getActivationSfx() const {
    return {};
}

std::string Princess::getCompletionBgm() const {
    return "ending";
}
