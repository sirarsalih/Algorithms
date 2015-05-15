#pragma once
#include "TravelingSalesPersonProblem.h"

class Fitness
{
public:
	Fitness(void);
	~Fitness(void);	
	void calculateFitness(std::vector<std::vector<int>> vectorPopulation, std::map<std::vector<int>, double> &mapChromosomeFitness);
private:
	double x0Position;
	double y0Position;
	double x1Position;
	double y1Position;
	double xDistance;
	double yDistance;
	double distance;
	double totalDistance;
};

