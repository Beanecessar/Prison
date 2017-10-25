#pragma once
#include "game.h"
#include <string>
#include <vector>
#include <mutex>

class solo_game :public game {
public:
	int compete(std::string stg1, std::string stg2);

	void comparator(int count, int all);

	void compete(std::string path);

private:
	std::vector<int> score;

	std::mutex lock;

	bool multi_threading = false;

protected:
	std::vector<std::string> strategies;

	void traversal_windows(const std::string path); //traversal given path and load all file into [strategies]
};

class gang_game :public solo_game {
public:
	void compete(std::string path);

	void set_spy(bool b);

private:
	bool spy_flag;
};