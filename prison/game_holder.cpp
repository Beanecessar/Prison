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
	solo_game_status p1_status, p2_status;
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

	std::cout << "prisoner1: " << stg1 << " | " << "prisoner2: " << stg2 << std::endl;
	std::cout << "prisoner1: " << p1_status.myscore << " | ";
	std::cout << "prisoner2: " << p2_status.myscore << " | ";

	if (p1_status.myscore < p2_status.myscore) {
		std::cout << "prison1 win." << std::endl;
		return 1;
	}
	else if (p1_status.myscore > p2_status.myscore) {
		std::cout << "prison2 win." << std::endl;
		return 2;
	}
	else {
		std::cout << "draw." << std::endl;
		return 0;
	}
}

void solo_game::comparator(int count, int all) {
	std::string stg1, stg2; 
	solo_game_status p1_status, p2_status;

	prisoner prisoner1(&p1_status);
	prisoner prisoner2(&p2_status);

	for (size_t i = count; i < strategies.size() - 1; i += all) {
		for (size_t j = i + 1; j < strategies.size(); ++j) {
			stg1 = strategies[i];
			stg2 = strategies[j];
			prisoner1.read_strategy(stg1);
			prisoner2.read_strategy(stg2);
			if (prisoner1.ready() && prisoner2.ready()) {
				for (unsigned i = 0; i < 200; ++i) {
					prisoner1.execute_strategy();
					prisoner2.execute_strategy();

					prisoner1.feedback(prisoner2.output());
					prisoner2.feedback(prisoner1.output());
				}
			}
			else
				continue;

			std::lock_guard<std::mutex> guard(lock);

			score[i] += p1_status.myscore;
			score[j] += p2_status.myscore;
		}
	}
}

void solo_game::compete(std::string path) {
	int best;
	float max_score;

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

	multi_threading = false;
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

///////////////////////////////////////////////////////////////////////

void gang_game::compete(std::string path) {
	gang_game_status g1_status, g2_status;
	gang_member g1_member[5], g2_member[5];
	gang purple(&g1_status), magenta(&g2_status);
	int spy_pos;

	srand((unsigned)time(NULL));

	traversal_windows(path);

	if (strategies.size() == 10) {
		for (unsigned i = 0; i < 5; ++i) {
			g1_member[i].set_status(&g1_status);
			g1_member[i].read_strategy(strategies[i]);
			g2_member[i].set_status(&g2_status);
			g2_member[i].read_strategy(strategies[i + 5]);
		}

		purple.set_all_member(g1_member);
		magenta.set_all_member(g2_member);

		if (rand() % 100 < possibility) {
			spy_pos = rand() % 2;
		}

		if (spy_flag) {
			for (unsigned i = 0; i < 200; ++i) {
				if (change_flag) {
					if (rand() % 8 < 3)
						find_flag = true;
					else
						find_flag = false;
				}
				else {
					if (rand() % 4 == 0)
						find_flag = true;
					else
						find_flag = false;
				}

				purple.execute_strategies();
				magenta.execute_strategies();

				if (spy_pos == 0) {
					if (g1_status.betray_num > g1_status.silence_num) {
						g1_status.betray_num += 1;
						g1_status.silence_num -= 1;
					}
					else {
						g1_status.betray_num -= 1;
						g1_status.silence_num += 1;
					}
				}
				else {
					if (g2_status.betray_num > g2_status.silence_num) {
						g2_status.betray_num += 1;
						g2_status.silence_num -= 1;
					}
					else {
						g2_status.betray_num -= 1;
						g2_status.silence_num += 1;
					}
				}

				purple.feedback(&g2_status);
				magenta.feedback(&g1_status);
			}
		}
		else {
			for (unsigned i = 0; i < 200; ++i) {
				purple.execute_strategies();
				magenta.execute_strategies();

				purple.feedback(&g2_status);
				magenta.feedback(&g1_status);
			}
			std::cout << "purple hand gang: " << g1_status.myscore << " | magenta hand gang: " << g2_status.myscore << std::endl;
		}
		
	}
	else
		std::cout << "invalid strategies path, too much or less file." << std::endl;
}

void gang_game::set_spy(bool b) {
	spy_flag = b;
}

void gang_game::set_spy(bool b, int p) {
	spy_flag = b;
	possibility = p;
}