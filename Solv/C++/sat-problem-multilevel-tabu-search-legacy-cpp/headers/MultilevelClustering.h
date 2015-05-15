#pragma once
#include "TimeElapsed.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <string>
#include <fstream>

class MultilevelClustering
{
public:
	MultilevelClustering(void);
	std::vector<int> initializeNumbers(std::string numbersString, int &numberOfLiterals);
	void MultilevelClustering::initializeDataStructure(std::vector<std::string> &vectorStringNumbers, std::vector<int> vectorNumbers, std::multimap<std::string, std::vector<std::string>> &multiMapLiteralClause);
	void cluster(std::ofstream &foutput, int &bestSoFar, std::string numberOfXLiterals, int &levelCounter, std::map<std::string, std::string> &mapLevelClusters, std::vector<std::string> vectorStringNumbers, int numberOfLiterals, std::string numberOfClauses, std::vector<std::string> &initialClusterCollection, std::vector<std::string> &clusterCollection, std::map<std::string, bool> &mapLiteralValues, std::multimap<std::string, bool> &mapClauseValues, clock_t clock);
	~MultilevelClustering(void);
};

