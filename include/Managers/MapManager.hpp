#pragma once
#include "MapData.hpp"
#include <nlohmann/json_fwd.hpp>
#include <string>

class MapManager {
public:
    MapManager() = default;
    ~MapManager() = default;

    // Parses a Tiled map JSON/TMJ file and populates the provided MapData structure
    bool loadMap(const std::string& filePath, MapData& outMapData);

private:
    // Helper function to parse object custom properties (e.g., targetMap, direction, enemyType)
    void parseCustomProperties(const nlohmann::json& objJson, MapObject& outObj);
};
