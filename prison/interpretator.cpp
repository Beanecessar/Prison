#include "stdafx.h"
#include "interpretator.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <ctime>
#include <thread>
#include <mutex>
#include <io.h>

prisoner::prisoner() {
	last_output = CHOICES::RANDOM;
	myscore = NULL;
	status = NULL;

	stc_index = 0;
	stg_index = 0;
}

prisoner::prisoner(game_status* sts) {
	last_output = CHOICES::RANDOM;
	status = sts;

	stc_index = 0;
	stg_index = 0;
}

prisoner::prisoner(const std::string stg, game_status* sts) {
	last_output = CHOICES::RANDOM;
	status = sts;

	stc_index = 0;
	stg_index = 0;

	read_strategy(stg);
}

prisoner::~prisoner() {
	for (auto i = strategy.begin(); i != strategy.end(); ++i) {
		delete *i;
	}

	strategy.clear();
}

bool prisoner::read_strategy(const std::string file_name) {
	std::fstream fin;
	std::string temp_str;

	check_flag = true; //checking syntax, disable jump and output
	error_flag = false;
	strategy.clear();

	fin.open(file_name);
	if (!fin.is_open()) {
		error_flag = true;
	}
	else {
		while (!fin.eof()) {
			getline(fin, temp_str);
			if (temp_str != "")
				sentence_analyse(temp_str);
		}

		read_flag = true;
	}

	fin.close();

	return !error_flag;
}

void prisoner::execute_strategy() {
	if (read_flag && !error_flag) {
		stg_index = 0;
		check_flag = false;
		output_flag = false;

		while (stg_index < strategy.size() && !output_flag) {
			stc = *strategy[stg_index];
			stc_index = 0;
			//cout << "execute line " << stc.line_num << "." << endl;
			expand_line();
		}
	}
}

game::CHOICES prisoner::output() {
	return last_output;
}

void prisoner::feedback(CHOICES o) {
	if (last_output == CHOICES::BETRAY) {
		if (o == CHOICES::BETRAY) {
			status->last_outcome = OUTS::Z;
			status->all_outcomes_Z += 1;
			status->myscore += 4;
		}
		else {
			status->last_outcome = OUTS::Y;
			status->all_outcomes_Y += 1;
		}
	}
	else {
		if (o == CHOICES::BETRAY) {
			status->last_outcome = OUTS::X;
			status->all_outcomes_Z += 1;
			status->myscore += 5;
		}
		else {
			status->last_outcome = OUTS::W;
			status->all_outcomes_Y += 1;
			status->myscore += 2;
		}
	}
}

bool prisoner::ready() {
	return read_flag && !error_flag;
}

inline bool prisoner::isNUM(std::string str) {
	for (int i = 0; i != str.length(); i++)
	{
		if (!isdigit(str[i]))
		{
			return false;
		}
	}
	return true;
}

void prisoner::split_string(const std::string& s, std::vector<std::string>& v, const std::string& c)//split string
{
	std::string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}

void prisoner::standardise_operator(std::string sin, std::string& sout) {
	std::string::size_type pos;
	sout = sin;

	for (auto i = sout.begin(); i != sout.end(); ++i)
	{
		if (*i == '+' || *i == '-' || *i == '>' || *i == '<' || *i == '=') {
			sout.insert(i, ' ');
			i = sout.insert(i + 2, ' ');
		}
	}

	pos = 0;
	while (pos != std::string::npos) {
		pos = sout.find("  ", pos);
		if (pos != std::string::npos)
			sout.replace(pos, 2, 1, ' ');
	}
}

