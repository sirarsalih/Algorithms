#pragma once
#include "LearningAutomata.h"
#include "TimeElapsed.h"
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>
#include <math.h>

class LearningAutomata
{
public:
	LearningAutomata(void);
	std::vector<int> initializeNumbers(std::string numbersString, int &numberOfLiterals);
	void LearningAutomata::initializeDataStructure(std::vector<std::string> &vectorStringNumbers, std::vector<int> vectorNumbers, std::multimap<std::string, std::vector<std::string>> &multiMapLiteralClause);
	int randomAssignValuesAndEvaluateSolution(std::ofstream &foutput, std::map<int, int> &mapLiteralsStates, std::vector<std::string> &vectorSatisfiedClauses, std::vector<std::string> &vectorUnsatisfiedClauses, std::string numberOfXLiterals, std::vector<std::string> &vectorStringNumbers, int numberOfLiterals, std::string numberOfClauses, std::map<std::string, bool> &mapLiteralValues, std::multimap<std::string, bool> &mapClauseValues, clock_t begin);
	void flip(std::map<std::string, bool> &mapLiteralValues, std::string stringLiteral);
	void run(std::ofstream &foutput, std::map<int, int> &mapLiteralsStates, std::string noLettersCollectionString, int &numberOfLiterals, std::vector<std::string> vectorStringNumbers, std::map<std::string, bool> &mapLiteralValues, std::multimap<std::string, bool> &mapClauseValues, std::string numberOfXLiterals, std::string numberOfClauses, std::multimap<std::string, std::vector<std::string>> &multiMapLiteralClause, clock_t begin);

	~LearningAutomata(void);
};
