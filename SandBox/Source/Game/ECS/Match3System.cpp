#include "Game/ECS/Match3System.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Sprite.h"
#include "ECS/Components/NameComponent.h"
#include "Core/Log.h"

#include <random>
#include <sstream>

namespace NK::Game::ECS {

    // ========================================================================
    // Конструктор
    // ========================================================================

    Match3System::Match3System(
        NK::ECS::World& world,
        int rows,
        int cols,
        float cellSize,
        float pixelsPerUnit
    )
        : m_World(world)
        , m_Rows(rows)
        , m_Cols(cols)
        , m_CellSize(cellSize)
        , m_PixelsPerUnit(pixelsPerUnit)
        , m_Grid(rows, std::vector<TileType>(cols, -1))
    {
    }

    // ========================================================================
    // Жизненный цикл
    // ========================================================================

    void Match3System::Start()
    {
        // Спавним все 100 entity, заполняем случайными цветами.
        for (int r = 0; r < m_Rows; ++r)
        {
            for (int c = 0; c < m_Cols; ++c)
            {
                CreateTileEntity(r, c);
            }
        }
        FillRandom();
        NK_CORE_INFO("Match3System: spawned {0} tile entities in World", m_Rows * m_Cols);
    }

    void Match3System::Update(float /*deltaTime*/)
    {
        // Пока пусто — анимации/гравитация будут добавлены позже.
    }

    // ========================================================================
    // ECS-хелперы
    // ========================================================================

    std::string Match3System::TileEntityName(int row, int col) const
    {
        std::ostringstream oss;
        oss << Match3TileComponent::Prefix << row << "_" << col;
        return oss.str();
    }

    entt::entity Match3System::GetTileEntity(int row, int col) const
    {
        return m_World.GetEntityByName(TileEntityName(row, col));
    }

