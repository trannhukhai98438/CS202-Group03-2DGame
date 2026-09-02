#include "Utilities/ThunderFlashTexture.h"

namespace {
bool loadProcessedTexture(sf::Texture& texture) {
    if (!texture.loadFromFile("assets/textures/thunderflash2.png")) {
        return false;
    }
    texture.setSmooth(false);
    return true;
}

struct TextureResource {
    sf::Texture texture;
    bool loaded{loadProcessedTexture(texture)};
};
} // namespace

const sf::Texture* ThunderFlashTexture::get() {
    static const TextureResource resource;
    return resource.loaded ? &resource.texture : nullptr;
}
