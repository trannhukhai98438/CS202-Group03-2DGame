#pragma once

#include <SFML/Graphics.hpp>
#include <cstddef>
#include <string>
#include <vector>

#include "MapManager.hpp"
#include "MapData.hpp"

// Handles level rendering via SFML and provides query APIs for Physics & Game Logic
class LevelManager : public sf::Drawable {
private:
    struct LayerMesh {
        sf::VertexArray mainVertices;
        sf::VertexArray objectVertices;
    };

    MapManager m_mapManager;
    MapData m_currentMap;

    sf::Texture m_tilesetTexture;
    sf::Texture m_objectTexture;
    std::vector<LayerMesh> m_layerMeshes;
    int m_mainFirstGid{1};
    int m_objectFirstGid{0};
    int m_objectLastGid{0};
    bool m_hasObjectTileset{false};

    std::string m_collisionLayerName{ "Terrain" };
    std::string m_interactiveLayerName{ "Interactive" };

private:
    // Builds SFML Quad Vertices and UV texture coordinates
    bool buildMapMesh();

    // SFML sf::Drawable interface override for direct window.draw() usage
    virtual void draw(sf::RenderTarget& target,
                      sf::RenderStates states) const override;

public:
    LevelManager() = default;
    ~LevelManager() = default;

    // Initializes level by parsing JSON via MapManager and loading tileset texture
    bool loadLevel(const std::string& jsonPath,
                   const std::string& tilesetTexturePath);

    // --- MAP METRICS GETTERS ---
    int getMapWidthPixels() const {
        return m_currentMap.getMapWidthPixels();
    }

    int getMapHeightPixels() const {
        return m_currentMap.getMapHeightPixels();
    }

    int getTileWidth() const {
        return m_currentMap.tileWidth;
    }

    int getTileHeight() const {
        return m_currentMap.tileHeight;
    }

    int getMapWidthTiles() const {
        return m_currentMap.width;
    }

    int getMapHeightTiles() const {
        return m_currentMap.height;
    }

    // --- TILE & COLLISION API ---
    int getTileID(const std::string& layerName,
                  int tileX,
                  int tileY) const;

    bool isSolidAtTile(int tileX, int tileY) const;

    bool isSolidAtPixel(float worldX,
                        float worldY) const;

    bool isInteractiveTile(int tileX,
                           int tileY) const;

    // Dynamically update or destroy a tile
    // (e.g., breakable brick / question block)
    bool setTileID(const std::string& layerName,
                   int tileX,
                   int tileY,
                   int newTileID);

    // --- OBJECT QUERY API ---
    // Get all objects within a specific object layer
    std::vector<MapObject> getObjectsFromLayer(
        const std::string& layerName) const;

    // Query a single object by unique Name within a layer
    bool getObjectByName(
        const std::string& layerName,
        const std::string& objectName,
        MapObject& outObject) const;

    // Query objects by ClassName within a specific layer
    std::vector<MapObject> getObjectsByClass(
        const std::string& layerName,
        const std::string& className) const;

    // Query objects matching BOTH ClassName AND Name
    // within a specific layer
    std::vector<MapObject> getObjectsByClassAndName(
        const std::string& layerName,
        const std::string& className,
        const std::string& objectName) const;

    // Flexible search across ALL object layers by className,
    // objectName, or both
    std::vector<MapObject> findObjects(
        const std::string& className = "",
        const std::string& objectName = "") const;

    // Layer name setters
    void setCollisionLayerName(const std::string& name) {
        m_collisionLayerName = name;
    }

    void setInteractiveLayerName(const std::string& name) {
        m_interactiveLayerName = name;
    }

    const MapData& getMapData() const {
        return m_currentMap;
    }
};
