#pragma once
#include "game.h"
#include <string>
#include <vector>
#include <mutex>

class solo_game :public game {
public:
	int compete(std::string stg1, std::string stg2);

	void comparator(int count, int all);

	void compete(char* path);

protected:
	game_status status;
	std::vector<std::string> strategies;
	std::vector<int> score;
	std::mutex lock;
	bool multi_threading = false;

	void traversal_windows(const std::string path);
};

class gang_game :public solo_game {
public:

};