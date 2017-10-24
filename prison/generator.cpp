#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <ctime>

using namespace std;

class generator {
public:
	generator() {
		max_rows = 20;
		srand((unsigned)time(NULL));
	}

	void ask_config() {
		cout << "Input max rows of the strategies." << endl;
		cin >> max_rows;
	}

	void start_generate(string path) {
		int count;
		cout << "How many strategies you want to create?" << endl;
		cin >> count;
		for (int i = 0; i < count; i++) {
			generate(path);
		}
	}

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

	stringstream sentence;

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


	void generate(string path) {
		//open and name file
		ofstream fp;
		stringstream file_name;
		file_name << path << time << rand() << ".txt";
		fp.open(file_name.str());
		//cout << file_name.str() << endl;
		file_name.clear();
		file_name.str("");

		stg_rows = rand() % (max_rows - 1) + 1;
		crt_row = 0;
		last_end_flag = false;
		for (unsigned i = 0; i < stg_rows; ++i) {
			deep_value = 32;
			expand_line();
			sentence << endl;
			crt_row += 1;
			fp << sentence.str();
			sentence.clear();
			sentence.str("");
		}

		fp.close();
	}

	void expand_line() {
		sentence << crt_row;

		expand_sentence();
	}

	void expand_sentence() {
		int branch;
		//choose branch
		if ((stg_rows - crt_row - 1) == 0 && !last_end_flag)
			branch = rand() % 3 + 2;
		else
			branch = rand() % 5;

		switch (branch)
		{
		case 0: //IF branch
			sentence << " IF";
			expand_expression();
			sentence << " GOTO";
			if ((stg_rows - crt_row - 1) == 0 || rand() % 3 == 0 && last_end_flag) {
				sentence << " " << rand() % crt_row; //jump backward
			}
			else
				sentence << " " << rand() % (stg_rows - crt_row - 1) + crt_row + 1; //jump forward
			last_end_flag = false;
			break;

		case 1: //GOTO branch
			sentence << " GOTO";
			if ((stg_rows - crt_row - 1) == 0 || rand() % 3 == 0 && last_end_flag) {
				sentence << " " << rand() % crt_row; //jump backward
			}
			else
				sentence << " " << rand() % (stg_rows - crt_row - 1) + crt_row + 1; //jump forward
			last_end_flag = false;
			break;

		case 2: //BETRAY branch
			sentence << " BETRAY";
			last_end_flag = true;
			break;

		case 3: //SILENCE branch
			sentence << " SILENCE";
			last_end_flag = true;
			break;

		case 4: //RANDOM branch
			sentence << " RANDOM";
			last_end_flag = true;
			break;

		default:
			break;
		}

	}

	void expand_expression() {
		//choose branch

		switch (rand() % 2)
		{
		case 0:
			sentence << " LASTOUTCOME =";

			switch (rand() % 4)
			{
			case 0:
				sentence << " W";
				break;

			case 1:
				sentence << " X";
				break;

			case 2:
				sentence << " Y";
				break;

			case 3:
				sentence << " Z";
				break;

			default:
				break;
			}

			break;

		case 1:
			expand_value();

			expand_cmpop();

			break;

		default:
			break;
		}
	}

	void expand_cmpop() {
		switch (rand() % 3)
		{
		case 0:
			sentence << " <";
			break;

		case 1:
			sentence << " >";
			break;

		case 2:
			sentence << " =";
			break;

		default:
			break;
		}

		expand_value();
	}

	void expand_value() {
		//choose branch

		switch (rand() % 2)
		{
		case 0: //VAR branch
			switch (rand() % 6)
			{
			case 0:
				sentence << " ALLOUTCOMES_W";
				break;

			case 1:
				sentence << " ALLOUTCOMES_X";
				break;

			case 2:
				sentence << " ALLOUTCOMES_Y";
				break;

			case 3:
				sentence << " ALLOUTCOMES_Z";
				break;

			case 4:
				sentence << " ITERATIONS";
				break;

			case 5:
				sentence << " MYSCORE";
				break;

			default:
				break;
			}

			break;

		case 1: //NUM branch
			sentence << " " << rand() % 200;
			break;

		default:
			break;
		}

		if ((unsigned)rand() % 100 < deep_value) {
			expand_asop();
			deep_value /= 2;
		}
	}

	void expand_asop() {
		switch (rand() % 2)
		{
		case 0:
			sentence << " +";
			break;

		case 1:
			sentence << " -";
			break;

		default:
			break;
		}

		expand_value();
	}
};