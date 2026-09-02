#pragma once

#include "Managers/MapData.hpp"

#include <SFML/Graphics/Texture.hpp>

enum class BlockVisual {
    Brick,
    Lifter
};

class BlockThemePalette {
public:
    bool load();

    void setActiveTheme(MapTheme theme);
    MapTheme getActiveTheme() const;

    const sf::Texture* getTexture(BlockVisual visual) const;

private:
    MapTheme m_activeTheme{MapTheme::Unspecified};

    sf::Texture m_overworldBrick;
    sf::Texture m_undergroundBrick;
    sf::Texture m_castleBrick;
    sf::Texture m_overworldLifter;
    sf::Texture m_undergroundLifter;
    sf::Texture m_castleLifter;

    bool m_overworldBrickLoaded{false};
    bool m_undergroundBrickLoaded{false};
    bool m_castleBrickLoaded{false};
    bool m_overworldLifterLoaded{false};
    bool m_undergroundLifterLoaded{false};
    bool m_castleLifterLoaded{false};
};
