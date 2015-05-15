#include "Selection.h"


Selection::Selection(void)
{
}

void Selection::rouletteWheel(std::vector<std::vector<int>> vectorOffspringsPopulation, std::map<std::vector<int>, double> mapChromosomeFitness, std::vector<std::vector<int>> &vectorSelectedOffspringsPopulation)
{
	double totalProbability = 0;		//Total probability of selecting individuals
	double totalFitness = 0;
	double probabilities[] = {0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
	double randomProbability = 0;
	std::vector<std::vector<int>>::iterator iteratorVector;
	std::map<double, std::vector<int>> mapTempFitnessChromosome;

	//Sum fitness of this population
	for(int i = 0; i < vectorOffspringsPopulation.size(); i++)
	{
		totalFitness += mapChromosomeFitness[vectorOffspringsPopulation[i]];

		//[Fitness][Chromosome]
		mapTempFitnessChromosome[mapChromosomeFitness[vectorOffspringsPopulation[i]]] = vectorOffspringsPopulation[i];
	}

	for(std::map<double, std::vector<int>>::const_iterator i = mapTempFitnessChromosome.begin(); i != mapTempFitnessChromosome.end(); ++i)
	{
		totalProbability += i->first / totalFitness;

		randomProbability = probabilities[rand() % sizeof(probabilities)/sizeof(double)];

		if(totalProbability >= randomProbability)
		{
			//See if we have the chromosome already
			iteratorVector = std::find(vectorSelectedOffspringsPopulation.begin(), vectorSelectedOffspringsPopulation.end(), i->second);
			
			//Did not find, push it in
			if(iteratorVector == vectorSelectedOffspringsPopulation.end())
			{				
				//std::cout << "Selecting offspring from 1 population (10 pr. population), roulette wheel....." << std::endl;
				vectorSelectedOffspringsPopulation.push_back(i->second);
				break;
			}
		}
	}
}

void Selection::elitism(std::vector<std::vector<int>> vectorOffspringsPopulation, std::map<std::vector<int>, double> mapChromosomeFitness, std::vector<std::vector<int>> &vectorSelectedOffspringsPopulation)
{
	std::map<double, std::vector<int>> mapTempFitnessChromosome;
	int counter = 0;

	//[Fitness][Chromosome]
	for(int i = 0; i < vectorOffspringsPopulation.size(); i++)
		mapTempFitnessChromosome[mapChromosomeFitness[vectorOffspringsPopulation[i]]] = vectorOffspringsPopulation[i];
	
	/**Take the top best chromosomes**/
	for(std::map<double, std::vector<int>>::const_iterator i = mapTempFitnessChromosome.begin(); i != mapTempFitnessChromosome.end(); ++i)
	{
		vectorSelectedOffspringsPopulation.push_back(i->second);
		counter++;
		
		if(counter == 2)
			break;
	}
}

Selection::~Selection(void)
{
}
