#pragma once

const float GRAVITY = 0.6f;
const float FLOOR_Y_MIN = 380.f;
const float FLOOR_Y_MAX = 540.f;

// Función para determinar si hay un abismo/vacío en una posición (X, Y)
inline bool isVoid(float x, float y) {
    // Un foso ubicado horizontalmente entre X = 500 y X = 650
    if (x >= 500.f && x <= 650.f) {
        return true;
    }
    return false;
}

// Configuración del escalón/borde elevado
const float LEDGE_Y = 460.f;          // Línea divisoria del escalón
const float LEDGE_MIN_Z = 15.f;       // Altura mínima de salto (Z) requerida para subir