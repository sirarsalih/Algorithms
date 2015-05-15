#include <iostream>
#include <vector>
#include "Tsetlin.h"

#ifndef REFEREE
#define REFEREE

class Referee{

	public:
		Referee();
		void decide(std::vector<Tsetlin> &tsetlinAutomata);

	private:		
		int yesVotes;
		double voteCounter;
		double probabilityArray[11];
		bool rewardOrPenalize(std::vector<Tsetlin> &tsetlinAutomata, double probability);
		bool reward(std::vector<Tsetlin> &tsetlinAutomata);
		bool penalize(std::vector<Tsetlin> &tsetlinAutomata);
};

#endif