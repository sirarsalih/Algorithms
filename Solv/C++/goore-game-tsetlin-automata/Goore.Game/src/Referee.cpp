#include "Referee.h"
#include "Tsetlin.h"

const int NUM_OF_ITERATIONS = 10000;

Referee::Referee()
{
	yesVotes = 0;
	voteCounter = 0;

	probabilityArray[0] = 0;
	probabilityArray[1] = 0.1;
	probabilityArray[2] = 0.2;
	probabilityArray[3] = 0.3;
	probabilityArray[4] = 0.4;
	probabilityArray[5] = 0.5;
	probabilityArray[6] = 0.6;
	probabilityArray[7] = 0.7;
	probabilityArray[8] = 0.8;
	probabilityArray[9] = 0.9;
	probabilityArray[10] = 1;
}

void Referee::decide(std::vector<Tsetlin> &tsetlinAutomata)
{
	//1 == yes, calculate number of yes votes
	for (int i = 0; i < NUM_OF_ITERATIONS; i++) {
		for (int j = 0; j < tsetlinAutomata.size(); j++) {
			if (tsetlinAutomata[j].makeDecision() == 1)
				yesVotes++;
		}

		//30 % reward chance
		if (yesVotes == 0)
			rewardOrPenalize(tsetlinAutomata, 0.3);

		//50 % reward chance
		if (yesVotes == 1)
			rewardOrPenalize(tsetlinAutomata, 0.5);

		//70 % reward chance
		if (yesVotes == 2) {
			if (rewardOrPenalize(tsetlinAutomata, 0.7))
				voteCounter++;
		}

		//40 % reward chance
		if (yesVotes == 3)
			rewardOrPenalize(tsetlinAutomata, 0.4);

		yesVotes = 0;
	}

	double probabilityOfYesVotes = voteCounter / NUM_OF_ITERATIONS;

	std::cout << "Number of states pr. action: " << tsetlinAutomata[0].getN() << std::endl << "Number of iterations: " << NUM_OF_ITERATIONS << std::endl << "Probability of yes votes: " << probabilityOfYesVotes << std::endl << std::endl;
}

bool Referee::rewardOrPenalize(std::vector<Tsetlin> &tsetlinAutomata, double probability)
{
	int randomIndex;
	for (int i = 0; i < 2; i++)
		randomIndex = rand() % 11;

	if (probabilityArray[randomIndex] <= probability) {
		return reward(tsetlinAutomata);
	}

	return penalize(tsetlinAutomata);
}

bool Referee::reward(std::vector<Tsetlin> &tsetlinAutomata)
{
	for (int i = 0; i < tsetlinAutomata.size(); i++)
		tsetlinAutomata[i].reward();
	return true;
}

bool Referee::penalize(std::vector<Tsetlin> &tsetlinAutomata)
{
	for (int i = 0; i < tsetlinAutomata.size(); i++)
		tsetlinAutomata[i].penalize();
	return false;
}