#include "Managers/LevelManager.hpp"

#include <cmath>
#include <iostream>

namespace {
constexpr const char* FLAG_LAYER_NAME = "Flag";
}

bool LevelManager::loadLevel(
    const std::string& jsonPath,
    const std::string& tilesetTexturePath) {

    // 1. Delegate map parsing to MapManager
    if (!m_mapManager.loadMap(jsonPath, m_currentMap)) {
        std::cerr
            << "[LevelManager] Error: Failed to load map JSON: "
            << jsonPath
            << std::endl;
        return false;
    }

    // 2. Load main Tileset Texture
    if (!m_tilesetTexture.loadFromFile(tilesetTexturePath)) {
        std::cerr
            << "[LevelManager] Error: Failed to load tileset image: "
            << tilesetTexturePath
            << std::endl;
        return false;
    }

    // Pixel-art tiles must use nearest-neighbour sampling.
    // Repeating would also allow pixels from the opposite atlas
    // edge to bleed into a tile.
    m_tilesetTexture.setSmooth(false);
    m_tilesetTexture.setRepeated(false);

    // 3. Load Object Tileset Texture
    //
    // 1-1.tmj:
    // object.tsx -> firstgid = 925
    //
    // object.tsx references the separate object tileset image.
    const std::string objectTexturePath =
        "assets/maps/resources/object.png";

    if (!m_objectTexture.loadFromFile(objectTexturePath)) {
        std::cerr
            << "[LevelManager] Error: Failed to load object image: "
            << objectTexturePath
            << std::endl;
        return false;
    }

    m_objectTexture.setSmooth(false);
    m_objectTexture.setRepeated(false);

    // 4. Build VertexArray meshes
    if (!buildMapMesh()) {
        std::cerr
            << "[LevelManager] Error: Failed to build map rendering mesh."
            << std::endl;
        return false;
    }

    std::cout
        << "[LevelManager] Level successfully initialized: "
        << jsonPath
        << std::endl;

    return true;
}

