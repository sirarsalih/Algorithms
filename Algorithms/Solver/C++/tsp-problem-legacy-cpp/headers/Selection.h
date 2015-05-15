#pragma once
#include "TravelingSalespersonProblem.h"

class Selection
{
public:
	Selection(void);
	~Selection(void);
	void rouletteWheel(std::vector<std::vector<int>> vectorOffspringsPopulation, std::map<std::vector<int>, double> mapChromosomeFitness, std::vector<std::vector<int>> &vectorSelectedOffspringsPopulation);
	void elitism(std::vector<std::vector<int>> vectorOffspringsPopulation, std::map<std::vector<int>, double> mapChromosomeFitness, std::vector<std::vector<int>> &vectorSelectedOffspringsPopulation);
};

