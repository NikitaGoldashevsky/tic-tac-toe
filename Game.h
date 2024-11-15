#pragma once

#include <array>
#include <vector>
#include <utility>
#include <stdexcept>
#include <time.h>
#include <iostream>


const static enum AIDifficulty {
	Easy,
	Normal,
	Harrd
};

const static enum GameState {
	Ongoing,
	PlayerWin,
	AIWin,
	Tie
};

const struct Cell {
	const int row;
	const int col;
	const char val;
};

class Game {
public:
	constexpr static char CELL_BLANK = ' ';
	constexpr static char CELL_X = 'X';
	constexpr static char CELL_O = 'O';

private:
	const static int lineLenWinCond = 3;

	const static int m_rowN = 3;
	const static int m_colN = 3;

	std::array<std::array<char, m_colN>, m_rowN> m_field;

	inline Game() {
		ResetField();
	}

	bool HorizLineCheck(const Cell& _cell) {
		const auto [_row, _col, _val] = _cell;
		int horizLineLen = 1;

		// to right
		for (int c = _col + 1; c < ColN(); c++) {
			if (GetVal(_row, c) == _val) horizLineLen++;
			else break;
		}
		// to left
		for (int c = _col - 1; c >= 0; c--) {
			if (GetVal(_row, c) == _val) horizLineLen++;
			else break;
		}
		return horizLineLen >= lineLenWinCond;
	}

	bool VertLineCheck(const Cell& _cell) {
		const auto [_row, _col, _val] = _cell;
		int vertLineLen = 1;

		// to down
		for (int r = _row + 1; r < RowN(); r++) {
			if (GetVal(r, _col) == _val) vertLineLen++;
			else break;
		}
		// to up
		for (int r = _row - 1; r >= 0; r--) {
			if (GetVal(r, _col) == _val) vertLineLen++;
			else break;
		}
		return vertLineLen >= lineLenWinCond;
	}

	bool DiagLinesCheck(const Cell& _cell) {
		const auto [_row, _col, _val] = _cell;

		int udDiagLineLen = 1; // diagonal like this \
		// to right-down
		for (int r = _row + 1, c = _col + 1; r < RowN() && c < ColN(); r++, c++) {
			if (GetVal(r, c) == _val) udDiagLineLen++;
			else break;
		}
		// to left-up
		for (int r = _row - 1, c = _col - 1; r >= 0 && c >= 0; r--, c--) {
			if (GetVal(r, c) == _val) udDiagLineLen++;
			else break;
		}
		if (udDiagLineLen > lineLenWinCond) return true;

		int duDiagLineLen = 1; // diagonal like this /
		// to left-down
		for (int r = _row + 1, c = _col - 1; r < RowN() && c >= 0; r++, c--) {
			if (GetVal(r, c) == _val) duDiagLineLen++;
			else break;
		}
		// to right-up
		for (int r = _row - 1, c = _col + 1; r >= 0 && c < ColN(); r--, c++) {
			if (GetVal(r, c) == _val) duDiagLineLen++;
			else break;
		}
		if (duDiagLineLen > lineLenWinCond) return true;

		return false;
	}

	bool CheckWin(const Cell& _cell) {
		return HorizLineCheck(_cell) || 
			VertLineCheck(_cell) || 
			DiagLinesCheck(_cell);
	}

	const char GetVal(const int _row, const int _col) {
		return operator[](_row)[_col];
	}

public:
	GameState lastState = Ongoing;

	inline const short RowN() {
		return m_rowN;
	}

	inline const short ColN() {
		return m_colN;
	}

	static Game& GetGame() {
		static Game m_instance = Game();
		return m_instance;
	}

	const std::array<char, m_colN>& operator [] (const int _row) {
		return m_field[_row];
	}

	inline void ResetField() {
		for (int i = 0; i < RowN(); i++) {
			for (int j = 0; j < ColN(); j++) {
				Set(Cell(i, j, CELL_BLANK));
			}
		}
		lastState = Ongoing;
	}

	void Set(const Cell& _cell) {
		const auto _row = _cell.row;
		const auto _col = _cell.col;
		const auto _val = _cell.val;

		if (_val == CELL_BLANK || _val == CELL_X || _val == CELL_O) {
			m_field[_row][_col] = _val;
		}
		else {
			throw std::invalid_argument("");
		}
	}

	GameState MakeAIMove(const AIDifficulty _AIDiff = AIDifficulty::Normal) {
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
		if (blankCount == 0) {
			lastState = Tie;
			return Tie;
		}
		
		srand(time(0));
		const int blankInd = rand() % blankCount;

		int curBlankInd = 0;
		for (size_t i = 0; i < RowN(); i++) {
			for (size_t j = 0; j < ColN(); j++) {
				if (GetVal(i, j) == CELL_BLANK) {
					if (curBlankInd == blankInd) {
						Cell newCell(i, j, CELL_O);
						Set(newCell);
						return GetGameState(newCell);
					}
					else {
						curBlankInd++;
					}
				}
			}
		}
	}

	GameState GetGameState(const Cell& _newCell) {
		const auto _cellVal = _newCell.val;
		const bool _won = CheckWin(_newCell);

		if (_won) {
			switch (_cellVal)
			{
			case CELL_X:
				lastState = PlayerWin;
				return lastState;
			case CELL_O:
				lastState = AIWin;
				return lastState;
			default:
				return Ongoing;
			}
		}
	}

	Game(const Game& other) = delete;
	Game(Game&& other) = delete;
	Game operator = (const Game& other) = delete;
	Game operator = (Game&& other) = delete;
};