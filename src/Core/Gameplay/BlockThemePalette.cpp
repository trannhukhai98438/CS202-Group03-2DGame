#include "Gameplay/BlockThemePalette.h"

#include <iostream>

bool BlockThemePalette::load() {
    m_overworldBrickLoaded =
        m_overworldBrick.loadFromFile("assets/textures/Brick.png");
    m_undergroundBrickLoaded =
        m_undergroundBrick.loadFromFile("assets/textures/Brick_2.png");
    m_overworldLifterLoaded =
        m_overworldLifter.loadFromFile("assets/textures/Lifter.png");
    m_undergroundLifterLoaded =
        m_undergroundLifter.loadFromFile("assets/textures/Lifter_2.png");

    if (m_overworldLifterLoaded) {
        m_overworldLifter.setRepeated(true);
    }
    if (m_undergroundLifterLoaded) {
        m_undergroundLifter.setRepeated(true);
    }

    if (!m_overworldBrickLoaded) {
        std::cerr << "[BlockThemePalette] Cannot load Brick.png.\n";
    }
    if (!m_undergroundBrickLoaded) {
        std::cerr << "[BlockThemePalette] Cannot load Brick_2.png.\n";
    }
    if (!m_overworldLifterLoaded) {
        std::cerr << "[BlockThemePalette] Cannot load Lifter.png.\n";
    }
    if (!m_undergroundLifterLoaded) {
        std::cerr << "[BlockThemePalette] Cannot load Lifter_2.png.\n";
    }

    return m_overworldBrickLoaded
        && m_undergroundBrickLoaded
        && m_overworldLifterLoaded
        && m_undergroundLifterLoaded;
}

void BlockThemePalette::setActiveTheme(MapTheme theme) {
    m_activeTheme = theme;
}

MapTheme BlockThemePalette::getActiveTheme() const {
    return m_activeTheme;
}

const sf::Texture* BlockThemePalette::getTexture(BlockVisual visual) const {
    const bool useUnderground = m_activeTheme == MapTheme::Underground;

    if (visual == BlockVisual::Brick) {
        if (useUnderground && m_undergroundBrickLoaded) {
            return &m_undergroundBrick;
        }
        if (m_overworldBrickLoaded) {
            return &m_overworldBrick;
        }
        return m_undergroundBrickLoaded ? &m_undergroundBrick : nullptr;
    }

    if (useUnderground && m_undergroundLifterLoaded) {
        return &m_undergroundLifter;
    }
    if (m_overworldLifterLoaded) {
        return &m_overworldLifter;
    }
    return m_undergroundLifterLoaded ? &m_undergroundLifter : nullptr;
}
