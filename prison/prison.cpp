// prison.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "generator.h"
#include "game_holder.h"


int main()
{
	//generator test
	generator g;
	g.start_generate("D:/strategies/");

	//interpretator test
	solo_game round;
	round.compete("D:/strategies/");

	return 0;
}

