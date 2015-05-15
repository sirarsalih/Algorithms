#pragma once
#include <iostream>
#include <fstream>
#include <sstream> 
#include <math.h>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <windows.h>
#include "Fitness.h"
#include "CrossOver.h"
#include "Mutation.h"
#include "Selection.h"
#include "GUI.h"

class TravelingSalespersonProblem
{
public:
	TravelingSalespersonProblem(void);
	TravelingSalespersonProblem::TravelingSalespersonProblem(int argc, char * args[]);
	~TravelingSalespersonProblem(void);
	void solve();
	std::vector<int> createSolution();
	int factorial(int);
	//Must be public such that can be accessed by Fitness class
	static std::map<int, double> mapX;
	static std::map<int, double> mapY;
private:
	static std::string RESULTS;
	static int MUTATION_METHOD;
	static int CROSS_OVER_METHOD;
	static int SELECTION_METHOD;
	int *cities;
	double *x;
	double *y;
	std::vector<int> vectorWidth;										//Screen vectors
	std::vector<int> vectorHeight;
	std::vector<std::vector<int>> vectorPopulation;						//Population
	std::vector<std::vector<int>> vectorOffspringsPopulation;			//Offsprings
	std::vector<std::vector<int>> vectorSelectedOffspringsPopulation;	//Selected offsprings; They are the future!
	std::vector<int> vectorChromosome;									//Chromosome
	double bestFitnessSoFar;											//The best fitness of an offspring so far (after each generation)
	std::string stringBestFitnessSoFar;
	std::string stringGeneration;
	std::map<std::vector<int>, double> mapChromosomeFitness;			//Offspring fitness maps [Chromosome][Fitness]
	std::vector<std::vector<int>>::iterator iteratorVector;				//Iterator vector, to be able to find stuff
	void randomPlotting();
	void circlePlotting();
	void fixedPlotting();
	std::string convertToString(int);
	void initFixedCoordinates(char * args[], int size);
	void drawBestFitnessSoFarAndGeneration(int);
	void getInputFromUserAndinitProblemParameters();
	void getInputFromUserAndInitSolutionParameters();
	void createInitialPopulationRandomly();
	void selectMutationMethod();
	void selectCrossOverMethod();
	void selectSelectionMethod();
	void makeSurePopulationHasAtLeastTwoParents();
	void printOutValueOfBestOffspring(std::ofstream&);
	void updateTheBestSoFarOffspringValue();
	void makeSelectedOffspringsTheNewPopulationForNextGeneration();
	int numOfCities;
	int numOfGenerations;
	int numOfChromosomes;		
	int randOrCircleOrFixed;
	std::string _coordinates;
};

