#include "Managers/MapData.hpp"

void MapData::clear() {
    width = 0;
    height = 0;
    tileWidth = 0;
    tileHeight = 0;
    tileLayers.clear();
}

int MapData::getMapWidthPixels() const {
    return width * tileWidth;
}

int MapData::getMapHeightPixels() const {
    return height * tileHeight;
}

const TileLayer* MapData::getLayer(const std::string& layerName) const {
    for (const auto& layer : tileLayers) {
        if (layer.name == layerName) {
            return &layer;
        }
    }
    return nullptr;
}