void prisoner::sentence_preprocess(std::string &sent) {
	stc_index = 0;
	stc.line_num = -1;
	stc.sym_vect.clear();
	stc.kwd_vect.clear();

	//TODO: rewrite standardise_operator function
	//standardise_operator(sent, std_str);

	split_string(sent, stc.kwd_vect, " ");

	for (auto it = stc.kwd_vect.begin(); it != stc.kwd_vect.end(); ++it) {
		if (*it == "IF")
			stc.sym_vect.push_back(SYMBOLS::IF);
		else if (*it == "GOTO")
			stc.sym_vect.push_back(SYMBOLS::GOTO);
		else if (*it == "BETRAY")
			stc.sym_vect.push_back(SYMBOLS::BETR);
		else if (*it == "SILENCE")
			stc.sym_vect.push_back(SYMBOLS::SILE);
		else if (*it == "RANDOM")
			stc.sym_vect.push_back(SYMBOLS::RAND);
		else if (*it == "+")
			stc.sym_vect.push_back(SYMBOLS::ADD);
		else if (*it == "-")
			stc.sym_vect.push_back(SYMBOLS::SUB);
		else if (*it == ">")
			stc.sym_vect.push_back(SYMBOLS::GRT);
		else if (*it == "<")
			stc.sym_vect.push_back(SYMBOLS::LST);
		else if (*it == "=")
			stc.sym_vect.push_back(SYMBOLS::EQU);
		else if (*it == "LASTOUTCOME" || *it == "W" || *it == "X" || *it == "Y" || *it == "Z")
			stc.sym_vect.push_back(SYMBOLS::OUT);
		else if (*it == "ALLOUTCOMES_W" || *it == "ALLOUTCOMES_X" || *it == "ALLOUTCOMES_Y" || *it == "ALLOUTCOMES_Z" || *it == "ITERATIONS" || *it == "MYSCORE")
			stc.sym_vect.push_back(SYMBOLS::VAR);
		else if (isNUM(*it))
			stc.sym_vect.push_back(SYMBOLS::NUM);
		else
			error_handling(ERRORS::UNDEFINDED_IDENTIFIER);
	}

	stc.kwd_vect.push_back("END");
	stc.sym_vect.push_back(SYMBOLS::END);

	if (stc.sym_vect[0] == SYMBOLS::NUM) {
		stc.line_num = stoi(stc.kwd_vect[0]);
		sentence* s = new sentence(stc);

		strategy.push_back(s);
	}
	else
		error_handling(MISSING_LINE_NUM);

}

inline bool prisoner::end() {//return whether index point the end of the sentence or not
	if (stc_index == stc.sym_vect.size() - 1)
		return true;
	else if (stc_index < stc.sym_vect.size() - 1)
		return false;
	else error_handling(ERRORS::INDEX_OUT_OF_BOUNDS);
}

void prisoner::advance(SYMBOLS sym) {//match current symbol and move forward
	if (stc.sym_vect[stc_index] == sym) {
		if (stc_index < stc.sym_vect.size() - 1)
			++stc_index;
		//else end analyse
	}
	else if (stc.sym_vect[stc_index] == SYMBOLS::END)
		error_handling(ERRORS::SENTENCE_MISMATCHING);
	//else throw ERROR
}

int prisoner::advance_value(SYMBOLS value) {//match VAR or NUM and return value
	int temp;

	switch (value)
	{
	case SYMBOLS::NUM:
		try
		{
			temp = stoi(stc.kwd_vect[stc_index]);
		}
		catch (const std::invalid_argument) {
			error_handling(ERRORS::INVALID_NUM);
			return -1;
		}
		catch (const std::out_of_range) {
			error_handling(ERRORS::TOO_BIG_NUM);
			return -1;
		}
		break;

	case SYMBOLS::VAR:
		if (stc.kwd_vect[stc_index] == "ALLOUTCOMES_W")
			temp = status->all_outcomes_W;
		else if (stc.kwd_vect[stc_index] == "ALLOUTCOMES_X")
			temp = status->all_outcomes_X;
		else if (stc.kwd_vect[stc_index] == "ALLOUTCOMES_Y")
			temp = status->all_outcomes_Y;
		else if (stc.kwd_vect[stc_index] == "ALLOUTCOMES_Z")
			temp = status->all_outcomes_Z;
		else if (stc.kwd_vect[stc_index] == "ITERATIONS")
			temp = status->iterations;
		else if (stc.kwd_vect[stc_index] == "MYSCORE")
			temp = *myscore;
		else {
			error_handling(ERRORS::VAR_MISMATCHING);
			return -1;
		}
		break;

	default:
		error_handling(ERRORS::VALUE_MISMATCHING);
		return -1;
		break;
	}
	++stc_index;

	return temp;
}

