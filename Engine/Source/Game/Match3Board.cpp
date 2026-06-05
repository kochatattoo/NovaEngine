#include "Game/Match3Board.h"
#include <algorithm>
#include <random>

namespace NK {

	Match3Board::Match3Board(int rows, int cols, float cellSize, float pixelsPerUnit)
		: m_Rows(rows), m_Cols(cols), m_CellSize(cellSize), m_PixelsPerUnit(pixelsPerUnit),
		m_Grid(rows, std::vector<TileType>(cols, -1))
	{
	}

	void Match3Board::FillRandom() {
		static std::mt19937 rng(std::random_device{}());
		std::uniform_int_distribution<int> dist(0, 5);
		for (int r = 0; r < m_Rows; ++r)
			for (int c = 0; c < m_Cols; ++c) {
				m_Grid[r][c] = dist(rng);
				if (OnTileChanged) OnTileChanged(r, c, m_Grid[r][c]);
			}
	}

	void Match3Board::SetTile(int row, int col, TileType type) {
		if (!IsValidCell(row, col)) return;
		m_Grid[row][col] = type;
		if (OnTileChanged) OnTileChanged(row, col, type);
	}

	Match3Board::TileType Match3Board::GetTile(int row, int col) const {
		if (!IsValidCell(row, col)) return -1;
		return m_Grid[row][col];
	}

	bool Match3Board::IsValidCell(int row, int col) const {
		return row >= 0 && row < m_Rows && col >= 0 && col < m_Cols;
	}

	glm::vec2 Match3Board::GetCellPosition(int row, int col) const {
		// Центр ячейки: отступ + (col+0.5)*cellSize, (row+0.5)*cellSize
		float x = (col + 0.5f) * m_CellSize / m_PixelsPerUnit;
		float y = (row + 0.5f) * m_CellSize / m_PixelsPerUnit;
		return glm::vec2(x, y);
	}

	bool Match3Board::Swap(int r1, int c1, int r2, int c2) {
		if (!IsValidCell(r1, c1) || !IsValidCell(r2, c2)) return false;
		// Обмен
		std::swap(m_Grid[r1][c1], m_Grid[r2][c2]);
		if (OnTileChanged) {
			OnTileChanged(r1, c1, m_Grid[r1][c1]);
			OnTileChanged(r2, c2, m_Grid[r2][c2]);
		}
		return true;
	}

	std::vector<std::pair<int, int>> Match3Board::FindMatches() const {
		std::vector<std::pair<int, int>> matches;
		// Горизонтальные
		for (int r = 0; r < m_Rows; ++r) {
			for (int c = 0; c < m_Cols - 2; ++c) {
				if (m_Grid[r][c] != -1 &&
					m_Grid[r][c] == m_Grid[r][c + 1] &&
					m_Grid[r][c] == m_Grid[r][c + 2])
				{
					matches.emplace_back(r, c);
					matches.emplace_back(r, c + 1);
					matches.emplace_back(r, c + 2);
				}
			}
		}
		// Вертикальные
		for (int c = 0; c < m_Cols; ++c) {
			for (int r = 0; r < m_Rows - 2; ++r) {
				if (m_Grid[r][c] != -1 &&
					m_Grid[r][c] == m_Grid[r + 1][c] &&
					m_Grid[r][c] == m_Grid[r + 2][c])
				{
					matches.emplace_back(r, c);
					matches.emplace_back(r + 1, c);
					matches.emplace_back(r + 2, c);
				}
			}
		}
		// Уникальность
		std::sort(matches.begin(), matches.end());
		matches.erase(std::unique(matches.begin(), matches.end()), matches.end());
		return matches;
	}

	void Match3Board::RemoveTiles(const std::vector<std::pair<int, int>>& cells) {
		for (auto [r, c] : cells) {
			m_Grid[r][c] = -1;
			if (OnTileChanged) OnTileChanged(r, c, -1);
		}
	}

	void Match3Board::ApplyGravity() {
		for (int c = 0; c < m_Cols; ++c) {
			int writeRow = m_Rows - 1;
			for (int r = m_Rows - 1; r >= 0; --r) {
				if (m_Grid[r][c] != -1) {
					if (r != writeRow) {
						m_Grid[writeRow][c] = m_Grid[r][c];
						m_Grid[r][c] = -1;
						if (OnTileChanged) {
							OnTileChanged(writeRow, c, m_Grid[writeRow][c]);
							OnTileChanged(r, c, -1);
						}
					}
					--writeRow;
				}
			}
		}
	}

	void Match3Board::FillEmpty() {
		static std::mt19937 rng(std::random_device{}());
		std::uniform_int_distribution<int> dist(0, 5);
		for (int r = 0; r < m_Rows; ++r)
			for (int c = 0; c < m_Cols; ++c) {
				if (m_Grid[r][c] == -1) {
					m_Grid[r][c] = dist(rng);
					if (OnTileChanged) OnTileChanged(r, c, m_Grid[r][c]);
				}
			}
	}

}