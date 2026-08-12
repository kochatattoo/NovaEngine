#pragma once

namespace NK::Game::ECS {

    // v0.2: POD-компонент плитки Match3.
    // Хранит позицию в сетке + цветовой тип.
    // Никакой игровой логики — только данные.
    struct Match3TileComponent {
        int Row;     // 0..m_Rows-1
        int Col;     // 0..m_Cols-1
        int Type;    // -1 = пусто, 0..5 = цвет

        // Удобный ключ для именования entity (например "tile_3_5")
        static constexpr const char* Prefix = "tile_";
    };

} // namespace NK::Game::ECS
