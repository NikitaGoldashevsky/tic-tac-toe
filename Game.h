#pragma once

#include <array>
#include <vector>
#include <utility>
#include <stdexcept>
#include <time.h>
#include <iostream>


static const enum AIDifficulty {
	Easy,
	Normal,
	Harrd
};

static const enum GameState {
	Ongoing,
	PlayerWin,
	AIWin,
	Tie
};

static const struct Cell {
	const int row;
	const int col;
	const char val;
};

static struct Move {
	const int row;
	const int col;
	const int score;
};


class Game {
private:
	const static int lineLenWinCond = 3;

	const static int m_rowN = 3;
	const static int m_colN = 3;

	std::array<std::array<char, m_colN>, m_rowN> m_field;

	Game();

	bool HorizLineCheck(const Cell& _cell) const;

	bool VertLineCheck(const Cell& _cell) const;

	bool DiagLinesCheck(const Cell& _cell) const;

	bool CheckWin(const Cell& _cell) const;

	const char GetVal(const int _row, const int _col) const;

	GameState MakeRandomMove(const AIDifficulty _AIDiff = AIDifficulty::Normal);

public:
	constexpr static char CELL_BLANK = ' ';
	constexpr static char CELL_X = 'X';
	constexpr static char CELL_O = 'O';

	GameState lastState = Ongoing;

	constexpr const short RowN() const;

	constexpr const short ColN() const;

	static Game& GetGame();

	const std::array<char, Game::m_colN>& operator[](const int _row) const;

	void ResetField();

	void Set(const Cell& _cell);

	GameState MakeAIMove(const AIDifficulty _AIDiff = AIDifficulty::Normal);

	GameState GetGameState(const Cell& _newCell);

	Game(const Game& other) = delete;
	Game(Game&& other) = delete;
	Game operator = (const Game& other) = delete;
	Game operator = (Game&& other) = delete;
};