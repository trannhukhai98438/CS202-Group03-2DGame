#pragma once
#include <string>
#include <vector>

// Represents a single tile layer parsed from Tiled JSON
struct TileLayer {
    std::string name;
    int width{ 0 };   // Layer width in tiles
    int height{ 0 };  // Layer height in tiles
    bool visible{ true };
    std::vector<int> data; // 1D grid array storing Tile IDs
};

// Represents a single object parsed from Tiled Object Group (Interactive, Spawner, Trigger)
struct MapObject {
    int id{ 0 };
    std::string name;       // Object identifier (e.g., pipe pair name "A", "B")
    std::string className;  // Class/Type from Tiled (e.g., "pipe_in", "pipe_out", "wining", "Goomba", "Koopa", "question")
    float x{ 0.0f };        // World X position in pixels
    float y{ 0.0f };        // World Y position in pixels
    float width{ 0.0f };    // Object bounding width in pixels
    float height{ 0.0f };   // Object bounding height in pixels

    // Optional custom properties parsed from Tiled
    std::string targetMap{ "" };    // Destination map file name
    std::string direction{ "down" }; // Entry direction for pipes ("down", "up", "right")
    std::string contain{ "none" };   // Item contained inside interactive blocks ("coin", "mushroom", "flower", "star")
    int count{ 1 };                  // Quantity of items contained
};

// Represents an object layer group from Tiled
struct ObjectLayer {
    std::string name;
    bool visible{ true };
    std::vector<MapObject> objects;
};

// Container holding all map parameters and layer data
struct MapData {
    int width{ 0 };      // Map width in tiles
    int height{ 0 };     // Map height in tiles
    int tileWidth{ 0 };  // Single tile width in pixels (e.g., 32)
    int tileHeight{ 0 }; // Single tile height in pixels (e.g., 32)

    std::vector<TileLayer> tileLayers;
    std::vector<ObjectLayer> objectLayers;

    // Resets all map variables and clears layers
    void clear();

    // Helper functions for map bounds calculations in total pixels
    int getMapWidthPixels() const;
    int getMapHeightPixels() const;

    // Fast pointer lookup for layers by name
    const TileLayer* getTileLayer(const std::string& layerName) const;
    const ObjectLayer* getObjectLayer(const std::string& layerName) const;

    // Fast object query helpers
    std::vector<MapObject> getObjectsByClass(const std::string& className) const;
    const MapObject* findPipeExit(const std::string& pipeName) const;
};