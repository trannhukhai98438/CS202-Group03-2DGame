#include "Managers/MapManager.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

void MapManager::parseCustomProperties(const nlohmann::json& objJson, MapObject& outObj) {
    if (objJson.contains("properties") && objJson["properties"].is_array()) {
        for (const auto& prop : objJson["properties"]) {
            std::string propName = prop.value("name", "");

            if (propName == "targetMap") {
                outObj.targetMap = prop.value("value", "");
            }
            else if (propName == "direction") {
                outObj.direction = prop.value("value", "down");
            }
            else if (propName == "contain") {
                outObj.contain = prop.value("value", "none");
            }
            else if (propName == "count" || propName == "number") {
                outObj.count = prop.value("value", 1);
            }
        }
    }
}

bool MapManager::loadMap(const std::string& filePath, MapData& outMapData) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[MapManager] Error: Failed to open map file: " << filePath << std::endl;
        return false;
    }

    nlohmann::json mapJson;
    try {
        file >> mapJson;
    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "[MapManager] JSON Parse Error in " << filePath << ": " << e.what() << std::endl;
        return false;
    }

    outMapData.clear();

    outMapData.width = mapJson.value("width", 0);
    outMapData.height = mapJson.value("height", 0);
    outMapData.tileWidth = mapJson.value("tilewidth", 32);
    outMapData.tileHeight = mapJson.value("tileheight", 32);

    if (mapJson.contains("layers") && mapJson["layers"].is_array()) {
        for (const auto& layerJson : mapJson["layers"]) {
            std::string layerType = layerJson.value("type", "");

            // Parse standard Tile Layers
            if (layerType == "tilelayer") {
                TileLayer layer;
                layer.name = layerJson.value("name", "Unnamed_TileLayer");
                layer.width = layerJson.value("width", 0);
                layer.height = layerJson.value("height", 0);
                layer.visible = layerJson.value("visible", true);

                if (layerJson.contains("data") && layerJson["data"].is_array()) {
                    layer.data = layerJson["data"].get<std::vector<int>>();
                }

                outMapData.tileLayers.push_back(layer);
            }
            // Parse Object Layers (Triggers, Spawner, Interactive, etc.)
            else if (layerType == "objectgroup") {
                ObjectLayer objLayer;
                objLayer.name = layerJson.value("name", "Unnamed_ObjectLayer");
                objLayer.visible = layerJson.value("visible", true);

                if (layerJson.contains("objects") && layerJson["objects"].is_array()) {
                    for (const auto& objJson : layerJson["objects"]) {
                        MapObject obj;
                        obj.id = objJson.value("id", 0);
                        obj.name = objJson.value("name", "");

                        // Fallback to "type" if "class" is missing
                        obj.className = objJson.value("class", objJson.value("type", ""));

                        obj.x = objJson.value("x", 0.0f);
                        obj.y = objJson.value("y", 0.0f);
                        obj.width = objJson.value("width", 0.0f);
                        obj.height = objJson.value("height", 0.0f);

                        // Extract custom properties (targetMap, direction, contain, count)
                        parseCustomProperties(objJson, obj);

                        objLayer.objects.push_back(obj);
                    }
                }

                outMapData.objectLayers.push_back(objLayer);
            }
        }
    }

    return true;
}