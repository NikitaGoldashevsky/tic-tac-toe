#pragma once

#include <array>
#include <vector>
#include <utility>
#include <stdexcept>


constinit const static size_t rowN = 3;
constinit const static size_t colN = 3;


class Game {
public:
	constexpr static char CELL_BLANK = ' ';
	constexpr static char CELL_CROSS = 'X';
	constexpr static char CELL_ZERO = 'O';

private:
	std::array<std::array<char, colN>, rowN> m_field;

	inline Game() {
		ResetField();
	}

public:
	inline const short RowN() {
		return rowN;
	}

	inline const short ColN() {
		return colN;
	}

	static Game& GetGame() {
		static Game m_instance = Game();
		return m_instance;
	}

	const std::array<char, colN>& operator [] (const size_t row) {
		return m_field[row];
	}

	inline void ResetField() {
		for (int i = 0; i < rowN; i++) {
			for (int j = 0; j < colN; j++) {
				Set(i, j, CELL_BLANK);
			}
		}
	}

	void Set(const size_t row, const size_t col, const char val) {
		if (val == CELL_BLANK || val == CELL_CROSS || val == CELL_ZERO) {
			m_field[row][col] = val;
		}
		else {
			throw std::invalid_argument("");
		}
	}

	Game(const Game& other) = delete;
	Game(Game&& other) = delete;
	Game operator = (const Game& other) = delete;
	Game operator = (Game&& other) = delete;
};