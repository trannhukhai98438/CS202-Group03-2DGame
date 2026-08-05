#ifndef KOOPA_ANIMATOR_H
#define KOOPA_ANIMATOR_H

class Koopa;

class KoopaAnimator {
public:
    void initAnimations(Koopa& koopa);
    void applyAnimation(Koopa& koopa);
};

#endif
