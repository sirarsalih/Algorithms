#include "TravelingSalespersonProblem.h"

std::map<int, double> TravelingSalespersonProblem::mapX;
std::map<int, double> TravelingSalespersonProblem::mapY;
std::string TravelingSalespersonProblem::RESULTS;
int TravelingSalespersonProblem::MUTATION_METHOD;
int TravelingSalespersonProblem::CROSS_OVER_METHOD;
int TravelingSalespersonProblem::SELECTION_METHOD;
CrossOver crossOver;
Mutation mutation;
Fitness fitness;
Selection selection;
GUI gui;

TravelingSalespersonProblem::TravelingSalespersonProblem(int argc, char * args [])
{
	numOfCities = atoi(args[1]);
	numOfGenerations = atoi(args[2]);
	numOfChromosomes = atoi(args[3]);
	randOrCircleOrFixed = atoi(args[4]);
	MUTATION_METHOD = atoi(args[5]);
	CROSS_OVER_METHOD = atoi(args[6]);
	SELECTION_METHOD = atoi(args[7]);
	initFixedCoordinates(args, argc);

	getInputFromUserAndinitProblemParameters();
	getInputFromUserAndInitSolutionParameters();
	gui.initSDL();
}

void TravelingSalespersonProblem::getInputFromUserAndinitProblemParameters() {

	/*std::cout << "\n" << "How many cities?" << "\n";
	std::cin >> numOfCities;
	std::cout << "\n" << "How many generations?" << "\n";
	std::cin >> numOfGenerations;
	std::cout << "\n" << "How many initial chromosomes?" << "\n";
	std::cin >> numOfChromosomes;*/

	cities = new int[numOfCities];
	x = new double[numOfCities];
	y = new double[numOfCities];
	stringGeneration = "Generations: ";
	bestFitnessSoFar = 0;
	stringBestFitnessSoFar = "Best solution: ";
}

void TravelingSalespersonProblem::getInputFromUserAndInitSolutionParameters() {

	/*std::cout << "\n" << "Randomize city positions or plot on circle?" << "\n";
	std::cout << "1. Random positions" << "\n";
	std::cout << "2. Circle positions" << "\n";
	std::cin >> randOrCircle;*/

	switch (randOrCircleOrFixed)
	{
	case 1:
		randomPlotting();
		break;
	case 2:
		circlePlotting();
		break;
	case 3:
		fixedPlotting();
		break;
	default:
		exit(0);
		break;
	}
	/*std::cout << "\n" << "How would you like to mutate the chromosomes?" << "\n";
	std::cout << "1. Random inversion" << "\n";
	std::cin >> MUTATION_METHOD;
	std::cout << "\n" << "How would you like the chromsome parents to have intercourse (cross over)?" << "\n";
	std::cout << "1. Single point intercourse" << "\n";
	std::cout << "2. Position based intercourse" << "\n";
	std::cout << "3. Order based intercourse" << "\n";
	std::cin >> CROSS_OVER_METHOD;
	std::cout << "\n" << "How would you like to select the chromosomes for next generation?" << "\n";
	std::cout << "1. Elitisim" << "\n";
	std::cout << "2. Roulette wheel" << "\n";
	std::cin >> SELECTION_METHOD;*/
}

void TravelingSalespersonProblem::initFixedCoordinates(char * args [], int size)
{
	_coordinates = "";
	for (auto i = 8; i < size; i++) {
		_coordinates += args[i];
		_coordinates += " ";
	}
}

void TravelingSalespersonProblem::solve()
{
	createInitialPopulationRandomly();

	std::ofstream foutput("results.txt");

	for (int i = 1; i <= numOfGenerations; i++)
	{
		vectorOffspringsPopulation.clear();
		vectorSelectedOffspringsPopulation.clear();
		mapChromosomeFitness.clear();

		makeSurePopulationHasAtLeastTwoParents();

		fitness.calculateFitness(vectorPopulation, mapChromosomeFitness);

		selectSelectionMethod();

		selectCrossOverMethod();

		selectMutationMethod();

		printOutValueOfBestOffspring(foutput);

		updateTheBestSoFarOffspringValue();

		gui.drawPath(vectorPopulation[0]);

		drawBestFitnessSoFarAndGeneration(i);

		vectorPopulation.clear();

		makeSelectedOffspringsTheNewPopulationForNextGeneration();

	}
	std::cout << RESULTS;
	foutput.close();
	//Beep(800, 2000);
	std::cout << std::endl;
}

