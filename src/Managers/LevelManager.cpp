#include "Managers/LevelManager.hpp"
#include <iostream>

bool LevelManager::loadLevel(const std::string& jsonPath, const std::string& tilesetTexturePath) {
    // 1. Delegate map parsing to MapManager
    if (!m_mapManager.loadMap(jsonPath, m_currentMap)) {
        std::cerr << "[LevelManager] Error: Failed to load map JSON: " << jsonPath << std::endl;
        return false;
    }

    // 2. Load Tileset Texture
    if (!m_tilesetTexture.loadFromFile(tilesetTexturePath)) {
        std::cerr << "[LevelManager] Error: Failed to load tileset image: " << tilesetTexturePath << std::endl;
        return false;
    }

    // 3. Build VertexArray mesh for batch rendering
    if (!buildMapMesh()) {
        std::cerr << "[LevelManager] Error: Failed to build map rendering mesh." << std::endl;
        return false;
    }

    std::cout << "[LevelManager] Level successfully initialized: " << jsonPath << std::endl;
    return true;
}

bool LevelManager::buildMapMesh() {
    m_vertices.clear();
    m_vertices.setPrimitiveType(sf::Quads);

    if (m_currentMap.tileWidth == 0 || m_currentMap.tileHeight == 0) return false;

    int tilesetColumns = m_tilesetTexture.getSize().x / m_currentMap.tileWidth;

    // Traverse all tile layers and populate quad vertices
    for (const auto& layer : m_currentMap.tileLayers) {
        if (!layer.visible) continue;

        for (int y = 0; y < layer.height; ++y) {
            for (int x = 0; x < layer.width; ++x) {
                int tileID = layer.data[y * layer.width + x];

                // Skip empty tiles (Tile ID 0)
                if (tileID == 0) continue;

                // Convert Tiled 1-based GID to 0-based index
                int tileIndex = tileID - 1;

                // Calculate UV coordinates on Tileset texture
                int tu = tileIndex % tilesetColumns;
                int tv = tileIndex / tilesetColumns;

                // World coordinates
                float posX = static_cast<float>(x * m_currentMap.tileWidth);
                float posY = static_cast<float>(y * m_currentMap.tileHeight);

                // Texture coordinates
                float texX = static_cast<float>(tu * m_currentMap.tileWidth);
                float texY = static_cast<float>(tv * m_currentMap.tileHeight);

                // Define Quad corners
                sf::Vertex quad[4];

                quad[0].position = sf::Vector2f(posX, posY);
                quad[1].position = sf::Vector2f(posX + m_currentMap.tileWidth, posY);
                quad[2].position = sf::Vector2f(posX + m_currentMap.tileWidth, posY + m_currentMap.tileHeight);
                quad[3].position = sf::Vector2f(posX, posY + m_currentMap.tileHeight);

                quad[0].texCoords = sf::Vector2f(texX, texY);
                quad[1].texCoords = sf::Vector2f(texX + m_currentMap.tileWidth, texY);
                quad[2].texCoords = sf::Vector2f(texX + m_currentMap.tileWidth, texY + m_currentMap.tileHeight);
                quad[3].texCoords = sf::Vector2f(texX, texY + m_currentMap.tileHeight);

                for (int i = 0; i < 4; ++i) {
                    m_vertices.append(quad[i]);
                }
            }
        }
    }
    return true;
}

void LevelManager::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.texture = &m_tilesetTexture;
    target.draw(m_vertices, states);
}

// --- TILE QUERY IMPLEMENTATION ---

int LevelManager::getTileID(const std::string& layerName, int tileX, int tileY) const {
    if (tileX < 0 || tileX >= m_currentMap.width || tileY < 0 || tileY >= m_currentMap.height) {
        return 0; // Out of map bounds
    }

    const TileLayer* layer = m_currentMap.getTileLayer(layerName);
    if (!layer || !layer->visible) return 0;

    int index = tileY * layer->width + tileX;
    if (index >= 0 && index < static_cast<int>(layer->data.size())) {
        return layer->data[index];
    }
    return 0;
}

bool LevelManager::isSolidAtTile(int tileX, int tileY) const {
    return getTileID(m_collisionLayerName, tileX, tileY) > 0;
}

bool LevelManager::isSolidAtPixel(float worldX, float worldY) const {
    if (m_currentMap.tileWidth == 0 || m_currentMap.tileHeight == 0) return false;

    int tileX = static_cast<int>(worldX) / m_currentMap.tileWidth;
    int tileY = static_cast<int>(worldY) / m_currentMap.tileHeight;

    return isSolidAtTile(tileX, tileY);
}

bool LevelManager::isInteractiveTile(int tileX, int tileY) const {
    return getTileID(m_interactiveLayerName, tileX, tileY) > 0;
}

bool LevelManager::setTileID(const std::string& layerName, int tileX, int tileY, int newTileID) {
    if (tileX < 0 || tileX >= m_currentMap.width || tileY < 0 || tileY >= m_currentMap.height) {
        return false;
    }

    for (auto& layer : m_currentMap.tileLayers) {
        if (layer.name == layerName) {
            int index = tileY * layer.width + tileX;
            if (index >= 0 && index < static_cast<int>(layer.data.size())) {
                layer.data[index] = newTileID;
                return buildMapMesh(); // Rebuild rendering mesh to reflect changes
            }
        }
    }
    return false;
}

// --- OBJECT QUERY IMPLEMENTATION ---

std::vector<MapObject> LevelManager::getObjectsFromLayer(const std::string& layerName) const {
    const ObjectLayer* layer = m_currentMap.getObjectLayer(layerName);
    if (layer && layer->visible) {
        return layer->objects;
    }
    return {};
}

bool LevelManager::getObjectByName(const std::string& layerName, const std::string& objectName, MapObject& outObject) const {
    const ObjectLayer* layer = m_currentMap.getObjectLayer(layerName);
    if (layer && layer->visible) {
        for (const auto& obj : layer->objects) {
            if (obj.name == objectName) {
                outObject = obj;
                return true;
            }
        }
    }
    return false;
}

std::vector<MapObject> LevelManager::getObjectsByClass(const std::string& layerName, const std::string& className) const {
    std::vector<MapObject> result;
    const ObjectLayer* layer = m_currentMap.getObjectLayer(layerName);
    if (layer && layer->visible) {
        for (const auto& obj : layer->objects) {
            if (obj.className == className) {
                result.push_back(obj);
            }
        }
    }
    return result;
}

std::vector<MapObject> LevelManager::getObjectsByClassAndName(const std::string& layerName, const std::string& className, const std::string& objectName) const {
    std::vector<MapObject> result;
    const ObjectLayer* layer = m_currentMap.getObjectLayer(layerName);
    if (layer && layer->visible) {
        for (const auto& obj : layer->objects) {
            if (obj.className == className && obj.name == objectName) {
                result.push_back(obj);
            }
        }
    }
    return result;
}

std::vector<MapObject> LevelManager::findObjects(const std::string& className, const std::string& objectName) const {
    std::vector<MapObject> result;

    for (const auto& layer : m_currentMap.objectLayers) {
        if (!layer.visible) continue;

        for (const auto& obj : layer.objects) {
            bool matchesClass = className.empty() || (obj.className == className);
            bool matchesName = objectName.empty() || (obj.name == objectName);

            if (matchesClass && matchesName) {
                result.push_back(obj);
            }
        }
    }
    return result;
}