#include "Managers/MapManager.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

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
    outMapData.tileWidth = mapJson.value("tilewidth", 16);
    outMapData.tileHeight = mapJson.value("tileheight", 16);

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
            // Parse Object Layers
            else if (layerType == "objectgroup") {
                ObjectLayer objLayer;
                objLayer.name = layerJson.value("name", "Unnamed_ObjectLayer");
                objLayer.visible = layerJson.value("visible", true);

                if (layerJson.contains("objects") && layerJson["objects"].is_array()) {
                    for (const auto& objJson : layerJson["objects"]) {
                        MapObject obj;
                        obj.id   = objJson.value("id", 0);
                        obj.name = objJson.value("name", "");

                        // Tiled legacy: object type is stored in "type" field.
                        // Newer Tiled uses "class"; fall back to it if "type" is absent.
                        if (objJson.contains("type") && objJson["type"].is_string()
                            && !objJson["type"].get<std::string>().empty()) {
                            obj.className = objJson["type"].get<std::string>();
                        } else {
                            obj.className = objJson.value("class", "");
                        }

                        // gid present => tile object; y is bottom-left (Tiled convention)
                        obj.gid    = objJson.value("gid",    0);
                        obj.x      = objJson.value("x",      0.0f);
                        obj.y      = objJson.value("y",      0.0f);
                        obj.width  = objJson.value("width",  0.0f);
                        obj.height = objJson.value("height", 0.0f);

                        // Parse custom properties array:
                        // [{"name":"item","type":"string","value":"mushroom"}, ...]
                        if (objJson.contains("properties") && objJson["properties"].is_array()) {
                            for (const auto& prop : objJson["properties"]) {
                                if (!prop.contains("name")) continue;
                                std::string key = prop["name"].get<std::string>();
                                if (!prop.contains("value")) continue;
                                const auto& val = prop["value"];
                                if      (val.is_string())          obj.properties[key] = val.get<std::string>();
                                else if (val.is_number_integer())  obj.properties[key] = std::to_string(val.get<int>());
                                else if (val.is_number_float())    obj.properties[key] = std::to_string(val.get<float>());
                                else if (val.is_boolean())         obj.properties[key] = val.get<bool>() ? "true" : "false";
                            }
                        }

                        objLayer.objects.push_back(obj);
                    }
                }

                outMapData.objectLayers.push_back(objLayer);
            }
        }
    }

    return true;
}