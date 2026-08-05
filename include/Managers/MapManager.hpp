#pragma once
#include "MapData.hpp"
#include <string>

class MapManager {
public:
    MapManager() = default;
    ~MapManager() = default;

    // Parses a Tiled map JSON/TMJ file and populates the provided MapData structure
    bool loadMap(const std::string& filePath, MapData& outMapData);
};