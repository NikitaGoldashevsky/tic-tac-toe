#include "pch.h"

Game::Game() {
	ResetField();
}

bool Game::HorizLineCheck(const Cell& _cell) const {
	const auto [_row, _col, _val] = _cell;
	size_t horizLineLen = 1;
	const size_t rowN = RowN();
	const size_t colN = ColN();

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
	size_t vertLineLen = 1;
	const size_t rowN = RowN();
	const size_t colN = ColN();

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
	const size_t rowN = RowN();
	const size_t colN = ColN();

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

const char Game::GetVal(const size_t _row, const size_t _col) const {
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

const std::vector<char>& Game::operator [] (const size_t _row) const {
	return m_field[_row];
}

void Game::ResetField() {
	m_field = std::vector<std::vector<char>>(
		RowN(), std::vector<char>(ColN(), CELL_BLANK)
	);

	for (size_t i = 0; i < RowN(); i++) {
		for (size_t j = 0; j < ColN(); j++) {
			Set(Cell(i, j, CELL_BLANK));
		}
	}
	lastState = ongoing;
}

void Game::Set(const Cell& _cell) {
	const auto [_row, _col, _val] = _cell;
	m_field[_row][_col] = _val;
}

GameState Game::MakeAIMove() {
	if (BlankCellsCount() == 0) {
		lastState = tie;
		return lastState;
	}

	m_minimaxStarted = std::chrono::high_resolution_clock::now();
	Move move = BestMove();

	const Cell moveCell(move.row, move.col, CELL_O);
	Set(moveCell);
	return GetGameState(moveCell);
}

size_t Game::BlankCellsCount() const {
	int blankCount = 0;
	for (size_t i = 0; i < RowN(); i++) {
		for (size_t j = 0; j < ColN(); j++) {
			if (GetVal(i, j) == CELL_BLANK)
				blankCount++;
		}
	}
	return blankCount;
}

const Move Game::RandomMove() {
	srand(time(0));
	const size_t blankInd = rand() % BlankCellsCount();

	int curBlankInd = 0;
	for (size_t i = 0; i < RowN(); i++) {
		for (size_t j = 0; j < ColN(); j++) {
			if (GetVal(i, j) == CELL_BLANK) {
				if (curBlankInd == blankInd) {
					return Move(i, j, 0);
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

	const char _cellVal = _newCell.value().val;
	const bool _won = CheckWin(_newCell.value());

	if (_won) {
		switch (_cellVal)
		{
		case CELL_X:
			lastState = playerWin;
			return lastState;
		case CELL_O:
			lastState = aiWin;
			return lastState;
		}
	}

	return BlankCellsCount() == 0 ? tie : ongoing;
}

AIDiff Game::GetAIDiff() const {
	return m_AIDiff;
}

void Game::SetAIDiff(const AIDiff _AIDiff) {
	m_AIDiff = _AIDiff;
}

void Game::SetFieldSize(const FieldOption _fieldOption) {
	switch (_fieldOption)
	{
	case opt33:
		m_rowN = 3; m_colN = 3;
		break;
	case opt44:
		m_rowN = 4; m_colN = 4;
		break;
	case opt55:
		m_rowN = 5; m_colN = 5;
		break;
	}
	ResetField();
}

Move Game::BestMove() {
	srand(time(0));
	Move curBestMove(-1, -1, -1000);

	for (size_t i = 0; i < RowN(); i++) {
		for (size_t j = 0; j < ColN(); j++) {
			if (GetVal(i, j) == CELL_BLANK) {
				if (m_AIDiff == easy && (rand() % 100) < 70) {
					continue;
				}
				else if (m_AIDiff == normal && (rand() % 100) < 30) {
					continue;
				}

				Set(Cell(i, j, CELL_O));
				int moveScore = Minimax(0, INT_MIN, INT_MAX, false);
				Set(Cell(i, j, CELL_BLANK));

				if (moveScore > curBestMove.score) {
					curBestMove = Move(i, j, moveScore);
				}
			}
		}
	}

	return curBestMove.score != -1000 ? curBestMove : RandomMove();
}

size_t Game::GetSearchDepth() const {
	const size_t blankCellsNum = BlankCellsCount();
	if (blankCellsNum > 12) return 3;
	if (blankCellsNum > 8) return 4;
	if (blankCellsNum > 4) return 6;
	return 8;
}

int Game::Minimax(const size_t _depth, int _alpha, int _beta, const bool _isMaximizing) {
	auto now = std::chrono::high_resolution_clock::now();
	if (std::chrono::duration_cast<std::chrono::milliseconds>(now - m_minimaxStarted).count() > MINIMAX_TIME_LIMIT)
		return _isMaximizing ? -1000 : 1000;
	if (_depth >= GetSearchDepth()) 
		return _isMaximizing ? -1000 : 1000;

	Cell lastMove(0, 0, _isMaximizing ? CELL_O : CELL_X);
	const size_t rowN = RowN();
	const size_t colN = ColN();

	for (size_t i = 0; i < rowN; i++) {
		for (size_t j = 0; j < colN; j++) {
			if (GetVal(i, j) == CELL_X) {
				lastMove = Cell(i, j, CELL_X);
				if (CheckWin(lastMove))
					return _depth - 10;
			}
		}
	}

	for (size_t i = 0; i < rowN; i++) {
		for (size_t j = 0; j < colN; j++) {
			if (GetVal(i, j) == CELL_O) {
				lastMove = Cell(i, j, CELL_O);
				if (CheckWin(lastMove))
					return 10 - _depth;
			}
		}
	}

	bool isFull = true;
	for (size_t i = 0; i < rowN; i++) {
		for (size_t j = 0; j < colN; j++) {
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
		for (size_t i = 0; i < rowN; i++) {
			for (size_t j = 0; j < colN; j++) {
				if (GetVal(i, j) == CELL_BLANK) {
					Set(Cell(i, j, CELL_O));
					const int score = Minimax(_depth + 1, _alpha, _beta, false);
					Set(Cell(i, j, CELL_BLANK));

					bestScore = std::max(bestScore, score);

					_alpha = std::max(_alpha, score);
					if (_beta <= _alpha) {
						return bestScore;
					}
				}
			}
		}
		return bestScore;
	}
	else {
		int bestScore = 1000;
		for (size_t i = 0; i < rowN; i++) {
			for (size_t j = 0; j < colN; j++) {
				if (GetVal(i, j) == CELL_BLANK) {
					Set(Cell(i, j, CELL_X));
					const int score = Minimax(_depth + 1, _alpha, _beta, true);
					Set(Cell(i, j, CELL_BLANK));

					bestScore = std::min(bestScore, score);

					_beta = std::min(_beta, score);
					if (_beta <= _alpha) {
						return bestScore;
					}
				}
			}
		}
		return bestScore;
	}
}