bool LevelManager::buildMapMesh() {
    // -------------------------------------------------------------------------
    // Reset main tileset mesh
    // -------------------------------------------------------------------------
    m_vertices.clear();
    m_vertices.setPrimitiveType(sf::Quads);

    // -------------------------------------------------------------------------
    // Reset object/Flag mesh
    // -------------------------------------------------------------------------
    m_flagVertices.clear();
    m_flagVertices.setPrimitiveType(sf::Quads);

    if (m_currentMap.tileWidth == 0
        || m_currentMap.tileHeight == 0) {
        return false;
    }

    const int tileWidth = m_currentMap.tileWidth;
    const int tileHeight = m_currentMap.tileHeight;

    const int tilesetColumns =
        static_cast<int>(m_tilesetTexture.getSize().x)
        / tileWidth;

    const int objectColumns =
        static_cast<int>(m_objectTexture.getSize().x)
        / tileWidth;

    if (tilesetColumns <= 0) {
        std::cerr
            << "[LevelManager] ERROR: Invalid main tileset dimensions."
            << std::endl;
        return false;
    }

    if (objectColumns <= 0) {
        std::cerr
            << "[LevelManager] ERROR: Invalid object tileset dimensions."
            << std::endl;
        return false;
    }

    // -------------------------------------------------------------------------
    // 1. Count quads separately
    // -------------------------------------------------------------------------
    std::size_t totalQuads = 0;
    std::size_t totalFlagQuads = 0;

    for (const auto& layer : m_currentMap.tileLayers) {
        if (!layer.visible) {
            continue;
        }

        for (int gid : layer.data) {
            if (gid <= 0) {
                continue;
            }

            if (layer.name == FLAG_LAYER_NAME) {
                ++totalFlagQuads;
            } else {
                ++totalQuads;
            }
        }
    }

    // Resize once to avoid repeated reallocations.
    m_vertices.resize(totalQuads * 4);
    m_flagVertices.resize(totalFlagQuads * 4);

    std::size_t vertexIndex = 0;
    std::size_t flagVertexIndex = 0;

    // -------------------------------------------------------------------------
    // 2. Populate vertex buffers
    // -------------------------------------------------------------------------
    for (const auto& layer : m_currentMap.tileLayers) {
        if (!layer.visible) {
            continue;
        }

        const bool isFlagLayer =
            layer.name == FLAG_LAYER_NAME;

        for (int y = 0; y < layer.height; ++y) {
            for (int x = 0; x < layer.width; ++x) {

                const int dataIndex =
                    y * layer.width + x;

                if (dataIndex < 0
                    || dataIndex >= static_cast<int>(layer.data.size())) {
                    continue;
                }

                // Tiled stores the GLOBAL tile GID here.
                const int globalGid =
                    layer.data[dataIndex];

                if (globalGid <= 0) {
                    continue;
                }

                float posX = static_cast<float>(x * tileWidth);

                float posY = static_cast<float>(y * tileHeight);

                // =============================================================
                // FLAG LAYER
                // =============================================================
                if (isFlagLayer) {
                    posX += 16.f;
                    // object.tsx starts at firstgid 925.
                    //
                    // Examples:
                    //
                    // GID 925 -> local tile 0
                    // GID 926 -> local tile 1
                    // GID 927 -> local tile 2
                    //
                    // Your TMJ contains Flag GIDs such as:
                    // 1005, etc.
                    const int localTileIndex =
                        globalGid - OBJECT_FIRST_GID;

                    if (localTileIndex < 0) {
                        std::cerr
                            << "[LevelManager] WARNING: Flag GID "
                            << globalGid
                            << " is smaller than object firstgid "
                            << OBJECT_FIRST_GID
                            << ". Skipping."
                            << std::endl;
                        continue;
                    }

                    const int tu =
                        localTileIndex % objectColumns;

                    const int tv =
                        localTileIndex / objectColumns;

                    const float texX =
                        static_cast<float>(
                            tu * tileWidth);

                    const float texY =
                        static_cast<float>(
                            tv * tileHeight);

                    // Position
                    m_flagVertices[flagVertexIndex + 0].position =
                        sf::Vector2f(
                            posX,
                            posY);

                    m_flagVertices[flagVertexIndex + 1].position =
                        sf::Vector2f(
                            posX + tileWidth,
                            posY);

                    m_flagVertices[flagVertexIndex + 2].position =
                        sf::Vector2f(
                            posX + tileWidth,
                            posY + tileHeight);

                    m_flagVertices[flagVertexIndex + 3].position =
                        sf::Vector2f(
                            posX,
                            posY + tileHeight);

                    // Texture coordinates
                    m_flagVertices[flagVertexIndex + 0].texCoords =
                        sf::Vector2f(
                            texX,
                            texY);

                    m_flagVertices[flagVertexIndex + 1].texCoords =
                        sf::Vector2f(
                            texX + tileWidth,
                            texY);

                    m_flagVertices[flagVertexIndex + 2].texCoords =
                        sf::Vector2f(
                            texX + tileWidth,
                            texY + tileHeight);

                    m_flagVertices[flagVertexIndex + 3].texCoords =
                        sf::Vector2f(
                            texX,
                            texY + tileHeight);

                    flagVertexIndex += 4;

                    continue;
                }

                // =============================================================
                // NORMAL TILE LAYERS
                // =============================================================

                // Main tileset starts at GID 1.
                const int tileIndex_UV =
                    globalGid - 1;

                if (tileIndex_UV < 0) {
                    continue;
                }

                const int tu =
                    tileIndex_UV % tilesetColumns;

                const int tv =
                    tileIndex_UV / tilesetColumns;

                const float texX =
                    static_cast<float>(
                        tu * tileWidth);

                const float texY =
                    static_cast<float>(
                        tv * tileHeight);

                // Position
                m_vertices[vertexIndex + 0].position =
                    sf::Vector2f(
                        posX,
                        posY);

                m_vertices[vertexIndex + 1].position =
                    sf::Vector2f(
                        posX + tileWidth,
                        posY);

                m_vertices[vertexIndex + 2].position =
                    sf::Vector2f(
                        posX + tileWidth,
                        posY + tileHeight);

                m_vertices[vertexIndex + 3].position =
                    sf::Vector2f(
                        posX,
                        posY + tileHeight);

                // Texture coordinates
                m_vertices[vertexIndex + 0].texCoords =
                    sf::Vector2f(
                        texX,
                        texY);

                m_vertices[vertexIndex + 1].texCoords =
                    sf::Vector2f(
                        texX + tileWidth,
                        texY);

                m_vertices[vertexIndex + 2].texCoords =
                    sf::Vector2f(
                        texX + tileWidth,
                        texY + tileHeight);

                m_vertices[vertexIndex + 3].texCoords =
                    sf::Vector2f(
                        texX,
                        texY + tileHeight);

                vertexIndex += 4;
            }
        }
    }

    return true;
}

