#include "stdafx.h"
#include "game.h"

game_status::game_status() {
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
}

void game_status::reset() {
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
}