#pragma once
#include "TravelingSalespersonProblem.h"

class Mutation
{
public:
	Mutation(void);
	~Mutation(void);	
	void mutateInverse(std::vector<std::vector<int>> &vectorOffspringsPopulation);
private:	
	int mutationProbability[10];
	int randomNum;
};

