#pragma once
#include <string>
#include <vector>
#include <unordered_map>

// Represents a single tile layer parsed from Tiled JSON
struct TileLayer {
    std::string name;
    int width{ 0 };   // Layer width in tiles
    int height{ 0 };  // Layer height in tiles
    bool visible{ true };
    std::vector<int> data; // 1D grid array storing Tile IDs
};

// Represents a single object parsed from Tiled Object Group (Spawners, Entities, Triggers)
struct MapObject {
    int id{ 0 };
    std::string name;
    std::string className; // Tiled "type" field (brick/question/coin/flag/invisible)
    int gid{ 0 };          // Tile GID >0 means this is a tile object (y = bottom-left in Tiled)
    float x{ 0.0f };       // World X in pixels (Tiled native, 16px grid)
    float y{ 0.0f };       // World Y in pixels (top for rects, bottom for tile objects)
    float width{ 0.0f };   // Object bounding width
    float height{ 0.0f };  // Object bounding height

    // Custom Tiled properties, e.g. {"item": "mushroom"}
    std::unordered_map<std::string, std::string> properties;

    // Returns property value for key, or def if not found
    std::string getProperty(const std::string& key,
                            const std::string& def = "") const {
        auto it = properties.find(key);
        return it != properties.end() ? it->second : def;
    }
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
    int tileWidth{ 0 };  // Single tile width in pixels (e.g., 16)
    int tileHeight{ 0 }; // Single tile height in pixels (e.g., 16)

    std::vector<TileLayer> tileLayers;
    std::vector<ObjectLayer> objectLayers;

    // Resets all map variables and clears layers
    void clear();

    // Helper functions for map bounds calculations
    int getMapWidthPixels() const;
    int getMapHeightPixels() const;

    // Fast pointer lookup for layers by name
    const TileLayer* getTileLayer(const std::string& layerName) const;
    const ObjectLayer* getObjectLayer(const std::string& layerName) const;
};