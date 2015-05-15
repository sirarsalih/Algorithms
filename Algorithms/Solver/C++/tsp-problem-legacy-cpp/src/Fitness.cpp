#include "Fitness.h"

Fitness::Fitness(void)
{
	x0Position = 0;
	y0Position = 0;
	x1Position = 0;
	y1Position = 0;
	xDistance = 0;
	yDistance = 0;
	distance = 0;
	totalDistance = 0;		//Total distance of a chromosome in a population: sqrt(city a....city b) + sqrt(city b....city c)...
}

void Fitness::calculateFitness(std::vector<std::vector<int>> vectorPopulation, std::map<std::vector<int>, double> &mapChromosomeFitness)
{
	//std::cout << "Calculating fitness 1 population....." << std::endl;

	/**Fitness calculation**/
	std::vector<int> vectorChromosome;

	for(int i = 0; i < vectorPopulation.size(); i++)
	{		
		vectorChromosome = vectorPopulation[i];		//Chromosome

		for(int j = 0; j < vectorChromosome.size(); j++)
		{
			/**Distance calculation between 2 given cities in a chromosome**/
			x0Position = TravelingSalespersonProblem::mapX[vectorChromosome[j]]; 			
			y0Position = TravelingSalespersonProblem::mapY[vectorChromosome[j]]; 

			if(j == vectorChromosome.size() - 1)
			{
				x1Position = TravelingSalespersonProblem::mapX[vectorChromosome[0]];
				y1Position = TravelingSalespersonProblem::mapY[vectorChromosome[0]];
			}
			else
			{
				x1Position = TravelingSalespersonProblem::mapX[vectorChromosome[j + 1]];
				y1Position = TravelingSalespersonProblem::mapY[vectorChromosome[j + 1]];
			}

			xDistance = x0Position - x1Position;
			yDistance = y0Position - y1Position;

			if(xDistance < 0)
				xDistance *= -1;

			if(yDistance < 0)
				yDistance *= -1;

			distance = sqrt((xDistance * xDistance) + (yDistance * yDistance));

			totalDistance += distance;
		}

		//Map[Chromosome][Fitness]
		mapChromosomeFitness[vectorChromosome] = totalDistance;

		//Clear
		totalDistance = 0;
	}
}


Fitness::~Fitness(void)
{
}
