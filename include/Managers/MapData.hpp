#pragma once
#include <string>
#include <vector>
#include <unordered_map>

enum class MapTheme {
    Unspecified,
    Overworld,
    Underground,
    Castle
};

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
    std::string name;       // Object identifier (for example, paired pipe names)
    std::string className;  // Tiled class/type (question, pipe_in, goomba, etc.)
    int gid{ 0 };           // GID > 0 means Tiled stores Y at the tile's bottom edge
    float x{ 0.0f };
    float y{ 0.0f };
    float width{ 0.0f };
    float height{ 0.0f };

    // Generic properties keep every scalar value available to gameplay code.
    std::unordered_map<std::string, std::string> properties;

    // Typed aliases from the incoming map schema.
    std::string targetMap;
    std::string direction{ "down" };
    std::string contain{ "none" };
    MapTheme theme{ MapTheme::Unspecified };
    int count{ 1 };

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
