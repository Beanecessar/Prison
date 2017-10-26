#pragma once
#include "stdafx.h"
#include "game.h"
#include <string>
#include <vector>

class prisoner :public game {
public:
	prisoner();

	prisoner(solo_game_status* sts);

	prisoner(std::string stg, solo_game_status* sts);

	~prisoner();

	bool read_strategy(const std::string file_name);

	void execute_strategy();

	CHOICES output();

	void feedback(CHOICES o);

	bool ready();

protected:
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

	solo_game_status* status;

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
		std::vector<SYMBOLS> sym_vect;
		std::vector<std::string> kwd_vect;

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

	std::vector<sentence*> strategy;

	sentence stc;

	inline bool isNUM(std::string str);

	void split_string(const std::string& s, std::vector<std::string>& v, const std::string& c);

	void standardise_operator(std::string sin, std::string& sout);

	void sentence_preprocess(std::string &sent);

	inline bool end(); //return whether index point the end of the sentence or not

	void advance(SYMBOLS sym); //match current symbol and move forward

	virtual float advance_value(SYMBOLS value); //match VAR or NUM and return value
		
	virtual OUTS advance_outcome(SYMBOLS out); //match OUT and return outcome

	void expand_line();

	void expand_sentence();

	bool expand_expression();

	bool expand_cmpop(float value);

	float expand_value();

	float expand_asop();

	void jump(int line);

	void output(CHOICES c);

	void error_handling(ERRORS e);

	void warning_handling(WARNINGS w);

	void sentence_analyse(std::string &sentence);
};

class gang_member :public prisoner {
public:
	gang_member();

	gang_member(gang_game_status* sts);

	gang_member(std::string stg, gang_game_status* sts);

	void set_status(gang_game_status* sts);

	float gang_member::advance_value(SYMBOLS value);

	game::OUTS gang_member::advance_outcome(SYMBOLS out);

protected:
	gang_game_status* status;

	
};

class gang :public game {
public:
	gang();

	gang(gang_game_status* sts);

	gang(gang_member* gms, gang_game_status* sts);

	int get_betray_num();

	int get_silence_num();

	//void set_all_member(std::string path);

	void set_all_member(gang_member* gms);

	void set_member(unsigned i, gang_member* m);

	bool check_ready(); //check if each member read the strategy correctly

	void execute_strategies();

	void feedback(gang_game_status* sts);

private:
	gang_game_status* status;

	gang_member* member_list[5];

	bool spy_flag;
};