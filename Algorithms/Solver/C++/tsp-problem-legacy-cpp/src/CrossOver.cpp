#include "CrossOver.h"

CrossOver::CrossOver(void)
{
	crossOverProbability[0] = 1;
	crossOverProbability[1] = 1;
	crossOverProbability[2] = 1;
	crossOverProbability[3] = 1;
	crossOverProbability[4] = 1;
	crossOverProbability[5] = 1;
	crossOverProbability[6] = 0;
	crossOverProbability[7] = 0;
	crossOverProbability[8] = 0;
	crossOverProbability[9] = 0;
	randomNum = 0;
	randomNumOfGenes = 0;
	numberOfChildren = 2;
	gene = 0;
	stringGene = "";
	randomBeginIndex = 0;
	randomEndIndex = 0;
}

void CrossOver::singlePoint(std::vector<std::vector<int>> vectorPopulation, std::vector<std::vector<int>> &vectorOffspringsPopulation)
{
	//std::cout << "Cross over 1 population, single point....." << std::endl;

	int beginIndex = 0;
	int randomEndIndex = 0;
	bool check = false;	
	std::vector<int> vectorIndex;
	std::vector<std::vector<int>>::iterator iteratorVector;

	if(vectorPopulation[0].size() == 2)
	{
		randomEndIndex = 0;
		check = true;
	}
	else if(vectorPopulation[0].size() == 3)
	{
		randomEndIndex = 1;
		check = true;
	}
	else
	{
		for(int i = 1; i <= vectorPopulation[0].size() - 2; i++)
			vectorIndex.push_back(i);
		check = false;
	}

	//Crossover process (Order crossover)
	for(int i = 0; i < vectorPopulation.size(); i++)
	{
		//Crossover probability
		randomNum = crossOverProbability[rand() % sizeof(crossOverProbability)/sizeof(int)];

		//Crossover
		if(randomNum == 1)
		{
			/**Parents**/
			vectorChromosomeParent1 = vectorPopulation[i];

			if(i == vectorPopulation.size() - 1)
				vectorChromosomeParent2 = vectorPopulation[0];
			else
				vectorChromosomeParent2 = vectorPopulation[i + 1];

			int vectorPopulationSize = vectorPopulation[0].size();
			vectorPopulationSize -= 2;

			if(!check)
				randomEndIndex = vectorIndex[rand() % vectorPopulationSize];

			/**Create children**/
			for(int i = 0; i < numberOfChildren; i++)
			{
				singlePointAssistance(beginIndex, randomEndIndex, vectorChromosomeParent1, vectorChromosomeParent2, vectorChromosomeChild);

				iteratorVector = std::find(vectorOffspringsPopulation.begin(), vectorOffspringsPopulation.end(), vectorChromosomeChild);
		
				//Did not find, push it in offspring population
				if(iteratorVector == vectorOffspringsPopulation.end())
					vectorOffspringsPopulation.push_back(vectorChromosomeChild);		

				//Clear
				vectorChromosomeChild.clear();
			}
		}

		//No crossover
		else
		{
			iteratorVector = std::find(vectorOffspringsPopulation.begin(), vectorOffspringsPopulation.end(), vectorPopulation[i]);

			//Did not find, push it in offspring population
			if(iteratorVector == vectorOffspringsPopulation.end())
				vectorOffspringsPopulation.push_back(vectorPopulation[i]);
		}
	}
}

