#pragma once

#include <SFML/Graphics/Texture.hpp>

namespace ThunderFlashTexture {

// Returns the shared, transparency-processed Thunder Flash texture, or
// nullptr when the source asset could not be loaded.
const sf::Texture* get();

} // namespace ThunderFlashTexture
