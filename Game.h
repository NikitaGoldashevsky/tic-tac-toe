#pragma once

#include <array>
#include <vector>
#include <utility>
#include <stdexcept>
#include <time.h>


const static enum AIDifficulty {
	Easy,
	Normal,
	Harrd
};


class Game {
private:
	const static size_t rowN = 3;
	const static size_t colN = 3;

	std::array<std::array<char, colN>, rowN> m_field;

	inline Game() {
		ResetField();
	}

public:
	constexpr static char CELL_BLANK = ' ';
	constexpr static char CELL_X = 'X';
	constexpr static char CELL_O = 'O';

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

	const std::array<char, colN>& operator [] (const size_t _row) {
		return m_field[_row];
	}

	inline void ResetField() {
		for (int i = 0; i < RowN(); i++) {
			for (int j = 0; j < ColN(); j++) {
				Set(i, j, CELL_BLANK);
			}
		}
	}

	void Set(const size_t _row, const size_t _col, const char _val) {
		if (_val == CELL_BLANK || _val == CELL_X || _val == CELL_O) {
			m_field[_row][_col] = _val;
		}
		else {
			throw std::invalid_argument("");
		}
	}

	void MakeAIMove(const AIDifficulty _AIDiff = AIDifficulty::Normal) {
		// AI's moves are random (for now)

		const auto FPCountBlank = [this]() -> size_t {
			size_t blankCount = 0;
			for (int i = 0; i < RowN(); i++) {
				for (int j = 0; j < ColN(); j++) {
					if (this->operator[](i)[j] == CELL_BLANK)
						blankCount++;
				}
			}
			return blankCount;
		};

		const auto blankCount = FPCountBlank();
		if (blankCount == 0)
			return;
		
		srand(time(0));
		const int moveInd = rand() % blankCount;

		int curMoveInd = 0;
		for (size_t i = 0; i < RowN(); i++) {
			for (size_t j = 0; j < ColN(); j++) {
				if (operator [] (i)[j] == CELL_BLANK) {
					if (curMoveInd == moveInd) {
						Set(i, j, CELL_O);
						return;
					}
					else {
						curMoveInd++;
					}
				}
			}
		}
	}

	Game(const Game& other) = delete;
	Game(Game&& other) = delete;
	Game operator = (const Game& other) = delete;
	Game operator = (Game&& other) = delete;
};