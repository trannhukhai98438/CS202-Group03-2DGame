#include "Entities/Block/Lifter.h"
#include "Gameplay/BlockThemePalette.h"

#include <algorithm>

Lifter::Lifter(float x, float y,
               float width, float height,
               float topBoundaryY, float bottomBoundaryY,
               bool movesUp,
               const BlockThemePalette& themePalette)
    : Block(x, y),
      themePalette(themePalette),
      boundTheme(MapTheme::Unspecified),
      hasBoundTheme(false),
      hasThemeTexture(false),
      velocity(0.0f, movesUp ? -MOVEMENT_SPEED : MOVEMENT_SPEED),
      topBoundary(std::min(topBoundaryY, bottomBoundaryY)),
      bottomBoundary(std::max(topBoundaryY, bottomBoundaryY)) {
    hitbox.setSize({width, height});
    hitbox.setPosition(position);
    hitbox.setFillColor(sf::Color(252, 152, 56));

    // Base texture is 16x16. With 2x scale and (0,0) origin,
    // texture rect (width/2, height/2) repeats the 16x16 tile and renders at exact (width x height) world size.
    sprite.setScale(2.0f, 2.0f);
    sprite.setOrigin(0.0f, 0.0f);

    sprite.setTextureRect(sf::IntRect(
        0, 0,
        static_cast<int>(width / 2.0f),
        static_cast<int>(height / 2.0f)));
    sprite.setPosition(position);
    syncThemeTexture();
}

void Lifter::syncThemeTexture() {
    const MapTheme activeTheme = themePalette.getActiveTheme();
    if (hasBoundTheme && activeTheme == boundTheme) return;

    const sf::Texture* themedTexture =
        themePalette.getTexture(BlockVisual::Lifter);
    hasThemeTexture = themedTexture != nullptr;
    if (themedTexture) {
        sprite.setTexture(*themedTexture);
    }

    boundTheme = activeTheme;
    hasBoundTheme = true;
}

void Lifter::update(float deltaTime) {
    position += velocity * deltaTime;

    if (velocity.y < 0.0f && position.y <= topBoundary) {
        position.y = bottomBoundary;
    } else if (velocity.y > 0.0f && position.y >= bottomBoundary) {
        position.y = topBoundary;
    }

    hitbox.setPosition(position);
    sprite.setPosition(position);
}

void Lifter::render(sf::RenderWindow& window) {
    // Pipe travel is resolved after block updates, so synchronize here as
    // well to apply the destination theme in the same rendered frame.
    syncThemeTexture();

    if (hasThemeTexture) {
        window.draw(sprite);
    } else {
        window.draw(hitbox);
    }
}

std::unique_ptr<Item> Lifter::hit(Hero*) {
    return nullptr;
}

