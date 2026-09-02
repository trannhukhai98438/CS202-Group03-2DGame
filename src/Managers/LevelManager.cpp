#include "Managers/LevelManager.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <utility>

namespace {
std::filesystem::path getTilesetImagePath(
    const std::filesystem::path& mapPath,
    const std::string& tilesetSource) {
    const std::filesystem::path tsxPath =
        (mapPath.parent_path() / tilesetSource).lexically_normal();
    std::ifstream tsxFile(tsxPath);
    std::string line;
    while (std::getline(tsxFile, line)) {
        const std::size_t imageTag = line.find("<image");
        if (imageTag == std::string::npos) continue;

        constexpr const char* SOURCE_ATTRIBUTE = "source=\"";
        const std::size_t sourceStart = line.find(
            SOURCE_ATTRIBUTE, imageTag);
        if (sourceStart == std::string::npos) continue;

        const std::size_t valueStart =
            sourceStart + std::char_traits<char>::length(SOURCE_ATTRIBUTE);
        const std::size_t valueEnd = line.find('"', valueStart);
        if (valueEnd == std::string::npos) continue;

        return (tsxPath.parent_path()
                / line.substr(valueStart, valueEnd - valueStart))
            .lexically_normal();
    }
    return {};
}
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

    // 3. Resolve optional secondary object tileset from this map's metadata.
    m_mainFirstGid = 1;
    m_objectFirstGid = 0;
    m_objectLastGid = 0;
    m_hasObjectTileset = false;

    const TilesetReference* objectTileset = nullptr;
    bool mainTilesetFound = false;
    for (const TilesetReference& tileset : m_currentMap.tilesets) {
        const std::filesystem::path sourcePath(tileset.source);
        if (sourcePath.stem() == "object") {
            objectTileset = &tileset;
            m_objectFirstGid = tileset.firstGid;
        } else if (!mainTilesetFound) {
            m_mainFirstGid = tileset.firstGid;
            mainTilesetFound = true;
        }
    }

    if (objectTileset) {
        m_objectLastGid = std::numeric_limits<int>::max();
        for (const TilesetReference& tileset : m_currentMap.tilesets) {
            if (tileset.firstGid > m_objectFirstGid) {
                m_objectLastGid = std::min(
                    m_objectLastGid, tileset.firstGid - 1);
            }
        }

        const std::filesystem::path objectTexturePath =
            getTilesetImagePath(jsonPath, objectTileset->source);
        if (objectTexturePath.empty()
            || !m_objectTexture.loadFromFile(objectTexturePath.string())) {
            std::cerr
                << "[LevelManager] Error: Failed to load object tileset image for "
                << objectTileset->source
                << std::endl;
            return false;
        }

        m_objectTexture.setSmooth(false);
        m_objectTexture.setRepeated(false);
        m_hasObjectTileset = true;
    }

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
    m_layerMeshes.clear();

    if (m_currentMap.tileWidth == 0
        || m_currentMap.tileHeight == 0) {
        return false;
    }

    const int tileWidth = m_currentMap.tileWidth;
    const int tileHeight = m_currentMap.tileHeight;
    const int tilesetColumns =
        static_cast<int>(m_tilesetTexture.getSize().x) / tileWidth;
    const int objectColumns = m_hasObjectTileset
        ? static_cast<int>(m_objectTexture.getSize().x) / tileWidth
        : 0;

    if (tilesetColumns <= 0) {
        std::cerr << "[LevelManager] ERROR: Invalid main tileset dimensions.\n";
        return false;
    }
    if (m_hasObjectTileset && objectColumns <= 0) {
        std::cerr << "[LevelManager] ERROR: Invalid object tileset dimensions.\n";
        return false;
    }

