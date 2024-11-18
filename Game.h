#pragma once

#include <vector>
#include <vector>
#include <utility>
#include <time.h>
#include <iostream>
#include <optional>
#include <chrono>


static const enum GameState {
	ongoing,
	playerWin,
	aiWin,
	tie
};

static enum AIDiff {
	easy,
	normal,
	hard
};

static const enum FieldOption {
	opt33,
	opt44,
	opt55
};

struct Cell {
	int row;
	int col;
	char val;
};

static struct Move {
	int row;
	int col;
	int score;
};


class Game {
private:
	const static int lineLenWinCond = 3;

	const static int INIT_ROWN = 3;
	const static int INIT_COLN = 3;

	int m_rowN = INIT_ROWN;
	int m_colN = INIT_COLN;

	const int MINIMAX_TIME_LIMIT = 5000; // Time in ms
	std::chrono::steady_clock::time_point m_minimaxStarted;

	AIDiff m_AIDiff = normal; // Default AI difficulty
	GameState lastState = ongoing;
	std::vector<std::vector<char>> m_field;

	Game();

	bool HorizLineCheck(const Cell& _cell) const;

	bool VertLineCheck(const Cell& _cell) const;

	bool DiagLinesCheck(const Cell& _cell) const;

	bool CheckWin(const Cell& _cell) const;

	const char GetVal(const int _row, const int _col) const;

	const Move RandomMove();

	Move BestMove();

	int Minimax(const int _depth, int _alpha, int _beta, const bool _isMaximizing);

	int GetSearchDepth() const;

	const int BlankCellsCount() const;

public:
	constexpr static char CELL_BLANK = ' ';
	constexpr static char CELL_X = 'X';
	constexpr static char CELL_O = 'O';

	constexpr const short RowN() const;

	constexpr const short ColN() const;

	static Game& GetGame();

	const std::vector<char>& operator[](const int _row) const;

	void ResetField();

	void Set(const Cell& _cell);

	GameState MakeAIMove();

	GameState GetGameState(std::optional<const Cell> _newCell = std::nullopt);

	AIDiff GetAIDiff() const;

	void SetAIDiff(const AIDiff _AIDiff);

	void SetFieldSize(const FieldOption _fieldOption);

	Game(const Game& other) = delete;
	Game(Game&& other) = delete;
	Game operator = (const Game& other) = delete;
	Game operator = (Game&& other) = delete;
};