game::OUTS prisoner::advance_outcome(SYMBOLS out) {//match OUT and return outcome
	OUTS temp = OUTS::N;

	if (stc.sym_vect[stc_index] == SYMBOLS::OUT) {
		if (stc.kwd_vect[stc_index] == "LASTOUTCOME")
			temp = status->last_outcome;
		else if (stc.kwd_vect[stc_index] == "W")
			temp = OUTS::W;
		else if (stc.kwd_vect[stc_index] == "X")
			temp = OUTS::X;
		else if (stc.kwd_vect[stc_index] == "Y")
			temp = OUTS::Y;
		else if (stc.kwd_vect[stc_index] == "Z")
			temp = OUTS::Z;
		else if (stc.kwd_vect[stc_index] == "w") {
			temp = OUTS::W;
			warning_handling(WARNINGS::LOWERCASE_OUTCOME);
		}
		else if (stc.kwd_vect[stc_index] == "x") {
			temp = OUTS::X;
			warning_handling(WARNINGS::LOWERCASE_OUTCOME);
		}
		else if (stc.kwd_vect[stc_index] == "y") {
			temp = OUTS::Y;
			warning_handling(WARNINGS::LOWERCASE_OUTCOME);
		}
		else if (stc.kwd_vect[stc_index] == "z") {
			temp = OUTS::Z;
			warning_handling(WARNINGS::LOWERCASE_OUTCOME);
		}
		else
			error_handling(ERRORS::OUTCOME_MISSMATCHING);

		++stc_index;
		return temp;
	}
	else
		error_handling(ERRORS::OUTCOME_MISSMATCHING);

	return OUTS::N;
}

void prisoner::expand_line() {
	//matching NUM
	if (stc.sym_vect[stc_index] == SYMBOLS::NUM)
		advance(SYMBOLS::NUM);
	else
		error_handling(ERRORS::MISSING_LINE_NUM);

	//matching sentence
	expand_sentence();

}

void prisoner::expand_sentence() {
	int temp_value = -1;
	bool temp_bool = false;

	switch (stc.sym_vect[stc_index])
	{
	case SYMBOLS::IF:
		//matching IF
		advance(SYMBOLS::IF);

		//matching expression
		temp_bool = expand_expression();

		//matching GOTO
		if (stc.sym_vect[stc_index] == SYMBOLS::GOTO)
			advance(SYMBOLS::GOTO);
		else
		{
			warning_handling(WARNINGS::MISSING_GOTO_AFTER_IF);
			break;
		}

		//matching value
		temp_value = expand_value();
		if (temp_value < 0)
			error_handling(ERRORS::NEGATIVE_LINE_NUM);
		else if (temp_bool)
			jump(temp_value);
		else
			//execute next sentence
			++stg_index;
		break;

	case SYMBOLS::GOTO:
		//matching GOTO
		advance(SYMBOLS::GOTO);

		//matching value
		temp_value = expand_value();
		if (temp_value < 0)
			error_handling(ERRORS::NEGATIVE_LINE_NUM);
		else if (temp_bool)
			jump(temp_value);
		else
			//execute next sentence
			++stg_index;
		break;

	case SYMBOLS::SILE:
		//matching SILE
		advance(SYMBOLS::SILE);
		output(CHOICES::SILENCE);
		break;

	case SYMBOLS::BETR:
		//matching BETR
		advance(SYMBOLS::BETR);
		output(CHOICES::BETRAY);
		break;

	case SYMBOLS::RAND:
		//matching RAND
		advance(SYMBOLS::RAND);
		output(CHOICES::RANDOM);
		break;

	default:
		error_handling(ERRORS::INVALID_SENTENCE);
		break;
	}
}

bool prisoner::expand_expression() {
	int temp_value;
	OUTS temp_out = OUTS::N;
	bool rtn_value;

	switch (stc.sym_vect[stc_index])
	{
	case SYMBOLS::OUT:
		//matching OUT

		temp_out = advance_outcome(SYMBOLS::OUT);

		//matching EQU
		if (stc.sym_vect[stc_index] == SYMBOLS::EQU)
			advance(SYMBOLS::EQU);
		else {
			error_handling(ERRORS::EXPRESSION_MISMATCHING);
			return false;
		}

		//matching OUT
		rtn_value = temp_out == advance_outcome(SYMBOLS::OUT);
		break;

	case SYMBOLS::VAR:
	case SYMBOLS::NUM:
		//matching value
		temp_value = expand_value();

		//matching cmpop
		return expand_cmpop(temp_value);

		break;
	default:
		error_handling(ERRORS::EXPRESSION_MISMATCHING);
		break;
	}

	return rtn_value;
}

