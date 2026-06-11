#include "Game/Match3Board.h"
#include <algorithm>
#include <random>

namespace NK {

	Match3Board::Match3Board(int rows, int cols, float cellSize, float pixelsPerUnit)
		: m_Rows(rows), m_Cols(cols), m_CellSize(cellSize), m_PixelsPerUnit(pixelsPerUnit),
		m_Grid(rows, std::vector<TileType>(cols, -1))
	{
	}

	enum class TileType : int {
		Normal0 = 0, Normal1 = 1, Normal2 = 2, Normal3 = 3, Normal4 = 4, Normal5 = 5,
		SpecialBomb = 100, SpecialRainbow = 101  // зарезервировано
	};

	void Match3Board::FillRandom() {
		static std::mt19937 rng(std::random_device{}());
		std::uniform_int_distribution<int> dist(0, 5);

		for (int r = 0; r < m_Rows; ++r) {
			for (int c = 0; c < m_Cols; ++c) {
				TileType type;
				bool valid = false;
				while (!valid) {
					type = dist(rng);
					// Проверяем горизонтальные тройки (слева)
					if (c >= 2 && m_Grid[r][c - 1] == type && m_Grid[r][c - 2] == type)
						continue;
					// Проверяем вертикальные тройки (сверху)
					if (r >= 2 && m_Grid[r - 1][c] == type && m_Grid[r - 2][c] == type)
						continue;
					valid = true;
				}
				m_Grid[r][c] = type;
				if (OnTileChanged) OnTileChanged(r, c, type);
			}
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

	bool Match3Board::HasPossibleMoves() {
		// Перебираем все клетки
		for (int r = 0; r < m_Rows; ++r) {
			for (int c = 0; c < m_Cols; ++c) {
				if (m_Grid[r][c] == -1) continue;
				// Проверяем обмен с соседом справа
				if (c + 1 < m_Cols && m_Grid[r][c + 1] != -1) {
					// Временный swap
					std::swap(m_Grid[r][c], m_Grid[r][c + 1]);
					auto matches = FindMatches();
					std::swap(m_Grid[r][c], m_Grid[r][c + 1]); // возвращаем обратно
					if (!matches.empty()) return true;
				}
				// Проверяем обмен с соседом снизу
				if (r + 1 < m_Rows && m_Grid[r + 1][c] != -1) {
					std::swap(m_Grid[r][c], m_Grid[r + 1][c]);
					auto matches = FindMatches();
					std::swap(m_Grid[r][c], m_Grid[r + 1][c]);
					if (!matches.empty()) return true;
				}
			}
		}
		return false;
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

		// Временная матрица флагов для отметки ячеек на удаление
		std::vector<std::vector<bool>> marked(m_Rows, std::vector<bool>(m_Cols, false));

		// 1. Горизонтальные цепочки (≥3)
		for (int r = 0; r < m_Rows; ++r) {
			for (int c = 0; c < m_Cols - 2; ++c) {
				int type = m_Grid[r][c];
				if (type == -1) continue;

				if (m_Grid[r][c + 1] == type && m_Grid[r][c + 2] == type) {
					marked[r][c] = true;
					marked[r][c + 1] = true;
					marked[r][c + 2] = true;

					// Расширяем цепочку вправо
					int nextC = c + 3;
					while (nextC < m_Cols && m_Grid[r][nextC] == type) {
						marked[r][nextC] = true;
						nextC++;
					}
				}
			}
		}

		// 2. Вертикальные цепочки (≥3)
		for (int c = 0; c < m_Cols; ++c) {
			for (int r = 0; r < m_Rows - 2; ++r) {
				int type = m_Grid[r][c];
				if (type == -1) continue;

				if (m_Grid[r + 1][c] == type && m_Grid[r + 2][c] == type) {
					marked[r][c] = true;
					marked[r + 1][c] = true;
					marked[r + 2][c] = true;

					// Расширяем цепочку вниз
					int nextR = r + 3;
					while (nextR < m_Rows && m_Grid[nextR][c] == type) {
						marked[nextR][c] = true;
						nextR++;
					}
				}
			}
		}

		// 3. Собираем все помеченные координаты
		for (int r = 0; r < m_Rows; ++r) {
			for (int c = 0; c < m_Cols; ++c) {
				if (marked[r][c]) {
					matches.emplace_back(r, c);
				}
			}
		}

		// Сортируем для предсказуемого порядка
		std::sort(matches.begin(), matches.end());
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

	void Match3Board::Mix() {
		do {
			// Заполняем заново, избегая начальных троек
			FillRandom();   // используйте вариант с проверкой троек (см. ниже)
		} while (!HasPossibleMoves());
	}
}