#pragma once
#include "ECS/World.h"
#include "Game/ECS/Match3TileComponent.h"
#include <vector>
#include <utility>
#include <string>
#include <functional>
#include <glm/glm.hpp>

namespace NK::Game::ECS {

    // v0.2: ECS-система Match3.
    // Owns grid-логику + спавнит/обновляет tile-entities в World.
    //
    // ⚠️ Это МИГРАЦИЯ ЛОГИКИ, не рендера:
    // - Сетка остаётся в std::vector<std::vector<int>> (как у Match3Board)
    // - Каждая клетка — отдельная ECS-entity с компонентами:
    //     NameComponent ("tile_<r>_<c>")
    //     TransformComponent (Position = cellPos)
    //     SpriteComponent (Color по типу, Texture = nullptr — рендер добавим позже)
    //     Match3TileComponent (Row, Col, Type)
    // - Lua API (board:Swap и т.д.) сохранён для обратной совместимости.
    // - Визуально пока ничего не рисуется — это OK, рендер будет в v0.2.6.
    class Match3System {
    public:
        using TileType = int;

        // Callback для совместимости с Lua OnTileChanged(row, col, newType).
        using TileChangedCallback = std::function<void(int row, int col, TileType newType)>;

        Match3System(NK::ECS::World& world,
                     int rows = 10,
                     int cols = 10,
                     float cellSize = 64.0f,
                     float pixelsPerUnit = 100.0f);
        ~Match3System() = default;

        // === Жизненный цикл ===
        void Start();                // спавнит все tile-entities
        void Update(float deltaTime); // per-frame (пока пусто)

        // === API для Lua (раньше Match3Board::X) ===
        void FillRandom();
        TileType GetTile(int row, int col) const;
        void SetTile(int row, int col, TileType type);
        bool IsValidCell(int row, int col) const;

        bool Swap(int r1, int c1, int r2, int c2);
        std::vector<std::pair<int, int>> FindMatches() const;
        void RemoveTiles(const std::vector<std::pair<int, int>>& cells);
        void ApplyGravity();
        void FillEmpty();
        bool HasPossibleMoves();
        void Mix();

        int GetRows() const { return m_Rows; }
        int GetCols() const { return m_Cols; }
        float GetCellSize() const { return m_CellSize; }
        float GetPixelsPerUnit() const { return m_PixelsPerUnit; }
        glm::vec2 GetCellPosition(int row, int col) const;

        // Callback, дёргается при изменении плитки.
        TileChangedCallback OnTileChanged;

    private:
        // Спавн/апдейт/удаление entity для одной клетки
        entt::entity CreateTileEntity(int row, int col);
        void UpdateTileEntity(int row, int col, TileType newType);
        void DestroyTileEntity(int row, int col);

        // Хелперы
        std::string TileEntityName(int row, int col) const;
        static glm::vec4 GetColorForType(TileType type);
        entt::entity GetTileEntity(int row, int col) const;

        // Данные
        NK::ECS::World& m_World;
        int m_Rows;
        int m_Cols;
        float m_CellSize;
        float m_PixelsPerUnit;

        std::vector<std::vector<TileType>> m_Grid;
    };

} // namespace NK::Game::ECS
