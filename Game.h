#pragma once

#include <array>
#include <vector>
#include <utility>

constinit const static size_t rowN = 3;
constinit const static size_t colN = 3;


class Game {
private:
	std::array<std::array<char, colN>, rowN> m_field;

	inline Game() {
		for (int i = 0; i < rowN; i++) {
			for (int j = 0; j < colN; j++) {
				m_field[i][j] = ' ';
			}
		}
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

	Game(const Game& other) = delete;
	Game(Game&& other) = delete;
	Game operator = (const Game& other) = delete;
	Game operator = (Game&& other) = delete;
};