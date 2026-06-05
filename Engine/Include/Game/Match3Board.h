#pragma once
#include <vector>
#include <functional>
#include <glm/glm.hpp>

namespace NK {

	class Match3Board {
	public:
		using TileType = int;
		using Grid = std::vector<std::vector<TileType>>;

		Match3Board(int rows, int cols, float cellSize = 64.0f, float pixelsPerUnit = 100.0f);

		void FillRandom();                         // заполнить случайными плитками (0..5)
		void SetTile(int row, int col, TileType type);
		TileType GetTile(int row, int col) const;
		bool IsValidCell(int row, int col) const;

		// Базовые операции
		bool Swap(int r1, int c1, int r2, int c2);  // вернёт true, если обмен возможен
		std::vector<std::pair<int, int>> FindMatches() const; // возвращает координаты совпавших плиток
		void RemoveTiles(const std::vector<std::pair<int, int>>& cells);
		void ApplyGravity();                         // сдвигает плитки вниз
		void FillEmpty();                            // заполняет пустые клетки новыми случайными плитками

		int GetRows() const { return m_Rows; }
		int GetCols() const { return m_Cols; }
		float GetCellSize() const { return m_CellSize; }
		glm::vec2 GetCellPosition(int row, int col) const; // мировая позиция центра ячейки
		float GetPixelsPerUnit() const { return m_PixelsPerUnit; }

		// Коллбек, вызываемый при изменении доски (для обновления визуала)
		std::function<void(int row, int col, TileType newType)> OnTileChanged;

	private:
		int m_Rows, m_Cols;
		float m_CellSize;
		float m_PixelsPerUnit;
		Grid m_Grid;
	};

}
