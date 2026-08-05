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

// Container for overall level data and layer list
struct MapData {
    int width{ 0 };      // Map width in tiles
    int height{ 0 };     // Map height in tiles
    int tileWidth{ 0 };  // Tile pixel width (e.g., 16)
    int tileHeight{ 0 }; // Tile pixel height (e.g., 16)
    std::vector<TileLayer> tileLayers;

    // Helper functions for easy data querying
    void clear();
    int getMapWidthPixels() const;
    int getMapHeightPixels() const;
    const TileLayer* getLayer(const std::string& layerName) const;
};