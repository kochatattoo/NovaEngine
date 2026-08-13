#pragma once

namespace NK::Game::ECS {

    // v0.2: POD-компонент плитки Match3.
    // Хранит позицию в сетке + цветовой тип.
    // Никакой игровой логики — только данные.
    //
    // Имя entity — "tile_<row>_<col>" (префикс Prefix).
    struct Match3TileComponent
    {
        int Row = 0;     // 0..m_Rows-1
        int Col = 0;     // 0..m_Cols-1
        int Type = -1;   // -1 = пусто, 0..5 = цвет

        static constexpr const char* Prefix = "tile_";
    };

} // namespace NK::Game::ECS
