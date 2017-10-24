#include "stdafx.h"
#include "generator.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <ctime>

generator::generator() {
	max_rows = 20;
	srand((unsigned)time(NULL));
}

void generator::ask_config() {
	std::cout << "Input max rows of the strategies." << std::endl;
	std::cin >> max_rows;
}

void generator::start_generate(std::string path) {
	int count;
	std::cout << "How many strategies you want to create?" << std::endl;
	std::cin >> count;
	for (int i = 0; i < count; i++) {
		generate(path);
	}
}
void generator::generate(std::string path) {
	//open and name file
	std::ofstream fp;
	std::stringstream file_name;
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
		sentence << std::endl;
		crt_row += 1;
		fp << sentence.str();
		sentence.clear();
		sentence.str("");
	}

	fp.close();
}

void generator::expand_line() {
	sentence << crt_row;

	expand_sentence();
}

void generator::expand_sentence() {
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

void generator::expand_expression() {
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

void generator::expand_cmpop() {
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

void generator::expand_value() {
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

void generator::expand_asop() {
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