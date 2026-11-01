#include "Tilemap.hpp"

Tilemap::Tilemap() {
    // Definimos el mapa inicial:
    // 0 = Abismo (Negro), 1 = Suelo Normal (Gris), 2 = Suelo Elevado (Azul), 3 = Caja (Marrón)
    int initialMap[MAP_HEIGHT][MAP_WIDTH] = {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // Filas 0 a 8: Fondo fuera de juego
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // Y = 360
        {2,2,2,2,2,2,2,2,2,2,0,0,2,2,2,2,2,2,2,2}, // Y = 360-400 (Suelo Elevado)
        {2,2,2,2,3,2,2,2,2,2,0,0,2,2,2,2,2,2,2,2}, // Y = 400-440 (Con una caja en X=4)
        {1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1}, // Y = 440-480 (Suelo Normal)
        {1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1}, // Y = 480-520 (Suelo Normal)
        {1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1}, // Y = 520-560 (Suelo Normal)
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}  // Fila 14: Límite inferior
    };

    for (int r = 0; r < MAP_HEIGHT; ++r) {
        for (int c = 0; c < MAP_WIDTH; ++c) {
            map[r][c] = initialMap[r][c];
        }
    }
}

TileType Tilemap::getTileAtPixel(float x, float y) const {
    int gridX = static_cast<int>(x) / TILE_SIZE;
    int gridY = static_cast<int>(y) / TILE_SIZE;

    if (gridX >= 0 && gridX < MAP_WIDTH && gridY >= 0 && gridY < MAP_HEIGHT) {
        return static_cast<TileType>(map[gridY][gridX]);
    }
    return TILE_VOID;
}

void Tilemap::setTile(int gridX, int gridY, TileType type) {
    if (gridX >= 0 && gridX < MAP_WIDTH && gridY >= 0 && gridY < MAP_HEIGHT) {
        map[gridY][gridX] = static_cast<int>(type);
    }
}

void Tilemap::draw(sf::RenderWindow& window) {
    sf::RectangleShape tileShape(sf::Vector2f(TILE_SIZE - 1.f, TILE_SIZE - 1.f)); // -1px para ver la cuadrícula

    for (int r = 0; r < MAP_HEIGHT; ++r) {
        for (int c = 0; c < MAP_WIDTH; ++c) {
            tileShape.setPosition(c * TILE_SIZE, r * TILE_SIZE);

            switch (map[r][c]) {
                case TILE_VOID:
                    tileShape.setFillColor(sf::Color(15, 15, 15)); // Abismo / Vacío
                    break;
                case TILE_FLOOR:
                    tileShape.setFillColor(sf::Color(70, 70, 70)); // Suelo Normal
                    break;
                case TILE_HIGH_FLOOR:
                    tileShape.setFillColor(sf::Color(60, 60, 110)); // Suelo Elevado
                    break;
                case TILE_BOX:
                    tileShape.setFillColor(sf::Color(139, 69, 19)); // Caja
                    break;
            }

            window.draw(tileShape);
        }
    }
}