void LevelManager::draw(
    sf::RenderTarget& target,
    sf::RenderStates states) const {

    // -------------------------------------------------------------------------
    // Draw normal map layers
    // -------------------------------------------------------------------------
    states.texture = &m_tilesetTexture;
    target.draw(m_vertices, states);

    // -------------------------------------------------------------------------
    // Draw Flag layer from object.png
    // -------------------------------------------------------------------------
    states.texture = &m_objectTexture;
    target.draw(m_flagVertices, states);
}

// --- TILE QUERY IMPLEMENTATION ----------------------------------------------

int LevelManager::getTileID(
    const std::string& layerName,
    int tileX,
    int tileY) const {

    if (tileX < 0
        || tileX >= m_currentMap.width
        || tileY < 0
        || tileY >= m_currentMap.height) {
        return 0;
    }

    const TileLayer* layer =
        m_currentMap.getTileLayer(layerName);

    if (!layer || !layer->visible) {
        return 0;
    }

    const int index =
        tileY * layer->width + tileX;

    if (index >= 0
        && index < static_cast<int>(layer->data.size())) {
        return layer->data[index];
    }

    return 0;
}

bool LevelManager::isSolidAtTile(
    int tileX,
    int tileY) const {

    return getTileID(
        m_collisionLayerName,
        tileX,
        tileY) > 0;
}

bool LevelManager::isSolidAtPixel(
    float worldX,
    float worldY) const {

    if (m_currentMap.tileWidth == 0
        || m_currentMap.tileHeight == 0) {
        return false;
    }

    // Use std::floor so negative coordinates are handled correctly.
    const int tileX =
        static_cast<int>(
            std::floor(
                worldX / m_currentMap.tileWidth));

    const int tileY =
        static_cast<int>(
            std::floor(
                worldY / m_currentMap.tileHeight));

    return isSolidAtTile(tileX, tileY);
}

bool LevelManager::isInteractiveTile(
    int tileX,
    int tileY) const {

    return getTileID(
        m_interactiveLayerName,
        tileX,
        tileY) > 0;
}

bool LevelManager::setTileID(
    const std::string& layerName,
    int tileX,
    int tileY,
    int newTileID) {

    if (tileX < 0
        || tileX >= m_currentMap.width
        || tileY < 0
        || tileY >= m_currentMap.height) {
        return false;
    }

    for (auto& layer : m_currentMap.tileLayers) {
        if (layer.name == layerName) {

            const int index =
                tileY * layer.width + tileX;

            if (index >= 0
                && index < static_cast<int>(
                    layer.data.size())) {

                layer.data[index] = newTileID;

                return buildMapMesh();
            }
        }
    }

    return false;
}

// --- OBJECT QUERY IMPLEMENTATION --------------------------------------------

std::vector<MapObject> LevelManager::getObjectsFromLayer(
    const std::string& layerName) const {

    const ObjectLayer* layer =
        m_currentMap.getObjectLayer(layerName);

    if (layer) {
        return layer->objects;
    }

    return {};
}

bool LevelManager::getObjectByName(
    const std::string& layerName,
    const std::string& objectName,
    MapObject& outObject) const {

    const ObjectLayer* layer =
        m_currentMap.getObjectLayer(layerName);

    if (layer) {
        for (const auto& obj : layer->objects) {
            if (obj.name == objectName) {
                outObject = obj;
                return true;
            }
        }
    }

    return false;
}

std::vector<MapObject> LevelManager::getObjectsByClass(
    const std::string& layerName,
    const std::string& className) const {

    std::vector<MapObject> result;

    const ObjectLayer* layer =
        m_currentMap.getObjectLayer(layerName);

    if (layer) {
        for (const auto& obj : layer->objects) {
            if (obj.className == className) {
                result.push_back(obj);
            }
        }
    }

    return result;
}

std::vector<MapObject> LevelManager::getObjectsByClassAndName(
    const std::string& layerName,
    const std::string& className,
    const std::string& objectName) const {

    std::vector<MapObject> result;

    const ObjectLayer* layer =
        m_currentMap.getObjectLayer(layerName);

    if (layer) {
        for (const auto& obj : layer->objects) {
            if (obj.className == className
                && obj.name == objectName) {
                result.push_back(obj);
            }
        }
    }

    return result;
}

std::vector<MapObject> LevelManager::findObjects(
    const std::string& className,
    const std::string& objectName) const {

    std::vector<MapObject> result;

    for (const auto& layer : m_currentMap.objectLayers) {
        for (const auto& obj : layer.objects) {

            const bool matchesClass =
                className.empty()
                || (obj.className == className);

            const bool matchesName =
                objectName.empty()
                || (obj.name == objectName);

            if (matchesClass && matchesName) {
                result.push_back(obj);
            }
        }
    }

    return result;
}