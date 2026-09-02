#ifndef GOOMBA_ANIMATOR_H
#define GOOMBA_ANIMATOR_H

class Goomba;

class GoombaAnimator {
public:
    void initAnimations(Goomba& goomba);
    void applyAnimation(Goomba& goomba);
};

#endif
