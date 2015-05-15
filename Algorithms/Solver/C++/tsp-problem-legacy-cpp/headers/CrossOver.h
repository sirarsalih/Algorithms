#pragma once
#include "TravelingSalespersonProblem.h"

class CrossOver
{
public:
	CrossOver(void);
	~CrossOver(void);
	void CrossOver::singlePoint(std::vector<std::vector<int>> vectorPopulation, std::vector<std::vector<int>> &vectorOffspringsPopulation);
	void singlePointAssistance(int beginInde, int randomEndIndex, std::vector<int> vectorChromosomeParent1, std::vector<int> vectorChromosomeParent2, std::vector<int> &vectorChromosomeChild1);
	void CrossOver::twoPoint(std::vector<std::vector<int>> vectorPopulation, std::vector<std::vector<int>> &vectorOffspringsPopulation);
	void CrossOver::positionBased(std::vector<std::vector<int>> vectorPopulation, std::vector<std::vector<int>> &vectorOffspringsPopulation);
	void CrossOver::order(std::vector<std::vector<int>> vectorPopulation, std::vector<std::vector<int>> &vectorOffspringsPopulation);
private:
	int crossOverProbability[10];
	int randomNum;
	int numberOfChildren;
	std::vector<int> vectorChromosomeParent1;
	std::vector<int> vectorChromosomeParent2;
	std::vector<int> vectorChromosomeChild;
	std::vector<std::string> vectorStringChromosomeChild;
	int gene;
	std::string stringGene;
	int randomNumOfGenes;
	int randomBeginIndex;
	int randomEndIndex;

	//Iterator vectors, to be able to find stuff
	std::vector<std::vector<int>>::iterator iteratorVector;
	std::vector<int>::iterator iteratorVectorInteger;
	std::vector<std::string>::iterator iteratorVectorString;
};

