#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

const int TILE_SIZE = 40;
const int MAP_WIDTH = 20;  // 20 columnas * 40px = 800px (Ancho de la ventana)
const int MAP_HEIGHT = 15; // 15 filas * 40px = 600px (Alto de la ventana)

enum TileType {
    TILE_VOID = 0,
    TILE_FLOOR = 1,
    TILE_HIGH_FLOOR = 2,
    TILE_BOX = 3
};

class Tilemap {
public:
    Tilemap();

    // Obtener el tipo de tile en una posición del mundo (x, y)
    TileType getTileAtPixel(float x, float y) const;
    
    // Cambiar el tipo de tile en la grilla (para el editor/modificación)
    void setTile(int gridX, int gridY, TileType type);

    // Dibujar la grilla del nivel
    void draw(sf::RenderWindow& window);

private:
    // Matriz de mapa 2D (15 filas x 20 columnas)
    int map[MAP_HEIGHT][MAP_WIDTH];
};