void TravelingSalespersonProblem::makeSelectedOffspringsTheNewPopulationForNextGeneration() {
	for (int j = 0; j < vectorSelectedOffspringsPopulation.size(); j++) {
		vectorPopulation.push_back(vectorSelectedOffspringsPopulation[j]);
	}
}

void TravelingSalespersonProblem::updateTheBestSoFarOffspringValue() {
	if (bestFitnessSoFar == 0) {
		bestFitnessSoFar = mapChromosomeFitness[vectorPopulation[0]];
	}
	else if (mapChromosomeFitness[vectorPopulation[0]] < bestFitnessSoFar) {
		bestFitnessSoFar = mapChromosomeFitness[vectorPopulation[0]];
	}
}

void TravelingSalespersonProblem::printOutValueOfBestOffspring(std::ofstream &foutput) {
	std::ostringstream s;
	s << mapChromosomeFitness[vectorPopulation[0]];
	std::string chromosome = "";

	for (std::vector<int>::size_type i = 0; i != vectorPopulation[0].size(); i++) {
		chromosome += convertToString(vectorPopulation[0][i]);
		chromosome += ",";
	}
	RESULTS += s.str() + " (" + chromosome + ")";
	RESULTS += " ";
	//std::cout << mapChromosomeFitness[vectorPopulation[0]] << std::endl;
	foutput << mapChromosomeFitness[vectorPopulation[0]] << " (" + chromosome + ")" << std::endl;
}

void TravelingSalespersonProblem::createInitialPopulationRandomly(){
	if (numOfCities < 3000) {
		int numOfPermutations = factorial(numOfCities);
		if (numOfPermutations != 0 && numOfPermutations < numOfChromosomes) {
			numOfChromosomes = numOfPermutations;
		}
	}
	while (vectorPopulation.size() < numOfChromosomes)
	{
		vectorChromosome = createSolution();
		iteratorVector = std::find(vectorPopulation.begin(), vectorPopulation.end(), vectorChromosome);

		//Did not find, push it in
		if (iteratorVector == vectorPopulation.end())
			vectorPopulation.push_back(vectorChromosome);
	}
}

void TravelingSalespersonProblem::drawBestFitnessSoFarAndGeneration(int i) {
	stringBestFitnessSoFar += convertToString(bestFitnessSoFar);
	stringGeneration += convertToString(i);
	const int X_TEXT_POS = 10;
	const int Y_TEXT_POS = 0;
	const char * charBestFitnessSoFar = stringBestFitnessSoFar.c_str();
	const char * charGeneration = stringGeneration.c_str();
	gui.drawText(charBestFitnessSoFar, X_TEXT_POS, Y_TEXT_POS + 30);
	gui.drawText(charGeneration, X_TEXT_POS, Y_TEXT_POS);
	stringBestFitnessSoFar = "Best solution: ";
	stringGeneration = "Generations: ";
}

std::string TravelingSalespersonProblem::convertToString(int input) {
	char sizeTemp = (char) input;
	char bufferTemp[sizeof(sizeTemp) / sizeof(char) +10];
	return itoa(input, bufferTemp, 10);
};

void TravelingSalespersonProblem::selectMutationMethod() {
	switch (MUTATION_METHOD)
	{
	case 1:
		mutation.mutateInverse(vectorSelectedOffspringsPopulation);
		break;
	default:
		exit(0);
		break;
	}
}

void TravelingSalespersonProblem::selectCrossOverMethod() {
	switch (CROSS_OVER_METHOD)
	{
	case 1:
		crossOver.singlePoint(vectorSelectedOffspringsPopulation, vectorPopulation);
		break;
	case 2:
		crossOver.positionBased(vectorSelectedOffspringsPopulation, vectorPopulation);
		break;
	case 3:
		crossOver.order(vectorSelectedOffspringsPopulation, vectorPopulation);
		break;
	default:
		exit(0);
		break;
	}
}

