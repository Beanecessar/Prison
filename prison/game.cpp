#include "stdafx.h"
#include "game.h"

solo_game_status::solo_game_status() {
	last_outcome = OUTS::N;
	all_outcomes_W = 0;
	all_outcomes_X = 0;
	all_outcomes_Y = 0;
	all_outcomes_Z = 0;
	iterations = 0;
	myscore = 0;
}

void solo_game_status::reset() {
	last_outcome = OUTS::N;
	all_outcomes_W = 0;
	all_outcomes_X = 0;
	all_outcomes_Y = 0;
	all_outcomes_Z = 0;
	iterations = 0;
	myscore = 0;
}

///////////////////////////////////////////////////////////////

gang_game_status::gang_game_status() {
	all_outcomes_A = 0;
	all_outcomes_B = 0;
	all_outcomes_C = 0;

	betray_num = 0;
	silence_num = 0;
}

void gang_game_status::reset() {
	last_outcome = OUTS::N;
	all_outcomes_A = 0;
	all_outcomes_B = 0;
	all_outcomes_C = 0;
	all_outcomes_W = 0;
	all_outcomes_X = 0;
	all_outcomes_Y = 0;
	all_outcomes_Z = 0;
	iterations = 0;
	myscore = 0;

	betray_num = 0;
	silence_num = 0;

	for (unsigned i = 0; i < 5; ++i) {
		choice_list[i] = CHOICES::RANDOM;
	}
}