void CrossOver::singlePointAssistance(int beginIndex, int randomEndIndex, std::vector<int> vectorChromosomeParent1, std::vector<int> vectorChromosomeParent2, std::vector<int> &vectorChromosomeChild)
{
	randomNum = rand () % 2;

	//DNA from parent 1, then parent 2
	if(randomNum == 0)
	{
		//Child inherents some DNA from Parent 1...
		for(int j = beginIndex; j <= randomEndIndex; j++)
			vectorChromosomeChild.push_back(vectorChromosomeParent1[j]);

		//...and from Parent 2
		for(int i = 0;  i < vectorChromosomeParent2.size(); i++)
		{
			iteratorVectorInteger = std::find(vectorChromosomeChild.begin(), vectorChromosomeChild.end(), vectorChromosomeParent2[i]);

			//Did not find, push it in child
			if(iteratorVectorInteger == vectorChromosomeChild.end())
				vectorChromosomeChild.push_back(vectorChromosomeParent2[i]);
		}
	}

	//DNA from parent 2, then parent 1
	else
	{		
		//Child inherents some DNA from Parent 2...
		for(int j = beginIndex; j <= randomEndIndex; j++)
			vectorChromosomeChild.push_back(vectorChromosomeParent2[j]);

		//...and from Parent 1
		for(int i = 0;  i < vectorChromosomeParent1.size(); i++)
		{
			iteratorVectorInteger = std::find(vectorChromosomeChild.begin(), vectorChromosomeChild.end(), vectorChromosomeParent1[i]);

			//Did not find, push it in child
			if(iteratorVectorInteger == vectorChromosomeChild.end())
				vectorChromosomeChild.push_back(vectorChromosomeParent1[i]);
		}
	}
}

void CrossOver::twoPoint(std::vector<std::vector<int>> vectorPopulation, std::vector<std::vector<int>> &vectorOffspringsPopulation)
{

}

void CrossOver::positionBased(std::vector<std::vector<int>> vectorPopulation, std::vector<std::vector<int>> &vectorOffspringsPopulation)
{
	//Crossover process (Position based crossover)
	for(int k = 0; k < vectorPopulation.size(); k++)
	{
		//Crossover probability
		randomNum = crossOverProbability[rand() % sizeof(crossOverProbability)/sizeof(int)];

		//Crossover
		if(randomNum == 1)
		{
			/**Create children**/
			for(int i = 0; i < numberOfChildren; i++)
			{
				/**Parents**/
				vectorChromosomeParent1 = vectorPopulation[k];

				if(k == vectorPopulation.size() - 1)
					vectorChromosomeParent2 = vectorPopulation[0];
				else
					vectorChromosomeParent2 = vectorPopulation[k + 1];

				//Construct child skeleton
				for(int j = 0; j < vectorChromosomeParent1.size(); j++)
					vectorStringChromosomeChild.push_back("#");

				//Maximum 50 % from Parent 1
				randomNumOfGenes = vectorChromosomeParent1.size() / 2;
				
				for(int s = 0; s < randomNumOfGenes; s++)
				{
					//Pick a random gene
					randomNum = rand () % vectorChromosomeParent1.size();
					gene = vectorChromosomeParent1[randomNum];

					//int -> string
					char sizeTemp = (char)gene;
					char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
					stringGene = itoa(gene, bufferTemp, 10);

					iteratorVectorString = std::find(vectorStringChromosomeChild.begin(), vectorStringChromosomeChild.end(), stringGene);
						
					//Did not find, push it in child
					if(iteratorVectorString == vectorStringChromosomeChild.end())
						vectorStringChromosomeChild[randomNum] = stringGene;
				}

				//The rest from Parent 2
				for(int s = 0; s < vectorStringChromosomeChild.size(); s++)
				{
					if(vectorStringChromosomeChild[s] == "#")
					{
						for(int d = 0; d < vectorChromosomeParent2.size(); d++)
						{
							gene = vectorChromosomeParent2[d];

							//int -> string
							char sizeTemp = (char)gene;
							char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
							stringGene = itoa(gene, bufferTemp, 10);

							iteratorVectorString = std::find(vectorStringChromosomeChild.begin(), vectorStringChromosomeChild.end(), stringGene);
						
							//Did not find, push it in child
							if(iteratorVectorString == vectorStringChromosomeChild.end())
							{
								vectorStringChromosomeChild[s] = stringGene;
								break;
							}
						}
					}
				}

				//string -> int
				for(int s = 0; s < vectorStringChromosomeChild.size(); s++)
					vectorChromosomeChild.push_back(atoi(vectorStringChromosomeChild[s].c_str()));

				iteratorVector = std::find(vectorOffspringsPopulation.begin(), vectorOffspringsPopulation.end(), vectorChromosomeChild);

				//Did not find, push it in offspring population
				if(iteratorVector == vectorOffspringsPopulation.end())
					vectorOffspringsPopulation.push_back(vectorChromosomeChild);

				//Clear
				vectorStringChromosomeChild.clear();

				//Clear
				vectorChromosomeChild.clear();
			}
			
		}
		
		//No crossover
		else
		{
			iteratorVector = std::find(vectorOffspringsPopulation.begin(), vectorOffspringsPopulation.end(), vectorPopulation[k]);

			//Did not find, push it in offspring population
			if(iteratorVector == vectorOffspringsPopulation.end())
				vectorOffspringsPopulation.push_back(vectorPopulation[k]);
		}
	}
}

