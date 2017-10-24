#pragma once
#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <ctime>

class generator {
public:
	generator();

	void ask_config();

	void start_generate(std::string path);

private:
	/*
	line -> NUM statement

	statement -> IF expression GOTO value
	statement -> GOTO value
	statement -> SILENCE
	statement -> BETRAY
	statement -> RANDOM

	expression -> OUT = OUT
	expression -> value cmpop
	cmpop -> > value|< value|= value

	value -> NUM asop
	value -> VAR asop
	asop -> + value|- value|e
	*/

	std::stringstream sentence;

	bool last_end_flag; //last sentence is one of the BETRAY/SILENCE/RANDOM

	unsigned max_rows; //max rows a strategy can have
	unsigned stg_rows; //rows strategy has
	unsigned crt_row; //current generating row

	//TODO set possibility

	//possibility of different branches in expand_sentence() : A of A+B+C+D+E
	//unsigned if_poss = 1;
	//unsigned gt_poss = 1;
	//unsigned bty_poss = 1;
	//unsigned slt_poss = 1;
	//unsigned rdm_poss = 1;

	//possibility of different branches in expand_expression() : A of A+B
	//unsigned out_poss = 1;
	//unsigned vle_poss = 1;

	//possibility of go deeper in expand_value() : X of 100
	unsigned deep_value;


	void generate(std::string path);

	void expand_line();

	void expand_sentence();

	void expand_expression();

	void expand_cmpop();

	void expand_value();

	void expand_asop();
};