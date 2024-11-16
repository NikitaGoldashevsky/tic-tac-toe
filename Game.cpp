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

	int udDiagLineLen = 1; // diagonal line like this \
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

	int duDiagLineLen = 1; // diagonal line like this /
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
	lastState = ongoing;
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
	//return MakeRandomMove(_AIDiff);
	Move bestMove = BestMove(_AIDiff);

	if (bestMove.row == -1 || bestMove.col == -1) {
		lastState = Tie;
		return Tie;
	}

	const Cell aiMove(bestMove.row, bestMove.col, CELL_O);
	Set(aiMove);
	return GetGameState(aiMove);
}

GameState Game::MakeRandomMove(const AIDifficulty _AIDiff) {
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

GameState Game::GetGameState(std::optional<const Cell> _newCell) {
	if (!_newCell.has_value()) {
		return lastState;
}

GameState Game::GetGameState(const Cell& _newCell) {
	const auto _cellVal = _newCell.val;
	const bool _won = CheckWin(_newCell);

	if (_won) {
		switch (_cellVal)
		{
		case CELL_X:
			lastState = playerWin;
			return lastState;
		case CELL_O:
			lastState = aiWin;
			return lastState;
		default:
			return ongoing;
		}
		}
	}
}

Move Game::BestMove(const AIDifficulty _AIDiff) {
	srand(time(0));
	Move curBestMove(-1, -1, -100);

	for (int i = 0; i < RowN(); i++) {
		for (int j = 0; j < ColN(); j++) {
			if (GetVal(i, j) == CELL_BLANK) {
				if (_AIDiff == Easy && (rand() % 100) < 70) {
					continue;
				}
				else if (_AIDiff == Normal && (rand() % 100) < 30) {
					continue;
				}

				Set(Cell(i, j, CELL_O));

				int moveScore = Minimax(0, false);

				Set(Cell(i, j, CELL_BLANK));

				if (moveScore > curBestMove.score) {
					curBestMove = Move(i, j, moveScore);
				}
			}
		}
	}

	return curBestMove;
}

int Game::Minimax(const int _depth, const bool _isMaximizing) {
	Cell lastMove(0, 0, _isMaximizing ? CELL_O : CELL_X);
	const int rowN = RowN();
	const int colN = ColN();

	for (int i = 0; i < rowN; i++) {
		for (int j = 0; j < colN; j++) {
			if (GetVal(i, j) == CELL_X) {
				lastMove = Cell(i, j, CELL_X);
				if (CheckWin(lastMove))
					return _depth - 10;
			}
		}
	}

	for (int i = 0; i < rowN; i++) {
		for (int j = 0; j < colN; j++) {
			if (GetVal(i, j) == CELL_O) {
				lastMove = Cell(i, j, CELL_O);
				if (CheckWin(lastMove))
					return 10 - _depth;
			}
		}
	}

	bool isFull = true;
	for (int i = 0; i < rowN; i++) {
		for (int j = 0; j < colN; j++) {
			if (GetVal(i, j) == CELL_BLANK) {
				isFull = false;
				break;
			}
		}
	}
	if (isFull) 
		return 0;

	if (_isMaximizing) {
		int bestScore = -1000;
		for (int i = 0; i < rowN; i++) {
			for (int j = 0; j < colN; j++) {
				if (GetVal(i, j) == CELL_BLANK) {
					Set(Cell(i, j, CELL_O));
					bestScore = std::max(bestScore, Minimax(_depth + 1, false));
					Set(Cell(i, j, CELL_BLANK));
				}
			}
		}
		return bestScore;
	}
	else {
		int bestScore = 1000;
		for (int i = 0; i < rowN; i++) {
			for (int j = 0; j < colN; j++) {
				if (GetVal(i, j) == CELL_BLANK) {
					Set(Cell(i, j, CELL_X));
					bestScore = std::min(bestScore, Minimax(_depth + 1, true));
					Set(Cell(i, j, CELL_BLANK));
				}
			}
		}
		return bestScore;
	}
}
