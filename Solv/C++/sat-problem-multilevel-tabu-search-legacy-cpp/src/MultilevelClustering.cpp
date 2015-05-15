#include "MultilevelClustering.h"

MultilevelClustering::MultilevelClustering(void)
{
}

std::vector<int> MultilevelClustering::initializeNumbers(std::string numbersString, int &numberOfLiterals)
{
	std::cout << "Initializing literals....." << std::endl;

	std::vector<char> vectorCharacters;
	std::vector<std::string> vectorString;
	std::vector<int> vectorNumbers;
	std::string stringNumbers = "";
	int counter = 2;
	int counterSpace = 0;
	int helpCounter = 0;

	for(int i = 0; i < numbersString.length(); i++)
	{
		while(numbersString[i] == '0' && numbersString[i - 1] == ' ')
		{
			helpCounter++;

			//"#"
			if(numbersString[i - counter] == '#')
			{
				vectorCharacters.push_back(numbersString[i - 1]);
				counter = 2;
				break;
			}

			//"  0"
			if(numbersString[i - counter] == '0' && numbersString[i - counter - 1] == ' ')
			{
				vectorCharacters.push_back(numbersString[i - 1]);
				counter = 2;
				break;
			}

			if(helpCounter == 1)
			{
				vectorCharacters.push_back(numbersString[i]);
				vectorCharacters.push_back(numbersString[i - 1]);
			}

			else
			{
				vectorCharacters.push_back(numbersString[i - counter]);
				counter++;
			}
		}
		helpCounter = 0;
		counter = 2;
	}

	//Char -> string
	for(int i = 0; i < vectorCharacters.size(); i++)
	{
		if(vectorCharacters[i] == ' ')
			counterSpace++;

		//Found a space
		if(counterSpace == 1)
		{
			vectorString.push_back(stringNumbers);

			stringNumbers = "";
			counterSpace = 0;
		}

		if(vectorCharacters[i] != ' ')
			stringNumbers += vectorCharacters[i];
	}

	//Reverse the string numbers properly
	for(int i = 0; i < vectorString.size(); i++)
		std::reverse(vectorString[i].begin(), vectorString[i].end());

	//Convert to integer values
	for(int i = 0; i < vectorString.size(); i++)
	{
		if(vectorString[i] != "")
			vectorNumbers.push_back(atoi(vectorString[i].c_str()));
	}

	//Push in the last number (ending number 0)
	vectorNumbers.push_back(0);

	//Get the number of literals
	numberOfLiterals = vectorNumbers.size();

	return vectorNumbers;
}

void MultilevelClustering::initializeDataStructure(std::vector<std::string> &vectorStringNumbers, std::vector<int> vectorNumbers, std::multimap<std::string, std::vector<std::string>> &multiMapLiteralClause)
{
	std::cout << "Initializing data structure....." << std::endl << std::endl;

	std::string clause = "";
	std::string stringLiteral = "";
	std::vector<std::string> vectorClause;

	//Create a vector string numbers
	for(int i = 0; i < vectorNumbers.size(); i++)
	{
		char sizeNumber = (char)vectorNumbers[i];
		char buffer[sizeof(sizeNumber)/sizeof(char) + 10];
		std::string stringNumber = itoa(vectorNumbers[i], buffer, 10);

		if(stringNumber == "0")
			vectorStringNumbers.push_back("|");
		else
			vectorStringNumbers.push_back(stringNumber);				
	}

	//Remove the first "|"
	vectorStringNumbers.erase(vectorStringNumbers.begin());

	for(int i = 0; i < vectorStringNumbers.size(); i++)
	{
		if(vectorStringNumbers[i] == "|")
		{
			//Reverse vector
			std::reverse(vectorClause.begin(), vectorClause.end());

			if(clause[clause.length() - 1] == ' ')
				clause.erase(clause.length() - 1);

			for(int j = 0; j < clause.length(); j++)
			{
				if(clause[j] != ' ')
					stringLiteral += clause[j];

				if(clause[j] == ' ' || j == clause.length() - 1)
				{
					if(stringLiteral != "")
					{
						//Insert literal and its vector clause into map
						multiMapLiteralClause.insert(std::pair<std::string, std::vector<std::string>>(stringLiteral, vectorClause));
					}

					//Reset
					stringLiteral = "";
				}
			}

			//Reset
			clause = "";

			//Reset
			vectorClause.clear();
		}

		else if(vectorStringNumbers[i] != "|")
		{
			vectorClause.push_back(vectorStringNumbers[i]);
			clause += vectorStringNumbers[i];
			clause += " ";
		}
	}
}

