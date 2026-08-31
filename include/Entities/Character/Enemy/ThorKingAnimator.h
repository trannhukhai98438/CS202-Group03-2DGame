#ifndef THOR_KING_ANIMATOR_H
#define THOR_KING_ANIMATOR_H

#include <SFML/Graphics.hpp>

class ThorKing;

class ThorKingAnimator {
private:
    sf::Texture m_texNormal;
    sf::Texture m_texEnraged;
    sf::Texture m_texSkyMeteor;
    sf::Texture m_texWinged;
    bool m_loaded = false;

public:
    void initAnimations(ThorKing& boss);
    void applyAnimation(ThorKing& boss);
};

#endif
