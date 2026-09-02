#pragma once
#include <SFML/Graphics.hpp>

// ============================================================
// BrickParticle — internal class of BrickBlock- appears when the Block is broken.
// IT IS NOT AN ENTITY- no interaction with another entity class.
// be managed in class BrickBlock (render + physics).
// Frame is driven by BrickBlock's animator ("PieceofBrick" animation).
// ============================================================
struct BrickParticle {
    sf::Vector2f pos;
    sf::Vector2f vel;
    float        lifetime;
    // No frame field — all 4 particles share the same animated frame from BrickBlock's animator
};
