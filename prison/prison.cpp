// prison.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "interpretator.cpp"
#include "generator.cpp"


int main()
{
	//generator test
	generator g;
	g.start_generate("D:/strategies/");

	//interpretator test
	game_holder round;
	round.compete("D:/strategies/");

	return 0;
}

