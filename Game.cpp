#include "Game.h"

Game::Game() {
	ResetField();
}

bool Game::HorizLineCheck(const Cell& _cell) const {
	const auto [_row, _col, _val] = _cell;
	int horizLineLen = 1;
	const int rowN = RowN();
	const int colN = ColN();

	// to right
	for (int c = _col + 1; c < colN; c++) {
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

bool Game::VertLineCheck(const Cell& _cell) const {
	const auto [_row, _col, _val] = _cell;
	int vertLineLen = 1;
	const int rowN = RowN();
	const int colN = ColN();

	// to down
	for (int r = _row + 1; r < rowN; r++) {
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

bool Game::DiagLinesCheck(const Cell& _cell) const {
	const auto [_row, _col, _val] = _cell;
	const int rowN = RowN();
	const int colN = ColN();

	int udDiagLineLen = 1; // diagonal like this \
	// to right-down
	for (int r = _row + 1, c = _col + 1; r < rowN && c < colN; r++, c++) {
		if (GetVal(r, c) == _val) udDiagLineLen++;
		else break;
	}
	// to left-up
	for (int r = _row - 1, c = _col - 1; r >= 0 && c >= 0; r--, c--) {
		if (GetVal(r, c) == _val) udDiagLineLen++;
		else break;
	}
	if (udDiagLineLen >= lineLenWinCond) return true;

	int duDiagLineLen = 1; // diagonal like this /
	// to left-down
	for (int r = _row + 1, c = _col - 1; r < rowN && c >= 0; r++, c--) {
		if (GetVal(r, c) == _val) duDiagLineLen++;
		else break;
	}
	// to right-up
	for (int r = _row - 1, c = _col + 1; r >= 0 && c < colN; r--, c++) {
		if (GetVal(r, c) == _val) duDiagLineLen++;
		else break;
	}
	if (duDiagLineLen >= lineLenWinCond) return true;

	return false;
}

bool Game::CheckWin(const Cell& _cell) const {
	return HorizLineCheck(_cell) ||
		VertLineCheck(_cell) ||
		DiagLinesCheck(_cell);
}

const char Game::GetVal(const int _row, const int _col) const {
	return operator[](_row)[_col];
}

constexpr const short Game::RowN() const {
	return m_rowN;
}

constexpr const short Game::ColN() const {
	return m_colN;
}

Game& Game::GetGame() {
	static Game m_instance = Game();
	return m_instance;
}

const std::array<char, Game::m_colN>& Game::operator [] (const int _row) const {
	return m_field[_row];
}

void Game::ResetField() {
	for (int i = 0; i < RowN(); i++) {
		for (int j = 0; j < ColN(); j++) {
			Set(Cell(i, j, CELL_BLANK));
		}
	}
	lastState = Ongoing;
}

void Game::Set(const Cell& _cell) {
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

GameState Game::MakeAIMove(const AIDifficulty _AIDiff) {
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

GameState Game::GetGameState(const Cell& _newCell) {
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