void CrossOver::order(std::vector<std::vector<int>> vectorPopulation, std::vector<std::vector<int>> &vectorOffspringsPopulation)
{
	//Iterator vectors, to be able to find stuff
	std::vector<std::vector<int>>::iterator iteratorVector;
	std::vector<int>::iterator iteratorVectorInteger;
	std::vector<std::string>::iterator iteratorVectorString;

	//Crossover process (Order crossover)
	for(int k = 0; k < vectorPopulation.size(); k++)
	{
		//Crossover probability
		randomNum = crossOverProbability[rand() % sizeof(crossOverProbability)/sizeof(int)];

		//Crossover
		if(randomNum == 1)
		{
			/**Create children**/
			for(int i = 0; i < numberOfChildren; i++)
			{
				/**Parents**/
				vectorChromosomeParent1 = vectorPopulation[k];

				if(k == vectorPopulation.size() - 1)
					vectorChromosomeParent2 = vectorPopulation[0];
				else
					vectorChromosomeParent2 = vectorPopulation[k + 1];

				//Construct child skeleton
				for(int j = 0; j < vectorChromosomeParent1.size(); j++)
					vectorStringChromosomeChild.push_back("#");

				//From Parent 1
				randomBeginIndex = rand() % vectorChromosomeParent1.size();
				randomEndIndex = rand() % vectorChromosomeParent1.size();

				while(randomEndIndex < randomBeginIndex)
					randomEndIndex = rand() % vectorChromosomeParent1.size();
				
				for(int s = randomBeginIndex; s <= randomEndIndex; s++)
				{
					gene = vectorChromosomeParent1[s];

					//int -> string
					char sizeTemp = (char)gene;
					char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
					stringGene = itoa(gene, bufferTemp, 10);

					//Put the gene from parent into child
					vectorStringChromosomeChild[s] = stringGene;
				}

				//The rest from Parent 2
				for(int s = 0; s < vectorStringChromosomeChild.size(); s++)
				{
					if(vectorStringChromosomeChild[s] == "#")
					{
						for(int d = 0; d < vectorChromosomeParent2.size(); d++)
						{
							gene = vectorChromosomeParent2[d];

							//int -> string
							char sizeTemp = (char)gene;
							char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
							stringGene = itoa(gene, bufferTemp, 10);

							iteratorVectorString = std::find(vectorStringChromosomeChild.begin(), vectorStringChromosomeChild.end(), stringGene);
						
							//Did not find, push it in child
							if(iteratorVectorString == vectorStringChromosomeChild.end())
							{
								vectorStringChromosomeChild[s] = stringGene;
								break;
							}
						}
					}
				}

				//string -> int
				for(int s = 0; s < vectorStringChromosomeChild.size(); s++)
					vectorChromosomeChild.push_back(atoi(vectorStringChromosomeChild[s].c_str()));

				iteratorVector = std::find(vectorOffspringsPopulation.begin(), vectorOffspringsPopulation.end(), vectorChromosomeChild);

				//Did not find, push it in offspring population
				if(iteratorVector == vectorOffspringsPopulation.end())
					vectorOffspringsPopulation.push_back(vectorChromosomeChild);

				//Clear
				vectorStringChromosomeChild.clear();

				//Clear
				vectorChromosomeChild.clear();
			}
			
		}
		
		//No crossover
		else
		{
			iteratorVector = std::find(vectorOffspringsPopulation.begin(), vectorOffspringsPopulation.end(), vectorPopulation[k]);

			//Did not find, push it in offspring population
			if(iteratorVector == vectorOffspringsPopulation.end())
				vectorOffspringsPopulation.push_back(vectorPopulation[k]);
		}
	}
}

CrossOver::~CrossOver(void)
{
}