    const auto writeQuad = [tileWidth, tileHeight](
                               sf::VertexArray& vertices,
                               std::size_t index,
                               float positionX,
                               float positionY,
                               float textureX,
                               float textureY) {
        vertices[index + 0].position = {positionX, positionY};
        vertices[index + 1].position = {
            positionX + tileWidth, positionY};
        vertices[index + 2].position = {
            positionX + tileWidth, positionY + tileHeight};
        vertices[index + 3].position = {
            positionX, positionY + tileHeight};

        vertices[index + 0].texCoords = {textureX, textureY};
        vertices[index + 1].texCoords = {
            textureX + tileWidth, textureY};
        vertices[index + 2].texCoords = {
            textureX + tileWidth, textureY + tileHeight};
        vertices[index + 3].texCoords = {
            textureX, textureY + tileHeight};
    };

    m_layerMeshes.reserve(m_currentMap.tileLayers.size());
    for (const TileLayer& layer : m_currentMap.tileLayers) {
        if (!layer.visible) continue;

        LayerMesh mesh;
        mesh.mainVertices.setPrimitiveType(sf::Quads);
        mesh.objectVertices.setPrimitiveType(sf::Quads);

        std::size_t mainQuadCount = 0;
        std::size_t objectQuadCount = 0;
        for (const int gid : layer.data) {
            if (gid <= 0) continue;
            const bool isObjectTile =
                m_hasObjectTileset
                && gid >= m_objectFirstGid
                && gid <= m_objectLastGid;
            if (isObjectTile) {
                ++objectQuadCount;
            } else {
                ++mainQuadCount;
            }
        }

        mesh.mainVertices.resize(mainQuadCount * 4);
        mesh.objectVertices.resize(objectQuadCount * 4);
        std::size_t mainVertexIndex = 0;
        std::size_t objectVertexIndex = 0;

        for (int y = 0; y < layer.height; ++y) {
            for (int x = 0; x < layer.width; ++x) {
                const int dataIndex = y * layer.width + x;
                if (dataIndex < 0
                    || dataIndex >= static_cast<int>(layer.data.size())) {
                    continue;
                }

                const int globalGid = layer.data[dataIndex];
                if (globalGid <= 0) continue;

                const float positionX =
                    static_cast<float>(x * tileWidth) + layer.offsetX;
                const float positionY =
                    static_cast<float>(y * tileHeight) + layer.offsetY;
                const bool isObjectTile =
                    m_hasObjectTileset
                    && globalGid >= m_objectFirstGid
                    && globalGid <= m_objectLastGid;

                if (isObjectTile) {
                    const int localTileIndex =
                        globalGid - m_objectFirstGid;
                    const int textureColumn =
                        localTileIndex % objectColumns;
                    const int textureRow =
                        localTileIndex / objectColumns;
                    writeQuad(
                        mesh.objectVertices,
                        objectVertexIndex,
                        positionX,
                        positionY,
                        static_cast<float>(textureColumn * tileWidth),
                        static_cast<float>(textureRow * tileHeight));
                    objectVertexIndex += 4;
                    continue;
                }

                const int localTileIndex = globalGid - m_mainFirstGid;
                if (localTileIndex < 0) {
                    std::cerr << "[LevelManager] WARNING: Tile GID "
                              << globalGid
                              << " is below main tileset firstgid "
                              << m_mainFirstGid << ". Skipping.\n";
                    continue;
                }

                const int textureColumn =
                    localTileIndex % tilesetColumns;
                const int textureRow =
                    localTileIndex / tilesetColumns;
                writeQuad(
                    mesh.mainVertices,
                    mainVertexIndex,
                    positionX,
                    positionY,
                    static_cast<float>(textureColumn * tileWidth),
                    static_cast<float>(textureRow * tileHeight));
                mainVertexIndex += 4;
            }
        }

        m_layerMeshes.push_back(std::move(mesh));
    }

    return true;
}

void LevelManager::draw(
    sf::RenderTarget& target,
    sf::RenderStates states) const {
    for (const LayerMesh& layer : m_layerMeshes) {
        if (layer.mainVertices.getVertexCount() > 0) {
            states.texture = &m_tilesetTexture;
            target.draw(layer.mainVertices, states);
        }
        if (layer.objectVertices.getVertexCount() > 0) {
            states.texture = &m_objectTexture;
            target.draw(layer.objectVertices, states);
        }
    }
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