    glm::vec4 Match3System::GetColorForType(TileType type)
    {
        switch (type)
        {
            case 0:  return glm::vec4(1.0f, 0.2f, 0.2f, 1.0f);  // red
            case 1:  return glm::vec4(0.2f, 1.0f, 0.2f, 1.0f);  // green
            case 2:  return glm::vec4(0.2f, 0.4f, 1.0f, 1.0f);  // blue
            case 3:  return glm::vec4(1.0f, 0.95f, 0.2f, 1.0f); // yellow
            case 4:  return glm::vec4(1.0f, 0.3f, 1.0f, 1.0f);  // magenta
            case 5:  return glm::vec4(0.2f, 1.0f, 1.0f, 1.0f);  // cyan
            default: return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);  // empty (alpha 0)
        }
    }

    entt::entity Match3System::CreateTileEntity(int row, int col)
    {
        entt::entity e = m_World.CreateEntity(TileEntityName(row, col));

        // Transform: позиция = центр клетки
        auto& transform = m_World.AddComponent<NK::ECS::TransformComponent>(e);
        const glm::vec2 pos = GetCellPosition(row, col);
        transform.Position = glm::vec3(pos.x, pos.y, 0.0f);
        transform.Scale = glm::vec3(m_CellSize / m_PixelsPerUnit, m_CellSize / m_PixelsPerUnit, 1.0f);

        // Sprite: общая 1x1 текстура + цвет по типу (-1 = прозрачный, FillRandom обновит)
        auto& sprite = m_World.AddComponent<NK::ECS::SpriteComponent>(e);
        sprite.Texture = m_SpriteTexture;
        sprite.Color = GetColorForType(-1);

        // Tile: позиция в сетке + тип
        auto& tile = m_World.AddComponent<Match3TileComponent>(e);
        tile.Row = row;
        tile.Col = col;
        tile.Type = -1;

        return e;
    }

    void Match3System::UpdateTileEntity(int row, int col, TileType newType)
    {
        const entt::entity e = GetTileEntity(row, col);
        if (e == entt::null)
        {
            return;
        }

        if (m_World.HasComponent<Match3TileComponent>(e))
        {
            m_World.GetComponent<Match3TileComponent>(e).Type = newType;
        }
        if (m_World.HasComponent<NK::ECS::SpriteComponent>(e))
        {
            m_World.GetComponent<NK::ECS::SpriteComponent>(e).Color = GetColorForType(newType);
        }
    }

    void Match3System::DestroyTileEntity(int row, int col)
    {
        const entt::entity e = GetTileEntity(row, col);
        if (e != entt::null)
        {
            m_World.DestroyEntity(e);
        }
    }

    // ========================================================================
    // API для Lua (копия Match3Board-логики, но с обновлением entity)
    // ========================================================================

    void Match3System::FillRandom()
    {
        static std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> dist(0, 5);

        for (int r = 0; r < m_Rows; ++r)
        {
            for (int c = 0; c < m_Cols; ++c)
            {
                TileType type;
                bool valid = false;
                while (!valid)
                {
                    type = dist(rng);
                    if (c >= 2 && m_Grid[r][c - 1] == type && m_Grid[r][c - 2] == type)
                    {
                        continue;
                    }
                    if (r >= 2 && m_Grid[r - 1][c] == type && m_Grid[r - 2][c] == type)
                    {
                        continue;
                    }
                    valid = true;
                }
                m_Grid[r][c] = type;
                UpdateTileEntity(r, c, type);
                if (OnTileChanged)
                {
                    OnTileChanged(r, c, type);
                }
            }
        }
    }

    void Match3System::SetTile(int row, int col, TileType type)
    {
        if (!IsValidCell(row, col))
        {
            return;
        }
        m_Grid[row][col] = type;
        UpdateTileEntity(row, col, type);
        if (OnTileChanged)
        {
            OnTileChanged(row, col, type);
        }
    }

    Match3System::TileType Match3System::GetTile(int row, int col) const
    {
        if (!IsValidCell(row, col))
        {
            return -1;
        }
        return m_Grid[row][col];
    }

    bool Match3System::IsValidCell(int row, int col) const
    {
        return row >= 0 && row < m_Rows && col >= 0 && col < m_Cols;
    }

    bool Match3System::HasPossibleMoves()
    {
        for (int r = 0; r < m_Rows; ++r)
        {
            for (int c = 0; c < m_Cols; ++c)
            {
                if (m_Grid[r][c] == -1)
                {
                    continue;
                }
                if (c + 1 < m_Cols && m_Grid[r][c + 1] != -1)
                {
                    std::swap(m_Grid[r][c], m_Grid[r][c + 1]);
                    auto matches = FindMatches();
                    std::swap(m_Grid[r][c], m_Grid[r][c + 1]);
                    if (!matches.empty())
                    {
                        return true;
                    }
                }
                if (r + 1 < m_Rows && m_Grid[r + 1][c] != -1)
                {
                    std::swap(m_Grid[r][c], m_Grid[r + 1][c]);
                    auto matches = FindMatches();
                    std::swap(m_Grid[r][c], m_Grid[r + 1][c]);
                    if (!matches.empty())
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    glm::vec2 Match3System::GetCellPosition(int row, int col) const
    {
        const float x = (col + 0.5f) * m_CellSize / m_PixelsPerUnit;
        const float y = (row + 0.5f) * m_CellSize / m_PixelsPerUnit;
        return glm::vec2(x, y);
    }

    bool Match3System::Swap(int r1, int c1, int r2, int c2)
    {
        if (!IsValidCell(r1, c1) || !IsValidCell(r2, c2))
        {
            return false;
        }
        std::swap(m_Grid[r1][c1], m_Grid[r2][c2]);
        UpdateTileEntity(r1, c1, m_Grid[r1][c1]);
        UpdateTileEntity(r2, c2, m_Grid[r2][c2]);
        if (OnTileChanged)
        {
            OnTileChanged(r1, c1, m_Grid[r1][c1]);
            OnTileChanged(r2, c2, m_Grid[r2][c2]);
        }
        return true;
    }

    std::vector<std::pair<int, int>> Match3System::FindMatches() const
    {
        std::vector<std::pair<int, int>> matches;
        std::vector<std::vector<bool>> marked(m_Rows, std::vector<bool>(m_Cols, false));

        // Горизонтальные цепочки (≥3)
        for (int r = 0; r < m_Rows; ++r)
        {
            for (int c = 0; c < m_Cols - 2; ++c)
            {
                const int type = m_Grid[r][c];
                if (type == -1)
                {
                    continue;
                }
                if (m_Grid[r][c + 1] == type && m_Grid[r][c + 2] == type)
                {
                    marked[r][c] = true;
                    marked[r][c + 1] = true;
                    marked[r][c + 2] = true;
                    int nextC = c + 3;
                    while (nextC < m_Cols && m_Grid[r][nextC] == type)
                    {
                        marked[r][nextC] = true;
                        nextC++;
                    }
                }
            }
        }

        // Вертикальные цепочки (≥3)
        for (int c = 0; c < m_Cols; ++c)
        {
            for (int r = 0; r < m_Rows - 2; ++r)
            {
                const int type = m_Grid[r][c];
                if (type == -1)
                {
                    continue;
                }
                if (m_Grid[r + 1][c] == type && m_Grid[r + 2][c] == type)
                {
                    marked[r][c] = true;
                    marked[r + 1][c] = true;
                    marked[r + 2][c] = true;
                    int nextR = r + 3;
                    while (nextR < m_Rows && m_Grid[nextR][c] == type)
                    {
                        marked[nextR][c] = true;
                        nextR++;
                    }
                }
            }
        }

        for (int r = 0; r < m_Rows; ++r)
        {
            for (int c = 0; c < m_Cols; ++c)
            {
                if (marked[r][c])
                {
                    matches.emplace_back(r, c);
                }
            }
        }

        std::sort(matches.begin(), matches.end());
        return matches;
    }

    void Match3System::RemoveTiles(const std::vector<std::pair<int, int>>& cells)
    {
        for (const auto& cell : cells)
        {
            const int r = cell.first;
            const int c = cell.second;
            m_Grid[r][c] = -1;
            UpdateTileEntity(r, c, -1);
            if (OnTileChanged)
            {
                OnTileChanged(r, c, -1);
            }
        }
    }

    void Match3System::ApplyGravity()
    {
        for (int c = 0; c < m_Cols; ++c)
        {
            int writeRow = m_Rows - 1;
            for (int r = m_Rows - 1; r >= 0; --r)
            {
                if (m_Grid[r][c] != -1)
                {
                    if (r != writeRow)
                    {
                        m_Grid[writeRow][c] = m_Grid[r][c];
                        m_Grid[r][c] = -1;
                        UpdateTileEntity(writeRow, c, m_Grid[writeRow][c]);
                        UpdateTileEntity(r, c, -1);
                        if (OnTileChanged)
                        {
                            OnTileChanged(writeRow, c, m_Grid[writeRow][c]);
                            OnTileChanged(r, c, -1);
                        }
                    }
                    --writeRow;
                }
            }
        }
    }

    void Match3System::FillEmpty()
    {
        static std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> dist(0, 5);

        for (int r = 0; r < m_Rows; ++r)
        {
            for (int c = 0; c < m_Cols; ++c)
            {
                if (m_Grid[r][c] != -1)
                {
                    continue;
                }

                TileType type;
                int attempts = 0;
                const int maxAttempts = 16;
                do
                {
                    type = dist(rng);
                    bool wouldMatch = false;
                    if (c >= 2 && m_Grid[r][c - 1] == type && m_Grid[r][c - 2] == type)
                    {
                        wouldMatch = true;
                    }
                    if (!wouldMatch && r >= 2 && m_Grid[r - 1][c] == type && m_Grid[r - 2][c] == type)
                    {
                        wouldMatch = true;
                    }
                    if (!wouldMatch)
                    {
                        break;
                    }
                    ++attempts;
                } while (attempts < maxAttempts);

                m_Grid[r][c] = type;
                UpdateTileEntity(r, c, type);
                if (OnTileChanged)
                {
                    OnTileChanged(r, c, m_Grid[r][c]);
                }
            }
        }
    }

    void Match3System::Mix()
    {
        const int maxAttempts = 50;
        int attempts = 0;
        do
        {
            FillRandom();
            ++attempts;
        } while (!HasPossibleMoves() && attempts < maxAttempts);

        if (!HasPossibleMoves())
        {
            NK_CORE_WARN("Match3System::Mix: failed to find a state with possible moves after {0} attempts", maxAttempts);
        }
    }

} // namespace NK::Game::ECS
