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

class solo_game_status :public game {
public:
	solo_game_status();

	OUTS last_outcome;
	int  all_outcomes_W, all_outcomes_X, all_outcomes_Y, all_outcomes_Z, iterations;
	float myscore;
	void reset();
};

class gang_game_status :public solo_game_status {
public:
	gang_game_status();

	int all_outcomes_A, all_outcomes_B, all_outcomes_C;
	int betray_num, silence_num;

	void reset();
};