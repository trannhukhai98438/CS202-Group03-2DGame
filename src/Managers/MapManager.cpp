#include "Managers/MapManager.hpp"
#include <cctype>
#include <exception>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

MapTheme MapManager::parseTheme(const std::string& value) {
    std::string normalized;
    normalized.reserve(value.size());

    for (const unsigned char character : value) {
        if (std::isspace(character) || character == '-'
            || character == '_') {
            continue;
        }
        normalized.push_back(static_cast<char>(std::tolower(character)));
    }

    if (normalized == "overworld") return MapTheme::Overworld;
    if (normalized == "underground") return MapTheme::Underground;
    return MapTheme::Unspecified;
}

void MapManager::parseCustomProperties(const nlohmann::json& objJson, MapObject& outObj) {
    if (!objJson.contains("properties")
        || !objJson["properties"].is_array()) return;

    for (const auto& prop : objJson["properties"]) {
        const std::string propName = prop.value("name", "");
        if (propName.empty() || !prop.contains("value")) continue;

        const auto& value = prop["value"];
        std::string textValue;
        if (value.is_string()) {
            textValue = value.get<std::string>();
        } else if (value.is_number_integer()) {
            textValue = std::to_string(value.get<int>());
        } else if (value.is_number_float()) {
            textValue = std::to_string(value.get<float>());
        } else if (value.is_boolean()) {
            textValue = value.get<bool>() ? "true" : "false";
        } else {
            continue;
        }
        outObj.properties[propName] = textValue;

        if (propName == "targetMap") {
            outObj.targetMap = textValue;
        } else if (propName == "direction") {
            outObj.direction = textValue;
        } else if (propName == "contain") {
            outObj.contain = textValue;
        } else if (propName == "theme") {
            outObj.theme = parseTheme(textValue);
        } else if (propName == "count"
                   || propName == "number"
                   || propName == "quantity") {
            try {
                outObj.count = std::stoi(textValue);
            } catch (const std::exception&) {
                outObj.count = 1;
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
                        obj.id   = objJson.value("id", 0);
                        obj.name = objJson.value("name", "");

                        // Prefer modern Tiled "class", then legacy "type".
                        obj.className = objJson.value("class", "");
                        if (obj.className.empty()) {
                            obj.className = objJson.value("type", "");
                        }

                        // gid present => tile object; y is bottom-left (Tiled convention)
                        obj.gid    = objJson.value("gid",    0);
                        obj.x      = objJson.value("x",      0.0f);
                        obj.y      = objJson.value("y",      0.0f);
                        obj.width  = objJson.value("width",  0.0f);
                        obj.height = objJson.value("height", 0.0f);

                        // Retain every scalar property and populate typed aliases.
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
