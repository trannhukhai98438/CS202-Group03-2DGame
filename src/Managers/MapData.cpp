#include "Managers/MapData.hpp"

void MapData::clear() {
    width = 0;
    height = 0;
    tileWidth = 0;
    tileHeight = 0;
    tileLayers.clear();
    objectLayers.clear();
}

int MapData::getMapWidthPixels() const {
    return width * tileWidth;
}

int MapData::getMapHeightPixels() const {
    return height * tileHeight;
}

const TileLayer* MapData::getTileLayer(const std::string& layerName) const {
    for (const auto& layer : tileLayers) {
        if (layer.name == layerName) {
            return &layer;
        }
    }
    return nullptr;
}

const ObjectLayer* MapData::getObjectLayer(const std::string& layerName) const {
    for (const auto& layer : objectLayers) {
        if (layer.name == layerName) {
            return &layer;
        }
    }
    return nullptr;
}