bool prisoner::expand_cmpop(int value) {
	SYMBOLS cmp = DFT;
	int temp_value = -1;

	//matching comparison
	switch (stc.sym_vect[stc_index])
	{
	case SYMBOLS::GRT:
		cmp = SYMBOLS::GRT;
		advance(SYMBOLS::GRT);
		break;

	case SYMBOLS::LST:
		cmp = SYMBOLS::LST;
		advance(SYMBOLS::LST);
		break;

	case SYMBOLS::EQU:
		cmp = SYMBOLS::EQU;
		advance(SYMBOLS::EQU);
		break;

	default:
		error_handling(ERRORS::WRONG_COMPARISON);
		break;
	}

	//matching value
	switch (cmp)
	{
	case SYMBOLS::GRT:
		return value > expand_value();
		break;

	case SYMBOLS::LST:
		return value < expand_value();
		break;

	case SYMBOLS::EQU:
		return value == expand_value();
		break;

	default:
		break;
	}

	return false;
}

int prisoner::expand_value() {
	int temp;

	switch (stc.sym_vect[stc_index])
	{
	case SYMBOLS::NUM:
		//matching NUM
		temp = advance_value(SYMBOLS::NUM);

		//matching asop
		temp += expand_asop();

		break;

	case SYMBOLS::VAR:
		//matching VAR
		temp = advance_value(SYMBOLS::VAR);

		//matching asop
		temp += expand_asop();

		break;

	default:
		error_handling(ERRORS::VALUE_MISMATCHING);
		return -1;
		break;
	}

	return temp;
}

int prisoner::expand_asop() {
	switch (stc.sym_vect[stc_index])
	{
	case SYMBOLS::ADD:
		//matching ADD
		advance(SYMBOLS::ADD);

		//matching value
		return expand_value();
		break;

	case SYMBOLS::SUB:
		//matching SUB
		advance(SYMBOLS::SUB);

		//matching value
		return -expand_value();
		break;

	default:
		return 0;
		break;
	}
}

void prisoner::jump(int line) {
	if (!check_flag) {
		for (unsigned i = 0; i < strategy.size(); ++i) {
			if (strategy[i]->line_num == line) {
				stg_index = i;
				stc = *strategy[i];
			}
		}
	}
}

void prisoner::output(CHOICES c) {
	if (check_flag)
		return;
	switch (c)
	{
	case CHOICES::BETRAY:
		last_output = CHOICES::BETRAY;
		output_flag = true;
		break;
	case CHOICES::SILENCE:
		last_output = CHOICES::SILENCE;
		output_flag = true;
		break;
	case CHOICES::RANDOM:
		if (rand() % 2 == 0) {//TODO: rewrite rand function
			last_output = CHOICES::BETRAY;
		}
		else {
			last_output = CHOICES::SILENCE;
		}
		output_flag = true;
	default:
		break;
	}
}

void prisoner::error_handling(ERRORS e) {
	std::cout << "ERROR: " << e << " in line ";
	if (stc.line_num)
		std::cout << stc.line_num << "." << std::endl;
	else
		std::cout << "unknown." << std::endl;
}

void prisoner::warning_handling(WARNINGS w) {
	std::cout << "WARNING: " << w << std::endl;
}

void prisoner::sentence_analyse(std::string &sentence) {

	sentence_preprocess(sentence);

	expand_line();

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

gang::gang() {
	betray_num = 0;
	silence_num = 0;
}

gang::gang(gang_member* gms) {
	betray_num = 0;
	silence_num = 0;

	if (sizeof(gms) == 5) {
		for (unsigned i = 0; i < 5; ++i) {
			member_list[i] = *(gms + i);
		}
	}
}

void gang::set_all_member(gang_member* gms) {
	if (sizeof(gms) == 5) {
		for (unsigned i = 0; i < 5; ++i) {
			member_list[i] = *(gms + i);
		}
	}
}

int gang::get_betray_num() {
	return betray_num;
}

int gang::get_silence_num() {
	return silence_num;
}

bool gang::check_ready() {
	bool ready = true;

	for (unsigned i = 0; i < 5; ++i) {
		if (!member_list[i].ready())
			ready = false;
	}

	return ready;
}

void gang::execute_strategis(){
	CHOICES temp;

	betray_num = 0;
	silence_num = 0;

	for (unsigned i; i < 5; ++i) {
		member_list[i].execute_strategy();
		temp = member_list[i].output();
		if (temp == CHOICES::BETRAY) {
			betray_num += 1;
		}
		else
			silence_num += 1;
	}
}
