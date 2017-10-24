#pragma once

class game
{
public:
	enum OUTS {
		A,
		B,
		C,
		W,
		X,
		Y,
		Z,
		N //no valid last outcome
	};

	enum CHOICES {
		SILENCE,
		BETRAY,
		RANDOM
	};
};

class game_status :public game {
public:
	game_status();

	OUTS last_outcome;
	int all_outcomes_A, all_outcomes_B, all_outcomes_C, all_outcomes_W, all_outcomes_X, all_outcomes_Y, all_outcomes_Z, iterations, myscore;
	void reset();
};