#include "Managers/MapManager.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

bool MapManager::loadMap(const std::string& filePath, MapData& outMapData) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open map file: " << filePath << std::endl;
        return false;
    }

    nlohmann::json mapJson;
    try {
        file >> mapJson;
    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "JSON Parse Error in " << filePath << ": " << e.what() << std::endl;
        return false;
    }

    // Reset previous map data
    outMapData.tileLayers.clear();

    // Extract global map parameters
    outMapData.width = mapJson.value("width", 0);
    outMapData.height = mapJson.value("height", 0);
    outMapData.tileWidth = mapJson.value("tilewidth", 16);
    outMapData.tileHeight = mapJson.value("tileheight", 16);

    // Parse layers
    if (mapJson.contains("layers") && mapJson["layers"].is_array()) {
        for (const auto& layerJson : mapJson["layers"]) {
            std::string layerType = layerJson.value("type", "");

            // Process tile layers only
            if (layerType == "tilelayer") {
                TileLayer layer;
                layer.name = layerJson.value("name", "Unnamed_Layer");
                layer.width = layerJson.value("width", 0);
                layer.height = layerJson.value("height", 0);
                layer.visible = layerJson.value("visible", true);

                // Read 1D grid array
                if (layerJson.contains("data") && layerJson["data"].is_array()) {
                    layer.data = layerJson["data"].get<std::vector<int>>();
                }

                outMapData.tileLayers.push_back(layer);
            }
        }
    }

    return true;
}