void TravelingSalespersonProblem::selectSelectionMethod() {
	switch (SELECTION_METHOD)
	{
	case 1:
		selection.elitism(vectorPopulation, mapChromosomeFitness, vectorSelectedOffspringsPopulation);
		break;
	case 2:
		for (int i = 0; i < numOfChromosomes; i++) {
			selection.rouletteWheel(vectorPopulation, mapChromosomeFitness, vectorSelectedOffspringsPopulation);
		}
		break;
	default:
		exit(0);
		break;
	}
}

void TravelingSalespersonProblem::makeSurePopulationHasAtLeastTwoParents() {
	while (vectorPopulation.size() < 2)
	{
		//New blood
		vectorChromosome = createSolution();
		iteratorVector = std::find(vectorPopulation.begin(), vectorPopulation.end(), vectorChromosome);

		//Did not find, push it in
		if (iteratorVector == vectorPopulation.end())
			vectorPopulation.push_back(vectorChromosome);
	}
}

void TravelingSalespersonProblem::randomPlotting() {
	for (int i = 0; i < numOfCities; i++) {
		vectorWidth.push_back(i);
	}
	for (int i = 0; i < numOfCities; i++) {
		vectorHeight.push_back(i + 70);
	}
	for (int i = 0; i < numOfCities; i++)
	{
		cities[i] = i;

		std::random_shuffle(vectorWidth.begin(), vectorWidth.end());
		std::random_shuffle(vectorHeight.begin(), vectorHeight.end());
		x[i] = vectorWidth[0];
		y[i] = vectorHeight[0];
		vectorWidth.erase(vectorWidth.begin());
		vectorHeight.erase(vectorHeight.begin());

		mapX[cities[i]] = x[i];
		mapY[cities[i]] = y[i];
	}
}

void TravelingSalespersonProblem::circlePlotting() {
	const int ORIGIN_X = 300;
	const int ORIGIN_Y = 250;
	const int RADIUS = 200;
	const double PI = 3.14159265358979323846;
	for (int i = 0; i < numOfCities; i++)
	{
		cities[i] = i;

		x[i] = ORIGIN_X + cos(((2 * PI) / numOfCities) * i) * RADIUS;
		y[i] = ORIGIN_Y + sin(((2 * PI) / numOfCities) * i) * RADIUS;

		mapX[cities[i]] = x[i];
		mapY[cities[i]] = y[i];
	}
}

void TravelingSalespersonProblem::fixedPlotting() {
	std::string stringX = "";
	std::string stringY = "";
	std::string coordinate = "";
	int i = 0;
	int j = 0;

	for (int k = 0; k < _coordinates.length(); k++) {
		if (_coordinates[k] == ' ') {
			while (coordinate[j] != ','){
				stringX += coordinate[j];
				j++;
			}
			j += 1;
			for (j; j < coordinate.length(); j++){
				stringY += coordinate[j];
			}

			cities[i] = i;

			x[i] = atoi(stringX.c_str());
			y[i] = atoi(stringY.c_str());

			mapX[cities[i]] = x[i];
			mapY[cities[i]] = y[i];

			stringX = "";
			stringY = "";
			coordinate = "";
			j = 0;
			i++;
		}
		else {
			coordinate += _coordinates[k];
		}
	}
}

std::vector<int> TravelingSalespersonProblem::createSolution()
{
	std::vector<int> vectorChromosome;

	for (int i = 0; i < numOfCities; i++)
		vectorChromosome.push_back(cities[i]);

	//Random shuffle the vector elements
	std::random_shuffle(vectorChromosome.begin(), vectorChromosome.end());

	return vectorChromosome;
}

int TravelingSalespersonProblem::factorial(int n)
{
	return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

TravelingSalespersonProblem::~TravelingSalespersonProblem(void)
{
	delete [] cities;
	delete [] x;
	delete [] y;
}
