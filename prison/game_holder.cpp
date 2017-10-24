#include "stdafx.h"
#include "game_holder.h"
#include "interpretator.h"
#include <string>
#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <io.h>

int solo_game::compete(std::string stg1, std::string stg2) {
	game_status p1_status, p2_status;
	prisoner prisoner1(stg1, &p1_status);
	prisoner prisoner2(stg1, &p2_status);

	srand((unsigned)time(NULL));

	for (int i = 0; i < 200; i++)
	{
		prisoner1.execute_strategy();
		prisoner2.execute_strategy();

		
		prisoner1.feedback(prisoner2.output());
		prisoner2.feedback(prisoner1.output());
	}

	if (!multi_threading) {
		std::cout << "prisoner1: " << stg1 << " | " << "prisoner2: " << stg2 << std::endl;
		std::cout << "prisoner1: " << p1_status.myscore << " | ";
		std::cout << "prisoner2: " << p2_status.myscore << " | ";
	}

	if (p1_status.myscore < p2_status.myscore) {
		if (!multi_threading) {
			std::cout << "prison1 win." << std::endl;
		}
		return 1;
	}
	else if (p1_status.myscore > p2_status.myscore) {
		if (!multi_threading) {
			std::cout << "prison2 win." << std::endl;
		}
		return 2;
	}
	else {
		if (!multi_threading) {
			std::cout << "draw." << std::endl;
		}
		return 0;
	}
}

void solo_game::comparator(int count, int all) {
	int temp;
	std::string stg1, stg2;

	for (size_t i = count; i < strategies.size() - 1; i += all) {
		for (size_t j = i + 1; j < strategies.size(); ++j) {
			stg1 = strategies[i];
			stg2 = strategies[j];
			temp = compete(stg1, stg2);

			std::lock_guard<std::mutex> guard(lock);

			switch (temp)
			{
			case 0:
				score[i] += 1;
				score[j] += 1;
				break;
			case 1:
				score[i] += 2;
				break;
			case 2:
				score[j] += 2;
				break;
			default:
				break;
			}
		}
	}
}

void solo_game::compete(char* path) {
	int best, max_score;

	traversal_windows(path);

	score.resize(strategies.size(), 0);

	multi_threading = true;

	std::thread t0(&solo_game::comparator, this, 0, 8);
	std::thread t1(&solo_game::comparator, this, 1, 8);
	std::thread t2(&solo_game::comparator, this, 2, 8);
	std::thread t3(&solo_game::comparator, this, 3, 8);
	std::thread t4(&solo_game::comparator, this, 4, 8);
	std::thread t5(&solo_game::comparator, this, 5, 8);
	std::thread t6(&solo_game::comparator, this, 6, 8);
	std::thread t7(&solo_game::comparator, this, 7, 8);

	t0.join();
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	t6.join();
	t7.join();

	max_score = 0;
	best = 0;
	for (unsigned i = 0; i < score.size(); i++) {
		if (score[i] > max_score) {
			max_score = score[i];
			best = i;
		}
	}
	std::cout << "best strategy is " << strategies[best] << std::endl;
}

void solo_game::traversal_windows(const std::string path) {
	std::string temp = path;
	temp += "*";
	long handle;
	struct _finddata_t fileinfo;
	handle = _findfirst(temp.c_str(), &fileinfo);
	if (!(handle == -1l)) {
		if (fileinfo.name[0] != '.')
			strategies.push_back(path + fileinfo.name);
	}
	while (!_findnext(handle, &fileinfo)) {
		if (fileinfo.name[0] != '.')
			strategies.push_back(path + fileinfo.name);
	}
}