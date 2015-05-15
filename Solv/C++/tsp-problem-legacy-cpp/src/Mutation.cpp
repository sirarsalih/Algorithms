#include "Mutation.h"

Mutation::Mutation(void)
{
	mutationProbability[0] = 1;
	mutationProbability[1] = 0;
	mutationProbability[2] = 0;
	mutationProbability[3] = 0;
	mutationProbability[4] = 0;
	mutationProbability[5] = 0;
	mutationProbability[6] = 0;
	mutationProbability[7] = 0;
	mutationProbability[8] = 0;
	mutationProbability[9] = 0;
	randomNum = 0;
}

void Mutation::mutateInverse(std::vector<std::vector<int>> &vectorOffspringsPopulation)
{
	//std::cout << "Mutating 1 population, inversion....." << std::endl;

	//Iterator vector, to be able to find stuff
	std::vector<std::vector<int>>::iterator iteratorVector;
	std::vector<int> vectorChromosome;
	int randomBeginIndex = 0;
	int randomEndIndex = 0;
	int counter = 0;

	//Inverse a random selected sequence in a chromosome in population
	for(int i = 0; i < vectorOffspringsPopulation.size(); i++)
	{		
		//Mutation probability
		randomNum = mutationProbability[rand() % sizeof(mutationProbability)/sizeof(int)];

		if(randomNum == 1)
		{
			vectorChromosome = vectorOffspringsPopulation[i];
		
			randomBeginIndex = rand() % vectorChromosome.size();

			std::vector<int> vectorIndex;
			vectorIndex.push_back(0);		//Push back dummy element (needed to do this because compiler was complaining about integer zero division)

			//Push all values bigger than randomBeginIndex up to 7 (into vectorIndex) to make randomEndIndex
			for(int i = 1; i <= (vectorChromosome.size() - 1) - randomBeginIndex; i++)
			{
				vectorIndex.pop_back();		//Pop dummy element
				vectorIndex.push_back(randomBeginIndex + i);
			}

			randomEndIndex = vectorIndex[rand() % vectorIndex.size()];
		
			//Create a vector sequence which will be filled and reversed
			std::vector<int> vectorSequence;

			for(int k = 0; k <= (randomEndIndex - randomBeginIndex); k++)
				vectorSequence.push_back(vectorChromosome[randomBeginIndex + k]);

			std::reverse(vectorSequence.begin(), vectorSequence.end());

			for(int j = randomBeginIndex; j <= randomEndIndex; j++)
			{
				vectorChromosome[j] = vectorSequence[counter];
				counter++;
			}

			iteratorVector = std::find(vectorOffspringsPopulation.begin(), vectorOffspringsPopulation.end(), vectorChromosome);

			//Did not find, apply change
			if(iteratorVector == vectorOffspringsPopulation.end())
				vectorOffspringsPopulation[i] = vectorChromosome;

			counter = 0;
		}
	}
}


Mutation::~Mutation(void)
{
}