void MultilevelClustering::cluster(std::ofstream &foutput, int &bestSoFar, std::string numberOfXLiterals, int &levelCounter, std::map<std::string, std::string> &mapLevelClusters, std::vector<std::string> vectorStringNumbers, int numberOfLiterals, std::string numberOfClauses, std::vector<std::string> &initialClusterCollection, std::vector<std::string> &clusterCollection, std::map<std::string, bool> &mapLiteralValues, std::multimap<std::string, bool> &mapClauseValues, clock_t begin)
{
	std::cout << "Running Multilevel clustering....." << std::endl << std::endl;

	std::cout << "Clustering literals randomly....." << std::endl << std::endl;

	const int NUMBER_OF_LITERALS = atoi(numberOfXLiterals.c_str());
	const int NUMBER_OF_CLAUSES = atoi(numberOfClauses.c_str());
	TimeElapsed te;
	double timeElapsedMultilevelClustering = 0;
	clock_t end = 0;
	std::string satisfaction = "";
	int trueClauseCounter = 0;
	bool literalValues[2] = {true, false};
	int entranceCounter = 0;
	int entranceCounter2 = 0;
	std::string cluster;
	std::vector<int> vectorVariable;
	int randomVariableOne = 0;
	int oppositeValue = 0;
	int levelIndex = 0;
	std::vector<std::string> vectorCheckedClauses;

	//Iterator vector, to help find elements
	std::vector<std::string>::iterator iteratorVector;

	//Iterator map, to be able to find stuff
	std::map<std::string, bool>::iterator iteratorMap;

	//Iterator vector, to help find elements
	std::vector<std::string>::iterator iteratorVectorString;

	bool firstEntrance = true;

	while(firstEntrance || clusterCollection.size() >= atoi(numberOfXLiterals.c_str()) * 0.10)
	{
		firstEntrance = false;

		if(entranceCounter == 0)
		{
			entranceCounter++;
			std::cout << "LEVEL " << levelCounter << std::endl << std::endl;

			for(int i = 1; i <= NUMBER_OF_LITERALS; i++)
			{
				vectorVariable.push_back(i);

				//int -> string
				char sizeTemp = (char)i;
				char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
				std::string tempLiteral = itoa(i, bufferTemp, 10);

				//int -> string
				char sizeTemp2 = (char)levelCounter;
				char bufferTemp2[sizeof(sizeTemp2)/sizeof(char) + 10];
				std::string stringLevelCounter = itoa(levelCounter, bufferTemp2, 10);

				//int -> string
				char size3 = (char)levelIndex;
				char buffer3[sizeof(size3)/sizeof(char) + 10];
				std::string stringLevelIndex = itoa(levelIndex, buffer3, 10);

				//Update the map level with literals
				mapLevelClusters[stringLevelCounter + " " + stringLevelIndex] = tempLiteral;

				//Increment level index
				levelIndex++;
			}

			/**Stop timer**/
			end = clock();

			timeElapsedMultilevelClustering = te.GetTimeElapsed(end, begin)/1000;

			//Level 0 complete
			std::cout << "Time used: " << timeElapsedMultilevelClustering << " seconds" << std::endl << std::endl;
			
			//Reset
			levelIndex = 0;

			levelCounter++;
			std::cout << "LEVEL " << levelCounter << std::endl << std::endl;

			//Cluster and create LEVEL 1 (Initial cluster collection)
			while(vectorVariable.size() != 0)
			{
				if(vectorVariable.size() == 1)
					break;

				//Random shuffle
				std::random_shuffle(vectorVariable.begin(), vectorVariable.end());

				char sizeVariableOne = (char)vectorVariable[0];
				char bufferVariableOne[sizeof(sizeVariableOne)/sizeof(char) + 10];
				cluster += itoa(vectorVariable[0], bufferVariableOne, 10);
				cluster += " ";

				char sizeVariableTwo = (char)vectorVariable[1];
				char bufferVariableTwo[sizeof(sizeVariableTwo)/sizeof(char) + 10];
				cluster += itoa(vectorVariable[1], bufferVariableTwo, 10);
				cluster += " ";

				while(cluster[0] == ' ')
					cluster.erase(cluster.begin());

				while(cluster[cluster.length() - 1] == ' ')
					cluster.erase(cluster.end() - 1);

				initialClusterCollection.push_back(cluster);

				//Erase
				vectorVariable.erase(vectorVariable.begin());
				vectorVariable.erase(vectorVariable.begin());

				//int -> string
				char sizeTemp2 = (char)levelCounter;
				char bufferTemp2[sizeof(sizeTemp2)/sizeof(char) + 10];
				std::string stringLevelCounter = itoa(levelCounter, bufferTemp2, 10);

				//int -> string
				char size3 = (char)levelIndex;
				char buffer3[sizeof(size3)/sizeof(char) + 10];
				std::string stringLevelIndex = itoa(levelIndex, buffer3, 10);

				//Update the map level with cluster
				mapLevelClusters[stringLevelCounter + " " + stringLevelIndex] = cluster;

				//Increment level index
				levelIndex++;

				//Reset
				cluster = "";
			}

			//Push the rest into initialClusterCollection
			if(vectorVariable.size() == 1)
			{				
				char sizeVariableOne = (char)vectorVariable[0];
				char bufferVariableOne[sizeof(sizeVariableOne)/sizeof(char) + 10];
				cluster = itoa(vectorVariable[0], bufferVariableOne, 10);

				while(cluster[0] == ' ')
					cluster.erase(cluster.begin());

				while(cluster[cluster.length() - 1] == ' ')
					cluster.erase(cluster.end() - 1);

				initialClusterCollection.push_back(cluster);

				//Erase
				vectorVariable.erase(vectorVariable.begin());

				//int -> string
				char sizeTemp2 = (char)levelCounter;
				char bufferTemp2[sizeof(sizeTemp2)/sizeof(char) + 10];
				std::string stringLevelCounter = itoa(levelCounter, bufferTemp2, 10);

				//int -> string
				char size3 = (char)levelIndex;
				char buffer3[sizeof(size3)/sizeof(char) + 10];
				std::string stringLevelIndex = itoa(levelIndex, buffer3, 10);

				//Update the map level with cluster
				mapLevelClusters[stringLevelCounter + " " + stringLevelIndex] = cluster;

				//Increment level index
				levelIndex++;
			}

			/**Stop timer**/
			end = clock();

			timeElapsedMultilevelClustering = te.GetTimeElapsed(end, begin)/1000;

			//Level 1 complete
			std::cout << "Time used: " << timeElapsedMultilevelClustering << " seconds" << std::endl << std::endl;			
		}

		//Clear cluster
		cluster = "";

		std::string trueClusters = "";
		std::string falseClusters = "";	
		std::string substring = " ";

		int clusterCounter = 0;		

		//Reset level index
		levelIndex = 0;

		//int -> string
		char sizeTemp = (char)levelCounter;
		char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
		std::string stringLevelCounter = itoa(levelCounter, bufferTemp, 10);

		if(entranceCounter2 == 0)
		{
			entranceCounter2++;
			
			levelCounter++;
			std::cout << "LEVEL " << levelCounter << std::endl << std::endl;

			//int -> string
			char sizeTemp = (char)levelCounter;
			char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
			std::string stringLevelCounter = itoa(levelCounter, bufferTemp, 10);

			//Cluster and create the next level
			while(initialClusterCollection.size() != 0)
			{
				if(initialClusterCollection.size() == 1)
					break;

				//Random shuffle
				std::random_shuffle(initialClusterCollection.begin(), initialClusterCollection.end());

				trueClusters += initialClusterCollection[0];
				trueClusters += " ";
				trueClusters += initialClusterCollection[1];
				trueClusters += " ";

				while(trueClusters[0] == ' ')
					trueClusters.erase(trueClusters.begin());

				while(trueClusters[trueClusters.length() - 1] == ' ')
					trueClusters.erase(trueClusters.end() - 1);

				clusterCollection.push_back(trueClusters);

				//Erase
				initialClusterCollection.erase(initialClusterCollection.begin());
				initialClusterCollection.erase(initialClusterCollection.begin());

				//int -> string
				char sizeTemp = (char)levelIndex;
				char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
				std::string stringLevelIndex = itoa(levelIndex, bufferTemp, 10);

				//Update the map level with cluster
				mapLevelClusters[stringLevelCounter + " " + stringLevelIndex] = trueClusters;

				//Increase level index
				levelIndex++;

				trueClusters = "";
			}

			//Push the rest into cluster collection
			if(initialClusterCollection.size() == 1)
			{
				trueClusters = initialClusterCollection[0];
				
				while(trueClusters[0] == ' ')
					trueClusters.erase(trueClusters.begin());

				while(trueClusters[trueClusters.length() - 1] == ' ')
					trueClusters.erase(trueClusters.end() - 1);

				clusterCollection.push_back(trueClusters);

				//Erase
				initialClusterCollection.erase(initialClusterCollection.begin());

				//int -> string
				char sizeTemp = (char)levelIndex;
				char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
				std::string stringLevelIndex = itoa(levelIndex, bufferTemp, 10);

				//Update the map level with TRUE cluster
				mapLevelClusters[stringLevelCounter + " " + stringLevelIndex] = trueClusters;

				//Increase level index
				levelIndex++;

				clusterCounter = 0;
				trueClusters = "";
			}

			//Reset cluster counter
			clusterCounter = 0;	

			/**Stop timer**/
			end = clock();

			timeElapsedMultilevelClustering = te.GetTimeElapsed(end, begin)/1000;

			//Level 2 complete
			std::cout << "Time used: " << timeElapsedMultilevelClustering << " seconds" << std::endl << std::endl;
		}
		
		levelCounter++;
		std::cout << "LEVEL " << levelCounter << std::endl << std::endl;

		//Reset level index
		levelIndex = 0;

		//int -> string
		char sizeLevelCounter = (char)levelCounter;
		char bufferLevelCounter[sizeof(sizeLevelCounter)/sizeof(char) + 10];
		std::string stringUpdatedLevelCounter = itoa(levelCounter, bufferTemp, 10);

		//Cluster and create the next level
		while(clusterCollection.size() != 0)
		{
			if(clusterCollection.size() == 1)
				break;

			//Random shuffle
			std::random_shuffle(clusterCollection.begin(), clusterCollection.end());

			trueClusters += clusterCollection[0];
			trueClusters += " ";
			trueClusters += clusterCollection[1];
			trueClusters += " ";
		
			substring += clusterCollection[0];
			substring += " ";
			substring += clusterCollection[1];
			substring += " ";

			while(trueClusters[0] == ' ')
				trueClusters.erase(trueClusters.begin());

			while(trueClusters[trueClusters.length() - 1] == ' ')
				trueClusters.erase(trueClusters.end() - 1);

			//Erase
			clusterCollection.erase(clusterCollection.begin());
			clusterCollection.erase(clusterCollection.begin());

			//int -> string
			char sizeTemp = (char)levelIndex;
			char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
			std::string stringLevelIndex = itoa(levelIndex, bufferTemp, 10);

			//Update the map level with TRUE cluster
			mapLevelClusters[stringUpdatedLevelCounter + " " + stringLevelIndex] = trueClusters;

			//Increase level index
			levelIndex++;
					
			substring += "|";
			trueClusters = "";
		}

		if(clusterCollection.size() == 1)
		{
			trueClusters = clusterCollection[0];
		
			substring += clusterCollection[0];

			while(trueClusters[0] == ' ')
				trueClusters.erase(trueClusters.begin());

			while(trueClusters[trueClusters.length() - 1] == ' ')
				trueClusters.erase(trueClusters.end() - 1);

			//Erase
			clusterCollection.erase(clusterCollection.begin());

			//int -> string
			char sizeTemp = (char)levelIndex;
			char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
			std::string stringLevelIndex = itoa(levelIndex, bufferTemp, 10);

			//Update the map level with TRUE cluster
			mapLevelClusters[stringUpdatedLevelCounter + " " + stringLevelIndex] = trueClusters;

			//Increase level index
			levelIndex++;
					
			substring += "|";
			trueClusters = "";
		}

		//Temp cluster
		std::string tempCluster = "";

		//Reset cluster collection
		clusterCollection.clear();

		//Update cluster collection
		for(int i = 0; i < substring.length(); i++)
		{
			if(substring[i] != '|')
				tempCluster += substring[i];
			
			else if(substring[i] == '|')
			{
				while(tempCluster[0] == ' ')
					tempCluster.erase(tempCluster.begin());

				while(tempCluster[tempCluster.length() - 1] == ' ')
					tempCluster.erase(tempCluster.end() - 1);

				clusterCollection.push_back(tempCluster);
				tempCluster = "";
			}
		}

		//Clear substring
		substring = "";

		//Clear tempCluster
		tempCluster = "";

		//Reset cluster counter
		clusterCounter = 0;

		/**Stop timer**/
		end = clock();

		timeElapsedMultilevelClustering = te.GetTimeElapsed(end, begin)/1000;

		//Level X complete
		std::cout << "Time used: " << timeElapsedMultilevelClustering << " seconds" << std::endl << std::endl;

		//We have reached the final level, assign TRUE/FALSE values
		if(clusterCollection.size() <= atoi(numberOfXLiterals.c_str()) * 0.10)
		{
			std::cout << "Assigning the clusters TRUE/FALSE values randomly....." << std::endl << std::endl;

			std::string tempCluster = "";
			std::string tempLiteral = "";
			int randomIndex = 0;
			int levelIndex = 0;		 //Cluster index
			int lowerLevelIndex = 0; //Literal index

			//Assign the clusters random TRUE/FALSE values as well as the literals within the clusters
			for(int j = 0; j < clusterCollection.size(); j++)
			{
				randomIndex = rand () % 2;
				tempCluster = clusterCollection[j];

				while(tempCluster[0] == ' ')
					tempCluster.erase(tempCluster.begin());

				while(tempCluster[tempCluster.length() - 1] == ' ')
					tempCluster.erase(tempCluster.end() - 1);

				mapLiteralValues[tempCluster] = literalValues[randomIndex];	

				//If FALSE, all literals inside must be FALSE
				if(mapLiteralValues[tempCluster] == false)
				{
					for(int k = 0; k < tempCluster.length(); k++)
					{						
						if(tempCluster[k] != ' ')
							tempLiteral += tempCluster[k];

						if(tempCluster[k] == ' ' || k == tempCluster.length() - 1)
						{
							//FALSE literal
							mapLiteralValues[tempLiteral] = literalValues[randomIndex];

							//Opposite value will be TRUE
							int temp = atoi(tempLiteral.c_str()) * -1;
							char sizeTempOpposite = (char)temp;
							char bufferTempOpposite[sizeof(sizeTempOpposite)/sizeof(char) + 10];
							std::string oppositeTempLiteral = itoa(temp, bufferTempOpposite, 10);

							mapLiteralValues[oppositeTempLiteral] = literalValues[randomIndex - 1];
							tempLiteral = "";
						}
					}
				}

				//If TRUE, all literals inside must be TRUE
				else if(mapLiteralValues[tempCluster] == true)
				{
					for(int k = 0; k < tempCluster.length(); k++)
					{						
						if(tempCluster[k] != ' ')
							tempLiteral += tempCluster[k];

						if(tempCluster[k] == ' ' || k == tempCluster.length() - 1)
						{
							//TRUE literal
							mapLiteralValues[tempLiteral] = literalValues[randomIndex];

							//Opposite value will be FALSE
							int temp = atoi(tempLiteral.c_str()) * -1;
							char sizeTempOpposite = (char)temp;
							char bufferTempOpposite[sizeof(sizeTempOpposite)/sizeof(char) + 10];
							std::string oppositeTempLiteral = itoa(temp, bufferTempOpposite, 10);

							mapLiteralValues[oppositeTempLiteral] = literalValues[randomIndex + 1];
							tempLiteral = "";
						}
					}
				}
			}

			//Assign TRUE/FALSE values to the SAT formula
			std::cout << "Assigning TRUE/FALSE values to the clauses of the SAT formula....." << std::endl << std::endl;

			int trueCounter = 0;
			std::string stringClause = "";

			for(int j = 0; j < vectorStringNumbers.size(); j++)
			{
				if(vectorStringNumbers[j] != "|")
				{
					stringClause += vectorStringNumbers[j];
					stringClause += " ";

					if(mapLiteralValues[vectorStringNumbers[j]] == true)
						trueCounter++;
				}
				
				else if(vectorStringNumbers[j] == "|")
				{
					//Erase the space at the end of string
					stringClause.erase(stringClause.end() - 1);

					iteratorVectorString = std::find(vectorCheckedClauses.begin(), vectorCheckedClauses.end(), stringClause);

					if(iteratorVectorString == vectorCheckedClauses.end())
					{
						vectorCheckedClauses.push_back(stringClause);

						if(trueCounter >= 1)
						{
							//Set the clause to TRUE
							mapClauseValues.insert(std::pair<std::string, bool>(stringClause, true));
							trueClauseCounter++;
						}
						
						else if(trueCounter == 0)
						{
							//Set the clause to FALSE
							mapClauseValues.insert(std::pair<std::string, bool>(stringClause, false));
						}				
					}

					//Reset
					stringClause = "";
					trueCounter = 0;
				}
			}
		}
	}

	bestSoFar = trueClauseCounter;

	//Reset
	trueClauseCounter = 0;
	vectorCheckedClauses.clear();

	foutput << "Literals: " << numberOfXLiterals << "  Clauses: " << NUMBER_OF_CLAUSES << "\n\n";
	std::cout << "Number of TRUE clauses (initial solution): " << std::endl << std::endl << bestSoFar << std::endl << std::endl;
	foutput << "Level     Mean satisfied clauses     Mean time     Mean flips\n\n";
	foutput << levelCounter << "                         " << bestSoFar << "                         " << 0 << "                         " << 0 << "\n";

	if(bestSoFar == NUMBER_OF_CLAUSES)
	{
		satisfaction = "SATISFIED";
		std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << " at LEVEL " << levelCounter << "." << std::endl << std::endl;
		//foutput << "LEVEL " << levelCounter << " SATISFIED!\n\n";
	}
	else
	{
		satisfaction = "NOT SATISFIED";
		std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << " at LEVEL " << levelCounter << "." << std::endl << std::endl;
		//foutput << "LEVEL " << levelCounter << " NOT SATISFIED!\n\n";
	}

	//Finished!
	if(satisfaction == "SATISFIED")
	{
		system("pause");
		system("exit");
	}
}

MultilevelClustering::~MultilevelClustering(void)
{
}
