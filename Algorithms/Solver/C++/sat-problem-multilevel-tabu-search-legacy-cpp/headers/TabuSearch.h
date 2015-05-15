#pragma once
#include "TabuSearch.h"
#include "TimeElapsed.h"
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>

class TabuSearch
{
public:
	TabuSearch(void);
	void flip(std::map<std::string, bool> &mapLiteralValues, std::string cluster);
	void run(std::ofstream &foutput, int &bestSoFar, std::string noLettersCollectionString, int &numberOfLiterals, int levelCounter, std::map<std::string, std::string> mapLevelClusters, std::vector<std::string> vectorStringNumbers, std::vector<std::string> &initialClusterCollection, std::vector<std::string> clusterCollection, std::map<std::string, bool> &mapLiteralValues, std::multimap<std::string, bool> &mapClauseValues, std::string numberOfXLiterals, std::string numberOfClauses, std::multimap<std::string, std::vector<std::string>> &multiMapLiteralClause, clock_t begin);

	~TabuSearch(void);
};
