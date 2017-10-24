#include "stdafx.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <ctime>
#include <thread>
#include <mutex>
#include <io.h>

using namespace std;

class interpretator {
protected:
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

public:
	interpretator() {
		last_outcome = OUTS::N;
		last_output = CHOICES::RANDOM;
		all_outcomes_W = 0;
		all_outcomes_X = 0;
		all_outcomes_Y = 0;
		all_outcomes_Z = 0;
		iterations = 0;
		myscore = 0;

		stc_index = 0;
		stg_index = 0;
	}

	~interpretator() {
		for (auto i = strategy.begin(); i != strategy.end(); ++i) {
			delete *i;
		}

		strategy.clear();
	}

	bool read_strategy(const string file_name) {
		fstream fin;
		string temp_str;

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

		return !error_flag;
	}

	void execute_strategy() {
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

	CHOICES output() {
		return last_output;
	}

	void feedback(CHOICES s) {//get the other prisoner's output
		switch (s)
		{
		case CHOICES::SILENCE:
			switch (last_output)
			{
			case CHOICES::SILENCE:
				last_outcome = OUTS::W;
				all_outcomes_W += 1;
				myscore += 2;
				break;
			case CHOICES::BETRAY:
				last_outcome = OUTS::Y;
				all_outcomes_Y += 1;
				//myscore += 0;
				break;
			default:
				break;
			}
			break;
		case CHOICES::BETRAY:
			switch (last_output)
			{
			case CHOICES::SILENCE:
				last_outcome = OUTS::X;
				all_outcomes_X += 1;
				myscore += 5;
				break;
			case CHOICES::BETRAY:
				last_outcome = OUTS::Z;
				all_outcomes_Z += 1;
				myscore += 4;
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}

		++iterations;
	}

	void add_score(int num) {
		myscore += num;
	}

	void set_score(int num) {
		myscore = num;
	}

	int get_score() {
		return myscore;
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
	bool check_flag = false; //checking syntax
	bool error_flag = false; //syntax error in strategy
	bool read_flag = false; //strategy been read
	bool output_flag = false; //strategy outputed
	unsigned int stc_index;
	unsigned int stg_index;

	OUTS last_outcome;
	int all_outcomes_W, all_outcomes_X, all_outcomes_Y, all_outcomes_Z, iterations;
	int myscore;

	enum SYMBOLS {
		EQU,
		ADD,
		SUB,
		GRT,
		LST,
		OUT,
		VAR,
		NUM,
		IF,
		GOTO,
		SILE,
		BETR,
		RAND,
		END,
		DFT
	};

	enum ERRORS {
		MISSING_LINE_NUM,
		INDEX_OUT_OF_BOUNDS,
		SENTENCE_MISMATCHING,
		NEGATIVE_LINE_NUM,
		INVALID_SENTENCE,
		EXPRESSION_MISMATCHING,
		WRONG_COMPARISON,
		OUTCOME_MISSMATCHING,
		INVALID_NUM,
		TOO_BIG_NUM,
		VAR_MISMATCHING,
		VALUE_MISMATCHING,
		UNDEFINDED_IDENTIFIER,
		WRONG_LINE_NUM
	};

	enum WARNINGS {
		MISSING_GOTO_AFTER_IF,
		LOWERCASE_OUTCOME
	};

	CHOICES last_output;

	class sentence {
	public:

		sentence() {
			line_num = -1;
		}

		int line_num;
		vector<SYMBOLS> sym_vect;
		vector<string> kwd_vect;

		friend bool operator> (int value, sentence s) {
			return value > s.line_num;
		}

		friend bool operator>= (int value, sentence s) {
			return value >= s.line_num;
		}

		friend bool operator< (int value, sentence s) {
			return value < s.line_num;
		}

		friend bool operator<= (int value, sentence s) {
			return value <= s.line_num;
		}

		friend bool operator== (int value, sentence s) {
			return value == s.line_num;
		}

		friend bool operator> (sentence s1, sentence s2) {
			return s1.line_num > s2.line_num;
		}

		friend bool operator>= (sentence s1, sentence s2) {
			return s1.line_num >= s2.line_num;
		}

		friend bool operator< (sentence s1, sentence s2) {
			return s1.line_num < s2.line_num;
		}

		friend bool operator<= (sentence s1, sentence s2) {
			return s1.line_num <= s2.line_num;
		}

		friend bool operator== (sentence s1, sentence s2) {
			return s1.line_num == s2.line_num;
		}
	};

	vector<sentence*> strategy;

	sentence stc;

	inline bool isNUM(string str) {
		for (int i = 0; i != str.length(); i++)
		{
			if (!isdigit(str[i]))
			{
				return false;
			}
		}
		return true;
	}

	void split_string(const std::string& s, std::vector<std::string>& v, const std::string& c)//split string
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

	void standardise_operator(string sin, string& sout) {
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
		while (pos != string::npos) {
			pos = sout.find("  ", pos);
			if (pos != string::npos)
				sout.replace(pos, 2, 1, ' ');
		}
	}

	void sentence_preprocess(string &sent) {
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

	inline bool end() {//return whether index point the end of the sentence or not
		if (stc_index == stc.sym_vect.size() - 1)
			return true;
		else if (stc_index < stc.sym_vect.size() - 1)
			return false;
		else error_handling(ERRORS::INDEX_OUT_OF_BOUNDS);
	}

	void advance(SYMBOLS sym) {//match current symbol and move forward
		if (stc.sym_vect[stc_index] == sym) {
			if (stc_index < stc.sym_vect.size() - 1)
				++stc_index;
			//else end analyse
		}
		else if (stc.sym_vect[stc_index] == SYMBOLS::END)
			error_handling(ERRORS::SENTENCE_MISMATCHING);
		//else throw ERROR
	}

	int advance_value(SYMBOLS value) {//match VAR or NUM and return value
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
				temp = all_outcomes_W;
			else if (stc.kwd_vect[stc_index] == "ALLOUTCOMES_X")
				temp = all_outcomes_X;
			else if (stc.kwd_vect[stc_index] == "ALLOUTCOMES_Y")
				temp = all_outcomes_Y;
			else if (stc.kwd_vect[stc_index] == "ALLOUTCOMES_Z")
				temp = all_outcomes_Z;
			else if (stc.kwd_vect[stc_index] == "ITERATIONS")
				temp = iterations;
			else if (stc.kwd_vect[stc_index] == "MYSCORE")
				temp = myscore;
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

	OUTS advance_outcome(SYMBOLS out) {//match OUT and return outcome
		OUTS temp = OUTS::N;

		if (stc.sym_vect[stc_index] == SYMBOLS::OUT) {
			if (stc.kwd_vect[stc_index] == "LASTOUTCOME")
				temp = last_outcome;
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

	void expand_line() {
		//matching NUM
		if (stc.sym_vect[stc_index] == SYMBOLS::NUM)
			advance(SYMBOLS::NUM);
		else
			error_handling(ERRORS::MISSING_LINE_NUM);

		//matching sentence
		expand_sentence();

	}

	void expand_sentence() {
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

	bool expand_expression() {
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

	bool expand_cmpop(int value) {
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

	int expand_value() {
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

	int expand_asop() {
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

	void jump(int line) {
		if (!check_flag) {
			for (unsigned i = 0; i < strategy.size(); ++i) {
				if (strategy[i]->line_num == line) {
					stg_index = i;
					stc = *strategy[i];
				}
			}
		}
	}

	void output(CHOICES c) {
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

	void error_handling(ERRORS e) {
		cout << "ERROR: " << e << " in line ";
		if (stc.line_num)
			cout << stc.line_num << "." << endl;
		else
			cout << "unknown." << endl;
	}

	void warning_handling(WARNINGS w) {
		cout << "WARNING: " << w << endl;
	}

	void sentence_analyse(string &sentence) {

		sentence_preprocess(sentence);

		expand_line();

	}
};

class game_holder :public interpretator {
public:
	int compete(string stg1, string stg2) {
		interpretator prisoner1, prisoner2;
		prisoner1.read_strategy(stg1);
		prisoner2.read_strategy(stg2);

		srand((unsigned)time(NULL));

		for (int i = 0; i < 200; i++)
		{
			prisoner1.execute_strategy();
			prisoner2.execute_strategy();

			prisoner1.feedback(prisoner2.output());
			prisoner2.feedback(prisoner1.output());
		}

		if (!multi_threading) {
			cout << "prisoner1: " << stg1 << " | " << "prisoner2: " << stg2 << endl;
			cout << "prisoner1: " << prisoner1.get_score() << " | ";
			cout << "prisoner2: " << prisoner2.get_score() << " | ";
		}

		if (prisoner1.get_score() < prisoner2.get_score()) {
			if (!multi_threading) {
				cout << "prison1 win." << endl;
			}
			return 1;
		}
		else if (prisoner1.get_score() > prisoner2.get_score()) {
			if (!multi_threading) {
				cout << "prison2 win." << endl;
			}
			return 2;
		}
		else {
			if (!multi_threading) {
				cout << "draw." << endl;
			}
			return 0;
		}
	}

	void comparator(int count, int all) {
		int temp;
		string stg1, stg2;

		for (size_t i = count; i < strategies.size() - 1; i += all) {
			for (size_t j = i + 1; j < strategies.size(); ++j) {
				stg1 = strategies[i];
				stg2 = strategies[j];
				temp = compete(stg1, stg2);

				lock_guard<mutex> guard(lock);

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

	void compete(char* path) {
		int best, max_score;

		traversal_windows(path);

		score.resize(strategies.size(), 0);

		multi_threading = true;

		thread t0(&game_holder::comparator, this, 0, 8);
		thread t1(&game_holder::comparator, this, 1, 8);
		thread t2(&game_holder::comparator, this, 2, 8);
		thread t3(&game_holder::comparator, this, 3, 8);
		thread t4(&game_holder::comparator, this, 4, 8);
		thread t5(&game_holder::comparator, this, 5, 8);
		thread t6(&game_holder::comparator, this, 6, 8);
		thread t7(&game_holder::comparator, this, 7, 8);

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
		cout << "best strategy is " << strategies[best] << endl;
	}

private:
	vector<string> strategies;
	vector<int> score;
	mutex lock;
	bool multi_threading = false;

	void traversal_windows(const string path) {
		string temp = path;
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

};