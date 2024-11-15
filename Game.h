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
private:
	const static int lineLenWinCond = 3;

	const static int m_rowN = 3;
	const static int m_colN = 3;

	std::array<std::array<char, m_colN>, m_rowN> m_field;

	Game();

	bool HorizLineCheck(const Cell& _cell);

	bool VertLineCheck(const Cell& _cell);

	bool DiagLinesCheck(const Cell& _cell);

	bool CheckWin(const Cell& _cell);

	const char GetVal(const int _row, const int _col);

public:
	constexpr static char CELL_BLANK = ' ';
	constexpr static char CELL_X = 'X';
	constexpr static char CELL_O = 'O';

	GameState lastState = Ongoing;

	constexpr const short RowN();

	constexpr const short ColN();

	static Game& GetGame();

	const std::array<char, m_colN>& operator [] (const int _row);

	void ResetField();

	void Set(const Cell& _cell);

	GameState MakeAIMove(const AIDifficulty _AIDiff = AIDifficulty::Normal);

	GameState GetGameState(const Cell& _newCell);

	Game(const Game& other) = delete;
	Game(Game&& other) = delete;
	Game operator = (const Game& other) = delete;
	Game operator = (Game&& other) = delete;
};