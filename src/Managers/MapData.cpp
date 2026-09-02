#include "Managers/MapData.hpp"

void MapData::clear() {
    width = 0;
    height = 0;
    tileWidth = 32;
    tileHeight = 32;
    tilesets.clear();
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

// Retrieves all objects across all object layers matching a specific class name
std::vector<MapObject> MapData::getObjectsByClass(const std::string& className) const {
    std::vector<MapObject> result;
    for (const auto& layer : objectLayers) {
        for (const auto& obj : layer.objects) {
            if (obj.className == className) {
                result.push_back(obj);
            }
        }
    }
    return result;
}

// Finds the corresponding pipe_out exit point by matching the pipe name (e.g., "A")
const MapObject* MapData::findPipeExit(const std::string& pipeName) const {
    for (const auto& layer : objectLayers) {
        for (const auto& obj : layer.objects) {
            if (obj.className == "pipe_out" && obj.name == pipeName) {
                return &obj;
            }
        }
    }
    return nullptr;
}
