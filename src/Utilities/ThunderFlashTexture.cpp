#include "Utilities/ThunderFlashTexture.h"

#include <SFML/Graphics/Image.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <vector>

namespace {
constexpr sf::Uint8 DARK_BACKGROUND_THRESHOLD = 20;
constexpr int OUTLINE_PRESERVATION_RADIUS = 2;

// Only these areas are used at runtime. Processing each frame independently
// prevents adjacent characters, labels, and the gray effect-guide panels from
// bleeding across a crop when the dark-outline mask is expanded.
const std::array<sf::IntRect, 19> USED_FRAME_RECTS{{
    {16, 244, 129, 186},
    {151, 243, 139, 187}, {291, 245, 157, 185},
    {453, 246, 125, 184},
    {587, 240, 119, 190}, {709, 244, 136, 186},
    {853, 283, 132, 147},
    {985, 237, 122, 193}, {1110, 248, 132, 182},
    {1246, 249, 137, 181}, {1387, 247, 132, 183},
    {1525, 248, 130, 182}, {1655, 246, 121, 184},
    {1777, 248, 145, 182},
    {1198, 495, 344, 93},
    {1190, 638, 70, 68}, {1288, 628, 89, 88},
    {1404, 614, 93, 108}, {1521, 614, 105, 107}
}};

bool loadProcessedTexture(sf::Texture& texture) {
    sf::Image image;
    if (!image.loadFromFile("assets/textures/thunderflash2.png")) {
        return false;
    }

    const sf::Vector2u size = image.getSize();
    const std::size_t pixelCount =
        static_cast<std::size_t>(size.x) * size.y;
    const sf::Uint8* sourcePixels = image.getPixelsPtr();
    if (!sourcePixels || pixelCount == 0) return false;

    std::vector<sf::Uint8> pixels(sourcePixels,
                                  sourcePixels + pixelCount * 4);
    std::vector<sf::Uint8> visible(pixelCount, 0);

    // The supplied RGB sheet has a compressed near-black background rather
    // than an alpha channel. Treat bright-enough pixels as artwork, then grow
    // that mask slightly so the character's intentionally dark outline stays
    // intact while the noisy background becomes transparent.
    for (const sf::IntRect& frame : USED_FRAME_RECTS) {
        const int frameLeft = std::max(0, frame.left);
        const int frameTop = std::max(0, frame.top);
        const int frameRight = std::min(static_cast<int>(size.x),
                                        frame.left + frame.width);
        const int frameBottom = std::min(static_cast<int>(size.y),
                                         frame.top + frame.height);
        if (frameLeft >= frameRight || frameTop >= frameBottom) continue;

        for (int y = frameTop; y < frameBottom; ++y) {
            for (int x = frameLeft; x < frameRight; ++x) {
                const std::size_t index =
                    static_cast<std::size_t>(y) * size.x
                    + static_cast<unsigned int>(x);
                const std::size_t offset = index * 4;
                const sf::Uint8 brightness = std::max(
                    pixels[offset],
                    std::max(pixels[offset + 1], pixels[offset + 2]));
                if (brightness <= DARK_BACKGROUND_THRESHOLD) continue;

                const int left = std::max(
                    frameLeft, x - OUTLINE_PRESERVATION_RADIUS);
                const int right = std::min(
                    frameRight - 1, x + OUTLINE_PRESERVATION_RADIUS);
                const int top = std::max(
                    frameTop, y - OUTLINE_PRESERVATION_RADIUS);
                const int bottom = std::min(
                    frameBottom - 1, y + OUTLINE_PRESERVATION_RADIUS);
                for (int maskY = top; maskY <= bottom; ++maskY) {
                    for (int maskX = left; maskX <= right; ++maskX) {
                        visible[static_cast<std::size_t>(maskY) * size.x
                                + static_cast<unsigned int>(maskX)] = 1;
                    }
                }
            }
        }
    }

    for (std::size_t index = 0; index < pixelCount; ++index) {
        pixels[index * 4 + 3] = visible[index] ? 255 : 0;
    }
    image.create(size.x, size.y, pixels.data());

    if (!texture.loadFromImage(image)) return false;
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
