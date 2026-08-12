#pragma once
#include "Game/ECS/Match3System.h"
#include <vector>
#include <utility>
#include <tuple>
#include <functional>
#include <glm/glm.hpp>

namespace NK::Game::ECS {

    // v0.2: Лёгкая Lua-прокси обёртка вокруг Match3System.
    //
    // Проблема: sol2 не умеет биндить `&T::std_function_field` как property
    // без явного `sol::property(getter, setter)`. А если использовать
    // `sol::no_constructor` для Match3System — теряются property-биндинги.
    //
    // Решение: Match3BoardProxy хранит raw-указатель на Match3System
    // и проксирует все методы. В Lua создаётся через конструктор
    // `Match3Board.new(sys)`, потом передаётся board.
    //
    // API 1:1 с Match3System — обратная совместимость с Lua-скриптом.
    struct Match3BoardProxy {
        Match3System* sys = nullptr;

        Match3BoardProxy() = default;
        explicit Match3BoardProxy(Match3System* s) : sys(s) {}

        // === API для Lua (копия Match3System) ===
        void FillRandom() const { if (sys) sys->FillRandom(); }
        int  GetTile(int r, int c) const { return sys ? sys->GetTile(r, c) : -1; }
        void SetTile(int r, int c, int t) const { if (sys) sys->SetTile(r, c, t); }
        bool IsValidCell(int r, int c) const { return sys && sys->IsValidCell(r, c); }

        bool Swap(int r1, int c1, int r2, int c2) const {
            return sys ? sys->Swap(r1, c1, r2, c2) : false;
        }
        std::vector<std::pair<int, int>> FindMatches() const {
            return sys ? sys->FindMatches() : std::vector<std::pair<int, int>>{};
        }
        void RemoveTiles(const std::vector<std::pair<int, int>>& v) const {
            if (sys) sys->RemoveTiles(v);
        }
        void ApplyGravity() const { if (sys) sys->ApplyGravity(); }
        void FillEmpty() const { if (sys) sys->FillEmpty(); }
        bool HasPossibleMoves() const { return sys && sys->HasPossibleMoves(); }
        void Mix() const { if (sys) sys->Mix(); }

        int GetRows() const { return sys ? sys->GetRows() : 0; }
        int GetCols() const { return sys ? sys->GetCols() : 0; }
        float GetCellSize() const { return sys ? sys->GetCellSize() : 0.0f; }
        float GetPixelsPerUnit() const { return sys ? sys->GetPixelsPerUnit() : 0.0f; }
        std::tuple<float, float> GetCellPosition(int r, int c) const {
            if (!sys) return { 0.0f, 0.0f };
            auto p = sys->GetCellPosition(r, c);
            return { p.x, p.y };
        }

        // OnTileChanged — getter/setter для sol::property.
        // sol2 не умеет биндить `&T::std_function_field` напрямую,
        // поэтому оборачиваем в явный sol::property(getter, setter).
        // НЕ const: sol2 лучше работает с non-const setter.
        using TileChangedCb = std::function<void(int, int, int)>;
        TileChangedCb GetOnTileChanged() const {
            return sys ? sys->OnTileChanged : TileChangedCb{};
        }
        void SetOnTileChanged(TileChangedCb cb) {
            if (sys) sys->OnTileChanged = std::move(cb);
        }
    };

} // namespace NK::Game::ECS
