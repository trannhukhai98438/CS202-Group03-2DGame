#include "Gameplay/BlockThemePalette.h"

#include <iostream>

bool BlockThemePalette::load() {
    m_overworldBrickLoaded =
        m_overworldBrick.loadFromFile("assets/textures/Brick_Overworld.png");
    m_undergroundBrickLoaded =
        m_undergroundBrick.loadFromFile("assets/textures/Brick_Underground.png");
    m_castleBrickLoaded =
        m_castleBrick.loadFromFile("assets/textures/Brick_Castle.png");
    m_overworldLifterLoaded =
        m_overworldLifter.loadFromFile("assets/textures/Lifter_Overworld.png");
    m_undergroundLifterLoaded =
        m_undergroundLifter.loadFromFile("assets/textures/Lifter_Underground.png");
    m_castleLifterLoaded =
        m_castleLifter.loadFromFile("assets/textures/Lifter_Castle.png");

    if (m_overworldLifterLoaded) {
        m_overworldLifter.setRepeated(true);
    }
    if (m_undergroundLifterLoaded) {
        m_undergroundLifter.setRepeated(true);
    }
    if (m_castleLifterLoaded) {
        m_castleLifter.setRepeated(true);
    }

    if (!m_overworldBrickLoaded) {
        std::cerr << "[BlockThemePalette] Cannot load Brick_Overworld.png.\n";
    }
    if (!m_undergroundBrickLoaded) {
        std::cerr << "[BlockThemePalette] Cannot load Brick_Underground.png.\n";
    }
    if (!m_castleBrickLoaded) {
        std::cerr << "[BlockThemePalette] Cannot load Brick_Castle.png.\n";
    }
    if (!m_overworldLifterLoaded) {
        std::cerr << "[BlockThemePalette] Cannot load Lifter_Overworld.png.\n";
    }
    if (!m_undergroundLifterLoaded) {
        std::cerr << "[BlockThemePalette] Cannot load Lifter_Underground.png.\n";
    }
    if (!m_castleLifterLoaded) {
        std::cerr << "[BlockThemePalette] Cannot load Lifter_Castle.png.\n";
    }

    return m_overworldBrickLoaded
        && m_undergroundBrickLoaded
        && m_castleBrickLoaded
        && m_overworldLifterLoaded
        && m_undergroundLifterLoaded
        && m_castleLifterLoaded;
}

void BlockThemePalette::setActiveTheme(MapTheme theme) {
    m_activeTheme = theme;
}

MapTheme BlockThemePalette::getActiveTheme() const {
    return m_activeTheme;
}

const sf::Texture* BlockThemePalette::getTexture(BlockVisual visual) const {
    if (visual == BlockVisual::Brick) {
        if (m_activeTheme == MapTheme::Castle && m_castleBrickLoaded) {
            return &m_castleBrick;
        }
        if (m_activeTheme == MapTheme::Underground
            && m_undergroundBrickLoaded) {
            return &m_undergroundBrick;
        }
        if (m_overworldBrickLoaded) {
            return &m_overworldBrick;
        }
        if (m_undergroundBrickLoaded) {
            return &m_undergroundBrick;
        }
        return m_castleBrickLoaded ? &m_castleBrick : nullptr;
    }

    if (m_activeTheme == MapTheme::Castle && m_castleLifterLoaded) {
        return &m_castleLifter;
    }
    if (m_activeTheme == MapTheme::Underground
        && m_undergroundLifterLoaded) {
        return &m_undergroundLifter;
    }
    if (m_overworldLifterLoaded) {
        return &m_overworldLifter;
    }
    if (m_undergroundLifterLoaded) {
        return &m_undergroundLifter;
    }
    return m_castleLifterLoaded ? &m_castleLifter : nullptr;
}
