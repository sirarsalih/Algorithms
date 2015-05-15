#include "TabuSearch.h"
#include "TimeElapsed.h"
#include <sys/stat.h>
#include <sstream>

TabuSearch::TabuSearch(void)
{
}

std::vector<int> TabuSearch::initializeNumbers(std::string numbersString, int &numberOfLiterals)
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

void TabuSearch::initializeDataStructure(std::vector<std::string> &vectorStringNumbers, std::vector<int> vectorNumbers, std::multimap<std::string, std::vector<std::string>> &multiMapLiteralClause)
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

int TabuSearch::randomAssignValuesAndEvaluateSolution(std::ofstream &foutput, std::vector<std::string> &vectorUnsatisfiedClauses, std::string numberOfXLiterals, std::vector<std::string> &vectorStringNumbers, int numberOfLiterals, std::string numberOfClauses, std::map<std::string, bool> &mapLiteralValues, std::multimap<std::string, bool> &mapClauseValues, clock_t begin)
{
	const int NUMBER_OF_LITERALS = atoi(numberOfXLiterals.c_str());
	const int NUMBER_OF_CLAUSES = atoi(numberOfClauses.c_str());
	std::string satisfaction = "";
	int bestSoFar = 0;
	int randomIndex = 0;
	bool literalValues[2] = {true, false};
	std::string stringLiteral = "";
	std::vector<std::string> vectorCheckedClauses;

	//Iterator vector, to help find elements
	std::vector<std::string>::iterator iteratorVectorString;

	std::cout << "Assigning the literals TRUE/FALSE values randomly....." << std::endl << std::endl;

	//Assign the literals random TRUE/FALSE values
	for(int i = 1; i <= NUMBER_OF_LITERALS; i++)
	{
		char sizeTemp = (char)i;
		char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
		stringLiteral = itoa(i, bufferTemp, 10);

		randomIndex = rand () % 2;
		mapLiteralValues[stringLiteral] = literalValues[randomIndex];

		//Opposite literal value
		int tempLiteral = i * -1;
		sizeTemp = (char)tempLiteral;
		bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
		stringLiteral = itoa(tempLiteral, bufferTemp, 10);

		if(randomIndex == 0)
			mapLiteralValues[stringLiteral] = literalValues[randomIndex + 1];
		else
			mapLiteralValues[stringLiteral] = literalValues[randomIndex - 1];
	}

	//Assign TRUE/FALSE values to the SAT formula
	std::cout << "Assigning TRUE/FALSE values to the clauses of the SAT formula....." << std::endl << std::endl;

	int trueClauseCounter = 0;
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
					vectorUnsatisfiedClauses.push_back(stringClause);
				}				
			}

			//Reset
			stringClause = "";
			trueCounter = 0;
		}
	}

	bestSoFar = trueClauseCounter;

	//Reset
	trueClauseCounter = 0;
	vectorCheckedClauses.clear();

	foutput << "Literals: " << numberOfXLiterals << "  Clauses: " << NUMBER_OF_CLAUSES << "\n\n";
	std::cout << "Number of TRUE clauses (initial solution): " << std::endl << std::endl << bestSoFar << std::endl << std::endl;
	foutput << "Mean satisfied clauses     Mean time     Mean flips\n\n";
	foutput << bestSoFar << "                         " << 0 << "                         " << 0 << "\n";

	if(bestSoFar == NUMBER_OF_CLAUSES)
	{
		satisfaction = "SATISFIED";
		std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << "." << std::endl << std::endl;
		//foutput << "SATISFIED!\n\n";
	}
	else
	{
		satisfaction = "NOT SATISFIED";
		std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << "." << std::endl << std::endl;
		//foutput << "NOT SATISFIED!\n\n";
	}

	//Finished!
	if(satisfaction == "SATISFIED")
	{
		system("pause");
		system("exit");
	}

	return bestSoFar;
}

void TabuSearch::run(std::ofstream &foutput, std::string noLettersCollectionString, int &numberOfLiterals, std::vector<std::string> vectorStringNumbers, std::map<std::string, bool> &mapLiteralValues, std::multimap<std::string, bool> &mapClauseValues, std::string numberOfXLiterals, std::string numberOfClauses, std::multimap<std::string, std::vector<std::string>> &multiMapLiteralClause, clock_t begin)
{
	const int NUMBER_OF_CLAUSES = atoi(numberOfClauses.c_str());	//Number of clauses
	std::vector<int> vectorLiterals;								//Vector contains literals
	std::vector<int> vectorVisitedLiterals;						//Vector contains visited literals
	std::vector<std::string> vectorClauseUnvisitedLiterals;					//Vector contains unvisited literals from unsatisfied clause
	std::map<int, int> mapTabuList;									//Map contains tabu list
	std::map<int, int> mapBestLiterals;								//Map contains best literals
	std::vector<std::string> vectorUnsatisfiedClauses;
	bool yesNo[2] = {true, false};
	double flipCounter = 0;
	double powerCounter = 1;

	std::pair<std::multimap<std::string, std::vector<std::string>>::iterator, std::multimap<std::string, std::vector<std::string>>::iterator> multiMapIterator;
	std::vector<std::string> vectorClause;
	std::vector<std::string> vectorClauses;
	std::vector<std::string> tempVector;
	std::string tempStringLiteral = "";
	std::string tempStringLiteral2 = "";
	std::string clause = "";
	int trueLiteralCounter = 0;
	int helpVariable = 1;
	int trueClauseCounter = 0;

	/**Initialize literals**/
	std::vector<int> vectorNumbers = initializeNumbers(noLettersCollectionString, numberOfLiterals);

	/**Initialize data structure**/
	initializeDataStructure(vectorStringNumbers, vectorNumbers, multiMapLiteralClause);

	//Assign TRUE/FALSE values to the SAT formula
	std::cout << "Running Tabu Search....." << std::endl << std::endl;

	/**Initialize Tabu list**/
	for(int i = 1; i <= atoi(numberOfXLiterals.c_str()); i++)
		mapTabuList[i] = 0;

	/**Random assign TRUE/FALSE values and evaluate initial solution**/
	int bestSoFar = randomAssignValuesAndEvaluateSolution(foutput, vectorUnsatisfiedClauses, numberOfXLiterals, vectorStringNumbers, numberOfLiterals, numberOfClauses, mapLiteralValues, mapClauseValues, begin);
	int current = bestSoFar;
	int bestGain = 0;
	int gain = 0;
	int randomIndex = 0;
	std::string stringClause = "";
	std::string stringLiteral = "";
	std::string satisfaction = "";

	//Iterator vector, to help find visited literals
	std::vector<int>::iterator iteratorVector;

	//Iterator vector, to help find elements
	std::vector<std::string>::iterator iteratorVectorString;

	//Iterator vector, to help find elements
	std::vector<int>::iterator iteratorVectorIndex;

	//Iterator map, to help find elements
	std::map<std::string, bool>::iterator iteratorMap;

	//Total iteration time
	//double maxIterationTime = 3600;
	double totalIterationTime = 0;

	//Object to calculate time
	TimeElapsed te;

	/**TS VERSION 0 START**
	while(current < NUMBER_OF_CLAUSES && totalIterationTime < maxIterationTime)
	{
		//Start time of iteration
		clock_t beginIterationTime = clock();

		//Set initial gains
		bestGain = -999;
		gain = 0;

		//Find unsatisfied clauses
		for(iteratorMap = mapClauseValues.begin(); iteratorMap != mapClauseValues.end(); iteratorMap++) 
		{
			if(iteratorMap->second == false)
				vectorUnsatisfiedClauses.push_back(iteratorMap->first);
		}

		for(int i = 0; i < NUMBER_OF_CLAUSES - current; i++)
		{
			//Reset
			stringLiteral = "";

			//Pick an unsatisfied clause
			stringClause = vectorUnsatisfiedClauses[i];

			//Pick an unvisited literal inside the clause
			for(int i = 0; i < stringClause.length(); i++)
			{
				if(stringClause[i] == ' ' || i == stringClause.length() - 1)
				{
					if(stringClause[i] != ' ' && i == stringClause.length() - 1)
						stringLiteral += stringClause[i];

					if(atoi(stringLiteral.c_str()) < 0)
					{
						int temp = atoi(stringLiteral.c_str()) * -1;
						char sizeTempOpposite = (char)temp;
						char bufferTempOpposite[sizeof(sizeTempOpposite)/sizeof(char) + 10];
						std::string oppositeLiteral = itoa(temp, bufferTempOpposite, 10);
						stringLiteral = oppositeLiteral;
					}

					vectorClauseUnvisitedLiterals.push_back(stringLiteral);

					//Reset
					stringLiteral = "";
				}
				else
					stringLiteral += stringClause[i];
			}

			//Pick a randomly unvisited literal
			std::random_shuffle(vectorClauseUnvisitedLiterals.begin(), vectorClauseUnvisitedLiterals.end());
			stringLiteral = vectorClauseUnvisitedLiterals[0];
			vectorClauseUnvisitedLiterals.erase(vectorClauseUnvisitedLiterals.begin());

			//Check if visited
			iteratorVector = std::find(vectorVisitedLiterals.begin(), vectorVisitedLiterals.end(), atoi(stringLiteral.c_str()));

			//While visited, pick another
			while(iteratorVector != vectorVisitedLiterals.end())
			{
				std::random_shuffle(vectorClauseUnvisitedLiterals.begin(), vectorClauseUnvisitedLiterals.end());
				stringLiteral = vectorClauseUnvisitedLiterals[0];
				vectorClauseUnvisitedLiterals.erase(vectorClauseUnvisitedLiterals.begin());

				if(vectorClauseUnvisitedLiterals.size() == 0)
				{
					stringLiteral = "";
					break;
				}
			}

			if(stringLiteral != "")
			{
				//Put in visited literals vector
				vectorVisitedLiterals.push_back(atoi(stringLiteral.c_str()));

				//Flip
				flip(mapLiteralValues, stringLiteral);

				std::cout << "Calculating gains....." << std::endl << std::endl;
	
				//Put in tempVector
				tempVector.push_back(stringLiteral);

				//Opposite value
				int oppositeLiteral = atoi(stringLiteral.c_str()) * -1;

				//int -> string
				char sizeTemp = (char)oppositeLiteral;
				char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
				
				std::string oppositeStringLiteral = itoa(oppositeLiteral, bufferTemp, 10);

				//Put in tempVector
				tempVector.push_back(oppositeStringLiteral);

				//Find the clauses that have this literal and update number of true clauses found
				for(int j = 0; j < tempVector.size(); j++)
				{
					tempStringLiteral2 = tempVector[j];

					//Find the clauses that have this literal
					multiMapIterator = multiMapLiteralClause.equal_range(tempStringLiteral2);

					//Check each clause
					for(std::multimap<std::string, std::vector<std::string>>::iterator multiMapIterator2 = multiMapIterator.first; multiMapIterator2 != multiMapIterator.second; ++multiMapIterator2)
					{
					   //Next clause
					   vectorClause = (*multiMapIterator2).second;

					   //Reset
					   clause = "";
		
					   //Check if the clause has been handled already
					   for(int i = 0; i < vectorClause.size(); i++)
					   {
							clause += vectorClause[i];
							clause += " ";
					   }

					   if(clause[clause.length() - 1] == ' ')
						   clause.erase(clause.length() - 1);

					   iteratorVectorString = std::find(vectorClauses.begin(), vectorClauses.end(), clause);

					   //Not found, handle it
					   if(iteratorVectorString == vectorClauses.end())
					   {
						   clause = "";

						   for(int i = 0; i < vectorClause.size(); i++)
						   {
								tempStringLiteral = vectorClause[i];

								//Check this literal
								iteratorVectorString = std::find(tempVector.begin(), tempVector.end(), tempStringLiteral);

								if(mapLiteralValues[tempStringLiteral] == true && iteratorVectorString != tempVector.end())
									helpVariable *= 1;

								else if(mapLiteralValues[tempStringLiteral] == true && iteratorVectorString == tempVector.end())
									helpVariable *= 0;
					
								else if(mapLiteralValues[tempStringLiteral] == false && iteratorVectorString != tempVector.end())
									helpVariable *= 0;
					
								else if(mapLiteralValues[tempStringLiteral] == false && iteratorVectorString == tempVector.end())
									helpVariable *= 1;

								if(mapLiteralValues[tempStringLiteral] == true)
									trueLiteralCounter++;

								clause += tempStringLiteral;
								clause += " ";
						   }

						   if(clause[clause.length() - 1] == ' ')
							   clause.erase(clause.length() - 1);
				
						   vectorClauses.push_back(clause);

							if(trueLiteralCounter == 0)
								trueClauseCounter--;
			
							else if(helpVariable == 1)
								trueClauseCounter++;

							//Reset
							helpVariable = 1;

							//Reset
							trueLiteralCounter = 0;
					   }
					}
				}

				//Set the gain
				gain = trueClauseCounter;

				//Clear
				tempVector.clear();

				//Reset
				vectorClause.clear();

				//Reset
				vectorClauses.clear();

				//Reset
				tempStringLiteral = "";

				//Reset
				tempStringLiteral2 = "";

				//Reset
				clause = "";

				//Reset
				trueClauseCounter = 0;

				//Tabu
				if(mapTabuList[atoi(stringLiteral.c_str())] > 0)
				{
					//Store literal and its gain
					mapBestLiterals[gain] = atoi(stringLiteral.c_str());
				}

				//Not Tabu
				else
				{
					if(gain == bestGain)
					{
						bestGain = gain;

						//Choose random
						int newRandomIndex = rand() % 2;

						if(yesNo[newRandomIndex] == true)
							//Store literal and its gain
							mapBestLiterals[bestGain] = atoi(stringLiteral.c_str());
					}

					else if(gain > bestGain)
					{
						//Update best gain
						bestGain = gain;

						//Clear
						mapBestLiterals.clear();
					
						//Store literal and its gain
						mapBestLiterals[gain] = atoi(stringLiteral.c_str());
					}
				}

				//Flip back
				flip(mapLiteralValues, stringLiteral);

				//Reset
				vectorClauseUnvisitedLiterals.clear();
			}
		}

		//Find best literal
		if(mapBestLiterals.size() != 0)
		{
			std::map<int, int>::iterator iteratorMapMax;
			iteratorMapMax = std::max_element(mapBestLiterals.begin(), mapBestLiterals.end());
			int bestLiteral = iteratorMapMax->second;

			//int -> string
			char sizeTemp = (char)bestLiteral;
			char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
			stringLiteral = itoa(bestLiteral, bufferTemp, 10);

			//If no improvement and best literal is tabu, then don't flip
			if(iteratorMapMax->first <= 0 && mapTabuList[atoi(stringLiteral.c_str())] > 0)
			{
				//Print satisfied clauses
				trueClauseCounter = 0;
				for(iteratorMap = mapClauseValues.begin(); iteratorMap != mapClauseValues.end(); iteratorMap++) 
				{
					if(iteratorMap->second == true)
						trueClauseCounter++;
				}

				current = trueClauseCounter;

				//Reset
				trueClauseCounter = 0;

				std::cout << "Number of TRUE clauses: " << std::endl << std::endl << current << std::endl << std::endl;
				foutput << "Number of TRUE clauses: \n";
				foutput << current << "\n";
				foutput << "Gain: "<< 0 << "\n";

				if(current == NUMBER_OF_CLAUSES)
				{
					satisfaction = "SATISFIED";
					std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << " at LEVEL " << levelCounter << "." << std::endl << std::endl;
					foutput << "SATISFIED!\n";
				}
				else
				{
					satisfaction = "NOT SATISFIED";
					std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << " at LEVEL " << levelCounter << "." << std::endl << std::endl;
					foutput << "NOT SATISFIED!\n";
				}
			}
			else
			{
				//Flip best literal
				flip(mapLiteralValues, stringLiteral);

				std::cout << "UPDATING SAT FORMULA WITH NEW LITERAL VALUES....." << std::endl << std::endl;

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

						if(trueCounter >= 1)
							//Set the clause to TRUE
							mapClauseValues[stringClause] = true;
						
						else
							//Set the clause to FALSE
							mapClauseValues[stringClause] = false;

						//Reset
						stringClause = "";
						trueCounter = 0;
					}
				}

				//Print satisfied clauses
				trueClauseCounter = 0;
				for(iteratorMap = mapClauseValues.begin(); iteratorMap != mapClauseValues.end(); iteratorMap++) 
				{
					if(iteratorMap->second == true)
						trueClauseCounter++;
				}

				current = trueClauseCounter;

				//Reset
				trueClauseCounter = 0;

				std::cout << "Number of TRUE clauses: " << std::endl << std::endl << current << std::endl << std::endl;
				foutput << "Number of TRUE clauses: \n";
				foutput << current << "\n";
				foutput << "Gain: "<< iteratorMapMax->first << "\n";

				if(current == NUMBER_OF_CLAUSES)
				{
					satisfaction = "SATISFIED";
					std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << " at LEVEL " << levelCounter << "." << std::endl << std::endl;
					foutput << "SATISFIED!\n";
				}
				else
				{
					satisfaction = "NOT SATISFIED";
					std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << " at LEVEL " << levelCounter << "." << std::endl << std::endl;
					foutput << "NOT SATISFIED!\n";
				}
			}

			//Finished!
			if(satisfaction == "SATISFIED")
				break;

			else
			{
				if(current >= bestSoFar)
				{
					//Update best so far
					bestSoFar = current;
				}

				//Find the Tabu that has best unsatisfied clauses
				std::vector<int> vectorLookUp;

                for(int i = 1; i <= mapTabuList.size(); i++)
                    vectorLookUp.push_back(mapTabuList[i]);
                        
                iteratorVector = std::unique(vectorLookUp.begin(), vectorLookUp.end());
                vectorLookUp.resize(iteratorVector - vectorLookUp.begin());
                std::sort(vectorLookUp.begin(), vectorLookUp.end());

                int tabuBestUnsatisfied = 0;

                while(vectorLookUp[0] == 0)
                {
                    vectorLookUp.erase(vectorLookUp.begin());
                                
                    //Assign the first Tabu
                    if(vectorLookUp.size() == 0)
                        break;
                }

				if(vectorLookUp.size() == 0)
					mapTabuList[atoi(stringLiteral.c_str())] = NUMBER_OF_CLAUSES - current;

				else
				{
					tabuBestUnsatisfied = vectorLookUp[0];

					if(mapTabuList[atoi(stringLiteral.c_str())] == 0 && NUMBER_OF_CLAUSES - current < tabuBestUnsatisfied)
						mapTabuList[atoi(stringLiteral.c_str())] = NUMBER_OF_CLAUSES - current;

					else if(mapTabuList[atoi(stringLiteral.c_str())] == 0 && NUMBER_OF_CLAUSES - current > tabuBestUnsatisfied)
						mapTabuList[atoi(stringLiteral.c_str())] = tabuBestUnsatisfied;

					else if(mapTabuList[atoi(stringLiteral.c_str())] == 0 && NUMBER_OF_CLAUSES - current == tabuBestUnsatisfied)
						mapTabuList[atoi(stringLiteral.c_str())] = tabuBestUnsatisfied;
				}

				//Update tabu list
				for(int i = 1; i <= mapTabuList.size(); i++)
				{
					if(i != atoi(stringLiteral.c_str()) && mapTabuList[i] != 0)
					{
						mapTabuList[i] -= 1;
					}
				}

				//Reset
				vectorLookUp.clear();
			}
		}

		//Reset
		vectorUnsatisfiedClauses.clear();

		//Reset
		vectorVisitedLiterals.clear();

		//Clear
		vectorClauseUnvisitedLiterals.clear();

		//Reset
		mapBestLiterals.clear();

		//Reset
		stringLiteral = "";

		//End time of iteration
		clock_t endIterationTime = clock();

		//Update total iteration time
		totalIterationTime += te.GetTimeElapsed(endIterationTime, beginIterationTime) / 1000;

		//Show time elapsed
		std::cout << "Time elapsed: " << totalIterationTime << " seconds" << std::endl << std::endl;
		foutput << "Time elapsed: " << totalIterationTime << " seconds\n\n";
	}
	**TS VERSION 0 END**/

	/**TS VERSION 1 START**/
	while(current < NUMBER_OF_CLAUSES)
	{
		//Start time of iteration
		clock_t beginIterationTime = clock();

		//Set initial gains
		bestGain = -999;
		gain = 0;

		for(int i = 0; i < NUMBER_OF_CLAUSES - current; i++)
		{
			//Reset
			stringLiteral = "";

			//Pick an unsatisfied clause
			stringClause = vectorUnsatisfiedClauses[i];

			//Pick an unvisited literal inside the clause
			for(int i = 0; i < stringClause.length(); i++)
			{
				if(stringClause[i] == ' ' || i == stringClause.length() - 1)
				{
					if(stringClause[i] != ' ' && i == stringClause.length() - 1)
						stringLiteral += stringClause[i];

					if(atoi(stringLiteral.c_str()) < 0)
					{
						int temp = atoi(stringLiteral.c_str()) * -1;
						char sizeTempOpposite = (char)temp;
						char bufferTempOpposite[sizeof(sizeTempOpposite)/sizeof(char) + 10];
						std::string oppositeLiteral = itoa(temp, bufferTempOpposite, 10);
						stringLiteral = oppositeLiteral;
					}

					vectorClauseUnvisitedLiterals.push_back(stringLiteral);

					//Reset
					stringLiteral = "";
				}
				else
					stringLiteral += stringClause[i];
			}

			//Pick a randomly unvisited literal			
			std::random_shuffle(vectorClauseUnvisitedLiterals.begin(), vectorClauseUnvisitedLiterals.end());
			stringLiteral = vectorClauseUnvisitedLiterals[0];
			vectorClauseUnvisitedLiterals.erase(vectorClauseUnvisitedLiterals.begin());

			//Check if visited
			iteratorVector = std::find(vectorVisitedLiterals.begin(), vectorVisitedLiterals.end(), atoi(stringLiteral.c_str()));

			//While visited, pick another
			while(iteratorVector != vectorVisitedLiterals.end())
			{
				if(vectorClauseUnvisitedLiterals.size() == 0)
				{
					stringLiteral = "";
					break;
				}

				std::random_shuffle(vectorClauseUnvisitedLiterals.begin(), vectorClauseUnvisitedLiterals.end());
				stringLiteral = vectorClauseUnvisitedLiterals[0];
				vectorClauseUnvisitedLiterals.erase(vectorClauseUnvisitedLiterals.begin());
				iteratorVector = std::find(vectorVisitedLiterals.begin(), vectorVisitedLiterals.end(), atoi(stringLiteral.c_str()));

				if(vectorClauseUnvisitedLiterals.size() == 0)
				{
					stringLiteral = "";
					break;
				}
			}

			if(stringLiteral != "")
			{
				//Put in visited literals vector
				vectorVisitedLiterals.push_back(atoi(stringLiteral.c_str()));

				//Flip
				flip(mapLiteralValues, stringLiteral);

				std::cout << "Calculating gains....." << std::endl << std::endl;
	
				//Put in tempVector
				tempVector.push_back(stringLiteral);

				//Opposite value
				int oppositeLiteral = atoi(stringLiteral.c_str()) * -1;

				//int -> string
				char sizeTemp = (char)oppositeLiteral;
				char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
				
				std::string oppositeStringLiteral = itoa(oppositeLiteral, bufferTemp, 10);

				//Put in tempVector
				tempVector.push_back(oppositeStringLiteral);

				//Find the clauses that have this literal and update number of true clauses found
				for(int j = 0; j < tempVector.size(); j++)
				{
					tempStringLiteral2 = tempVector[j];

					//Find the clauses that have this literal
					multiMapIterator = multiMapLiteralClause.equal_range(tempStringLiteral2);

					//Check each clause
					for(std::multimap<std::string, std::vector<std::string>>::iterator multiMapIterator2 = multiMapIterator.first; multiMapIterator2 != multiMapIterator.second; ++multiMapIterator2)
					{
					   //Next clause
					   vectorClause = (*multiMapIterator2).second;

					   //Reset
					   clause = "";
		
					   //Check if the clause has been handled already
					   for(int i = 0; i < vectorClause.size(); i++)
					   {
							clause += vectorClause[i];
							clause += " ";
					   }

					   if(clause[clause.length() - 1] == ' ')
						   clause.erase(clause.length() - 1);

					   iteratorVectorString = std::find(vectorClauses.begin(), vectorClauses.end(), clause);

					   //Not found, handle it
					   if(iteratorVectorString == vectorClauses.end())
					   {
						   clause = "";

						   for(int i = 0; i < vectorClause.size(); i++)
						   {
								tempStringLiteral = vectorClause[i];

								//Check this literal
								iteratorVectorString = std::find(tempVector.begin(), tempVector.end(), tempStringLiteral);

								if(mapLiteralValues[tempStringLiteral] == true && iteratorVectorString != tempVector.end())
									helpVariable *= 1;

								else if(mapLiteralValues[tempStringLiteral] == true && iteratorVectorString == tempVector.end())
									helpVariable *= 0;
					
								else if(mapLiteralValues[tempStringLiteral] == false && iteratorVectorString != tempVector.end())
									helpVariable *= 0;
					
								else if(mapLiteralValues[tempStringLiteral] == false && iteratorVectorString == tempVector.end())
									helpVariable *= 1;

								if(mapLiteralValues[tempStringLiteral] == true)
									trueLiteralCounter++;

								clause += tempStringLiteral;
								clause += " ";
						   }

						   if(clause[clause.length() - 1] == ' ')
							   clause.erase(clause.length() - 1);
				
						   vectorClauses.push_back(clause);

							if(trueLiteralCounter == 0)
								trueClauseCounter--;
			
							else if(helpVariable == 1)
								trueClauseCounter++;

							//Reset
							helpVariable = 1;

							//Reset
							trueLiteralCounter = 0;
					   }
					}
				}

				//Set the gain
				gain = trueClauseCounter;

				//Clear
				tempVector.clear();

				//Reset
				vectorClause.clear();

				//Reset
				vectorClauses.clear();

				//Reset
				tempStringLiteral = "";

				//Reset
				tempStringLiteral2 = "";

				//Reset
				clause = "";

				//Reset
				trueClauseCounter = 0;

				//Not Tabu
				if(mapTabuList[atoi(stringLiteral.c_str())] == 0)
				{
					if(gain == bestGain)
					{
						bestGain = gain;

						//Choose random
						int newRandomIndex = rand() % 2;

						if(yesNo[newRandomIndex] == true)
							//Store literal and its gain
							mapBestLiterals[bestGain] = atoi(stringLiteral.c_str());
					}

					else if(gain > bestGain)
					{
						//Update best gain
						bestGain = gain;

						//Clear
						mapBestLiterals.clear();
					
						//Store literal and its gain
						mapBestLiterals[bestGain] = atoi(stringLiteral.c_str());
					}
				}

				//Flip back
				flip(mapLiteralValues, stringLiteral);

				//Increase the flip counter
				flipCounter++;

				//Reached 10^6 flips and 1 hour duration
				if(flipCounter / pow(10, powerCounter) >= 1 && powerCounter == 6 && totalIterationTime >= 3600)
				{
					foutput << bestSoFar << "                         " << totalIterationTime << "                         " << flipCounter << "\n";
					break;
				}

				else if(flipCounter / pow(10, powerCounter) == 1 && powerCounter < 6)
				{
					foutput << bestSoFar << "                         " << totalIterationTime << "                         " << flipCounter << "\n";
					powerCounter++;
				}

				//Reset
				vectorClauseUnvisitedLiterals.clear();
			}
		}

		//Find best literal
		if(mapBestLiterals.size() != 0)
		{
			std::map<int, int>::iterator iteratorMapMax;
			iteratorMapMax = std::max_element(mapBestLiterals.begin(), mapBestLiterals.end());
			int bestLiteral = iteratorMapMax->second;

			//int -> string
			char sizeTemp = (char)bestLiteral;
			char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
			stringLiteral = itoa(bestLiteral, bufferTemp, 10);

			//If no improvement and best literal is tabu, then don't flip
			if(iteratorMapMax->first + current < bestSoFar && mapTabuList[atoi(stringLiteral.c_str())] > 0)
			{
				//Print satisfied clauses
				std::cout << "Number of TRUE clauses: " << std::endl << std::endl << bestSoFar << std::endl << std::endl;
				foutput << bestSoFar << "                         " << totalIterationTime << "                         " << flipCounter << "\n";

				if(bestSoFar == NUMBER_OF_CLAUSES)
				{
					satisfaction = "SATISFIED";
					std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << "." << std::endl << std::endl;
					//foutput << "SATISFIED!\n";
				}
				else
				{
					satisfaction = "NOT SATISFIED";
					std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << "." << std::endl << std::endl;					
					//foutput << "NOT SATISFIED!\n";
				}
			}
			else
			{
				//Flip best literal
				flip(mapLiteralValues, stringLiteral);

				std::cout << "UPDATING SAT FORMULA WITH NEW LITERAL VALUES....." << std::endl << std::endl;

				//Put in tempVector
				tempVector.push_back(stringLiteral);

				//Opposite value
				int oppositeLiteral = atoi(stringLiteral.c_str()) * -1;

				//int -> string
				char sizeTemp = (char)oppositeLiteral;
				char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];				
				std::string oppositeStringLiteral = itoa(oppositeLiteral, bufferTemp, 10);

				//Put in tempVector
				tempVector.push_back(oppositeStringLiteral);

				for(int j = 0; j < tempVector.size(); j++)
				{
					tempStringLiteral2 = tempVector[j];

					//Find the clauses that have this literal
					multiMapIterator = multiMapLiteralClause.equal_range(tempStringLiteral2);

					//Check each clause
					for(std::multimap<std::string, std::vector<std::string>>::iterator multiMapIterator2 = multiMapIterator.first; multiMapIterator2 != multiMapIterator.second; ++multiMapIterator2)
					{
						//Next clause
						vectorClause = (*multiMapIterator2).second;
	
						//Reset
						clause = "";
		
						//Check if the clause has been handled already
						for(int i = 0; i < vectorClause.size(); i++)
						{
							clause += vectorClause[i];
							clause += " ";
						}

						if(clause[clause.length() - 1] == ' ')
							clause.erase(clause.length() - 1);

						iteratorVectorString = std::find(vectorClauses.begin(), vectorClauses.end(), clause);

						//Not found, handle it
						if(iteratorVectorString == vectorClauses.end())
						{
							clause = "";

							for(int i = 0; i < vectorClause.size(); i++)
							{
								tempStringLiteral = vectorClause[i];

								//Check this literal
								iteratorVectorString = std::find(tempVector.begin(), tempVector.end(), tempStringLiteral);

								if(mapLiteralValues[tempStringLiteral] == true && iteratorVectorString != tempVector.end())
									helpVariable *= 1;

								else if(mapLiteralValues[tempStringLiteral] == true && iteratorVectorString == tempVector.end())
									helpVariable *= 0;
					
								else if(mapLiteralValues[tempStringLiteral] == false && iteratorVectorString != tempVector.end())
									helpVariable *= 0;
					
								else if(mapLiteralValues[tempStringLiteral] == false && iteratorVectorString == tempVector.end())
									helpVariable *= 1;

								if(mapLiteralValues[tempStringLiteral] == true)
									trueLiteralCounter++;

								clause += tempStringLiteral;
								clause += " ";
							}

							if(clause[clause.length() - 1] == ' ')
								clause.erase(clause.length() - 1);
				
							vectorClauses.push_back(clause);

							//Unsatisfied, push it in unsatisfied vector
							if(trueLiteralCounter == 0)
							{
								trueClauseCounter--;
								vectorUnsatisfiedClauses.push_back(clause);
							}
			
							//Satisfied, remove it from unsatisfied vector
							else if(helpVariable == 1)
							{
								trueClauseCounter++;
								iteratorVectorString = std::find(vectorUnsatisfiedClauses.begin(), vectorUnsatisfiedClauses.end(), clause);
								int index = iteratorVectorString - vectorUnsatisfiedClauses.begin();

								if(iteratorVectorString != vectorUnsatisfiedClauses.end())
									vectorUnsatisfiedClauses.erase(vectorUnsatisfiedClauses.begin() + index);
							}

							//Reset
							helpVariable = 1;

							//Reset
							trueLiteralCounter = 0;

							//Reset
							clause = "";
						}
					}
				}

				//Update current
				current += trueClauseCounter;

				//Reset
				trueClauseCounter = 0;

				//Clear
				tempVector.clear();

				//Reset
				vectorClause.clear();

				//Reset
				vectorClauses.clear();

				//Reset
				tempStringLiteral = "";

				//Reset
				tempStringLiteral2 = "";

				if(current >= bestSoFar)
				{
					//Update best so far
					bestSoFar = current;
				}

				std::cout << "Number of TRUE clauses: " << std::endl << std::endl << bestSoFar << std::endl << std::endl;
				foutput << bestSoFar << "                         " << totalIterationTime << "                         " << flipCounter << "\n";

				if(bestSoFar == NUMBER_OF_CLAUSES)
				{
					satisfaction = "SATISFIED";
					std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << "." << std::endl << std::endl;
					//foutput << "SATISFIED!\n";
				}
				else
				{
					satisfaction = "NOT SATISFIED";
					std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << "." << std::endl << std::endl;
					//foutput << "NOT SATISFIED!\n";
				}

				//Find the Tabu that has best unsatisfied clauses
				std::vector<int> vectorLookUp;

                for(int i = 1; i <= mapTabuList.size(); i++)
                    vectorLookUp.push_back(mapTabuList[i]);
                        
                iteratorVector = std::unique(vectorLookUp.begin(), vectorLookUp.end());
                vectorLookUp.resize(iteratorVector - vectorLookUp.begin());
                std::sort(vectorLookUp.begin(), vectorLookUp.end());

                int tabuBestUnsatisfied = 0;

                while(vectorLookUp[0] == 0)
                {
                    vectorLookUp.erase(vectorLookUp.begin());
                                
                    //Assign the first Tabu
                    if(vectorLookUp.size() == 0)
                        break;
                }

				if(vectorLookUp.size() == 0)
					mapTabuList[atoi(stringLiteral.c_str())] = NUMBER_OF_CLAUSES - current;

				else
				{
					tabuBestUnsatisfied = vectorLookUp[0];

					if(mapTabuList[atoi(stringLiteral.c_str())] == 0 && NUMBER_OF_CLAUSES - current < tabuBestUnsatisfied)
						mapTabuList[atoi(stringLiteral.c_str())] = NUMBER_OF_CLAUSES - current;

					else if(mapTabuList[atoi(stringLiteral.c_str())] == 0 && NUMBER_OF_CLAUSES - current > tabuBestUnsatisfied)
						mapTabuList[atoi(stringLiteral.c_str())] = tabuBestUnsatisfied;

					else if(mapTabuList[atoi(stringLiteral.c_str())] == 0 && NUMBER_OF_CLAUSES - current == tabuBestUnsatisfied)
						mapTabuList[atoi(stringLiteral.c_str())] = tabuBestUnsatisfied;
				}

				//Reset
				vectorLookUp.clear();
			}

			//Update tabu list
			for(int i = 1; i <= mapTabuList.size(); i++)
			{
				if(i != atoi(stringLiteral.c_str()) && mapTabuList[i] != 0)
				{
					mapTabuList[i] -= 1;
				}
			}
		}

		//Reset
		vectorVisitedLiterals.clear();

		//Clear
		vectorClauseUnvisitedLiterals.clear();

		//Reset
		mapBestLiterals.clear();

		//Reset
		stringLiteral = "";

		//End time of iteration
		clock_t endIterationTime = clock();

		//Update total iteration time
		totalIterationTime += te.GetTimeElapsed(endIterationTime, beginIterationTime) / 1000;

		//Finished!
		if(satisfaction == "SATISFIED" || flipCounter / pow(10, powerCounter) >= 1 && powerCounter == 6 && totalIterationTime >= 3600)
			break;

		//Show time elapsed
		std::cout << "Time elapsed: " << totalIterationTime << " seconds" << std::endl << std::endl;
		//foutput << "Time elapsed: " << totalIterationTime << " seconds\n\n";
	}
	/**TS VERSION 1 END**/

	/**TS VERSION 2 START**
	std::vector<int> vectorSequence;
	std::vector<std::vector<int>> vectorFinalSequence;
	std::vector<int> vectorTabu;

	while(current < NUMBER_OF_CLAUSES && totalIterationTime < maxIterationTime)
	{
		//Start time of iteration
		clock_t beginIterationTime = clock();

		//Set initial gains
		gain = 0;

		//Find unsatisfied clauses
		for(iteratorMap = mapClauseValues.begin(); iteratorMap != mapClauseValues.end(); iteratorMap++) 
		{
			if(iteratorMap->second == false)
				vectorUnsatisfiedClauses.push_back(iteratorMap->first);
		}

		for(int i = 0; i < NUMBER_OF_CLAUSES - current; i++)
		{
			//Reset
			stringLiteral = "";

			//Pick an unsatisfied clause
			stringClause = vectorUnsatisfiedClauses[i];

			//Pick an unvisited literal inside the clause
			for(int i = 0; i < stringClause.length(); i++)
			{
				if(stringClause[i] == ' ' || i == stringClause.length() - 1)
				{
					if(stringClause[i] != ' ' && i == stringClause.length() - 1)
						stringLiteral += stringClause[i];

					if(atoi(stringLiteral.c_str()) < 0)
					{
						int temp = atoi(stringLiteral.c_str()) * -1;
						char sizeTempOpposite = (char)temp;
						char bufferTempOpposite[sizeof(sizeTempOpposite)/sizeof(char) + 10];
						std::string oppositeLiteral = itoa(temp, bufferTempOpposite, 10);
						stringLiteral = oppositeLiteral;
					}

					vectorClauseUnvisitedLiterals.push_back(stringLiteral);

					//Reset
					stringLiteral = "";
				}
				else
					stringLiteral += stringClause[i];
			}

			//Pick a randomly unvisited literal
			std::random_shuffle(vectorClauseUnvisitedLiterals.begin(), vectorClauseUnvisitedLiterals.end());
			stringLiteral = vectorClauseUnvisitedLiterals[0];
			vectorClauseUnvisitedLiterals.erase(vectorClauseUnvisitedLiterals.begin());

			//Check if visited
			iteratorVector = std::find(vectorVisitedLiterals.begin(), vectorVisitedLiterals.end(), atoi(stringLiteral.c_str()));

			//While visited, pick another
			while(iteratorVector != vectorVisitedLiterals.end())
			{
				std::random_shuffle(vectorClauseUnvisitedLiterals.begin(), vectorClauseUnvisitedLiterals.end());
				stringLiteral = vectorClauseUnvisitedLiterals[0];
				vectorClauseUnvisitedLiterals.erase(vectorClauseUnvisitedLiterals.begin());

				if(vectorClauseUnvisitedLiterals.size() == 0)
				{
					stringLiteral = "";
					break;
				}
			}

			if(stringLiteral != "")
			{
				//Put in visited literals vector
				vectorVisitedLiterals.push_back(atoi(stringLiteral.c_str()));

				//Flip
				flip(mapLiteralValues, stringLiteral);

				std::cout << "Calculating gains....." << std::endl << std::endl;
	
				//Put in tempVector
				tempVector.push_back(stringLiteral);

				//Opposite value
				int oppositeLiteral = atoi(stringLiteral.c_str()) * -1;

				//int -> string
				char sizeTemp = (char)oppositeLiteral;
				char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
				
				std::string oppositeStringLiteral = itoa(oppositeLiteral, bufferTemp, 10);

				//Put in tempVector
				tempVector.push_back(oppositeStringLiteral);

				//Find the clauses that have this literal and update number of true clauses found
				for(int j = 0; j < tempVector.size(); j++)
				{
					tempStringLiteral2 = tempVector[j];

					//Find the clauses that have this literal
					multiMapIterator = multiMapLiteralClause.equal_range(tempStringLiteral2);

					//Check each clause
					for(std::multimap<std::string, std::vector<std::string>>::iterator multiMapIterator2 = multiMapIterator.first; multiMapIterator2 != multiMapIterator.second; ++multiMapIterator2)
					{
					   //Next clause
					   vectorClause = (*multiMapIterator2).second;

					   //Reset
					   clause = "";
		
					   //Check if the clause has been handled already
					   for(int i = 0; i < vectorClause.size(); i++)
					   {
							clause += vectorClause[i];
							clause += " ";
					   }

					   if(clause[clause.length() - 1] == ' ')
						   clause.erase(clause.length() - 1);

					   iteratorVectorString = std::find(vectorClauses.begin(), vectorClauses.end(), clause);

					   //Not found, handle it
					   if(iteratorVectorString == vectorClauses.end())
					   {
						   clause = "";

						   for(int i = 0; i < vectorClause.size(); i++)
						   {
								tempStringLiteral = vectorClause[i];

								//Check this literal
								iteratorVectorString = std::find(tempVector.begin(), tempVector.end(), tempStringLiteral);

								if(mapLiteralValues[tempStringLiteral] == true && iteratorVectorString != tempVector.end())
									helpVariable *= 1;

								else if(mapLiteralValues[tempStringLiteral] == true && iteratorVectorString == tempVector.end())
									helpVariable *= 0;
					
								else if(mapLiteralValues[tempStringLiteral] == false && iteratorVectorString != tempVector.end())
									helpVariable *= 0;
					
								else if(mapLiteralValues[tempStringLiteral] == false && iteratorVectorString == tempVector.end())
									helpVariable *= 1;

								if(mapLiteralValues[tempStringLiteral] == true)
									trueLiteralCounter++;

								clause += tempStringLiteral;
								clause += " ";
						   }

						   if(clause[clause.length() - 1] == ' ')
							   clause.erase(clause.length() - 1);
				
						   vectorClauses.push_back(clause);

							if(trueLiteralCounter == 0)
								trueClauseCounter--;
			
							else if(helpVariable == 1)
								trueClauseCounter++;

							//Reset
							helpVariable = 1;

							//Reset
							trueLiteralCounter = 0;
					   }
					}
				}

				//Set the gain
				gain = trueClauseCounter;

				//Clear
				tempVector.clear();

				//Reset
				vectorClause.clear();

				//Reset
				vectorClauses.clear();

				//Reset
				tempStringLiteral = "";

				//Reset
				tempStringLiteral2 = "";

				//Reset
				clause = "";

				//Reset
				trueClauseCounter = 0;

				//Tabu
				if(mapTabuList[atoi(stringLiteral.c_str())] > 0)
				{
					//If no improvement, flip it back
					if(gain <= 0)
						flip(mapLiteralValues, stringLiteral);
					
					else
					{
						//Store literal and its gain
						mapBestLiterals[atoi(stringLiteral.c_str())] = gain;

						vectorSequence.push_back(gain);
						vectorSequence.push_back(atoi(stringLiteral.c_str()));
						vectorFinalSequence.push_back(vectorSequence);

						//Clear
						vectorSequence.clear();
					}
				}

				//Not Tabu
				else
				{					
					//Store literal and its gain
					mapBestLiterals[atoi(stringLiteral.c_str())] = gain;

					vectorSequence.push_back(gain);
					vectorSequence.push_back(atoi(stringLiteral.c_str()));
					vectorFinalSequence.push_back(vectorSequence);

					//Clear
					vectorSequence.clear();
				}

				//Reset
				vectorClauseUnvisitedLiterals.clear();
			}
		}

		if(mapBestLiterals.size() != 0)
		{
			//The final sequence is ready for evaluation
			std::vector<int> tempVector = vectorFinalSequence[0];
			int globalGain = 0;

			globalGain = tempVector[0];
			int tempGain = 0;
			int i = 0;
			
			//Find global gain from sequence
			for(i; i < vectorFinalSequence.size(); i++)
			{
				if(i == vectorFinalSequence.size() - 1)
					break;

				tempVector = vectorFinalSequence[i + 1];

				tempGain += globalGain + tempVector[0];

				if(tempGain >= globalGain)
				{
					globalGain = tempGain;
					tempGain = 0;
				}
				else
					break;
			}

			//The end was not reached, so it means there are bad gains that must be flipped back
			if(i != vectorFinalSequence.size() - 1)
			{
				for(i = i + 1; i < vectorFinalSequence.size(); i++)
				{
					tempVector = vectorFinalSequence[i];

					//int -> string
					char sizeTemp = (char)tempVector[tempVector.size() - 1];
					char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
             		stringLiteral = itoa(tempVector[tempVector.size() - 1], bufferTemp, 10);

					//Flip back
					flip(mapLiteralValues, stringLiteral);
				}
			}

			std::cout << "UPDATING SAT FORMULA WITH NEW LITERAL VALUES....." << std::endl << std::endl;

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

					if(trueCounter >= 1)
						//Set the clause to TRUE
						mapClauseValues[stringClause] = true;
						
					else
						//Set the clause to FALSE
						mapClauseValues[stringClause] = false;

					//Reset
					stringClause = "";
					trueCounter = 0;
				}
			}

			//Print satisfied clauses
			trueClauseCounter = 0;
			for(iteratorMap = mapClauseValues.begin(); iteratorMap != mapClauseValues.end(); iteratorMap++) 
			{
				if(iteratorMap->second == true)
					trueClauseCounter++;
			}

			current = trueClauseCounter;

			//Reset
			trueClauseCounter = 0;

			std::cout << "Number of TRUE clauses: " << std::endl << std::endl << current << std::endl << std::endl;
			foutput << "Number of TRUE clauses: \n";
			foutput << current << "\n";
			foutput << "Gain: "<< globalGain << "\n";

			//Reset
			globalGain = 0;
			tempGain = 0;

			if(current == NUMBER_OF_CLAUSES)
			{
				satisfaction = "SATISFIED";
				std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << " at LEVEL " << levelCounter << "." << std::endl << std::endl;
				foutput << "SATISFIED!\n";
			}
			else
			{
				satisfaction = "NOT SATISFIED";
				std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << " at LEVEL " << levelCounter << "." << std::endl << std::endl;
				foutput << "NOT SATISFIED!\n";
			}

			//Finished!
			if(satisfaction == "SATISFIED")
				break;

			else
			{
				if(current >= bestSoFar)
				{
					//Update best so far
					bestSoFar = current;
				}

				//Find the Tabu that has best unsatisfied clauses
				std::vector<int> vectorLookUp;

                for(int i = 1; i <= mapTabuList.size(); i++)
                    vectorLookUp.push_back(mapTabuList[i]);
                        
                iteratorVector = std::unique(vectorLookUp.begin(), vectorLookUp.end());
                vectorLookUp.resize(iteratorVector - vectorLookUp.begin());
                std::sort(vectorLookUp.begin(), vectorLookUp.end());

                int tabuBestUnsatisfied = 0;

                while(vectorLookUp[0] == 0)
                {
                    vectorLookUp.erase(vectorLookUp.begin());
                                
                    //Assign the first Tabu
                    if(vectorLookUp.size() == 0)
                        break;
                }

				//Make the flipped literals all tabu
				tempVector = vectorFinalSequence[0];
				i = 0;

				for(i; i < vectorFinalSequence.size(); i++)
				{
					if(i == vectorFinalSequence.size() - 1)
						break;

					tempVector = vectorFinalSequence[i + 1];

					tempGain += globalGain + tempVector[0];

					if(tempGain >= globalGain)
					{
						globalGain = tempGain;
						tempGain = 0;
					}
					else
						break;
				}

				for(int j = 0; j <= i; j++)
				{
					tempVector = vectorFinalSequence[j];

					if(vectorLookUp.size() == 0)
                    {
                        mapTabuList[tempVector[tempVector.size() - 1]] = NUMBER_OF_CLAUSES - current;
                        vectorTabu.push_back(tempVector[tempVector.size() - 1]);
                    }

					else
					{
						tabuBestUnsatisfied = vectorLookUp[0];

						if(mapTabuList[tempVector[tempVector.size() - 1]] == 0 && NUMBER_OF_CLAUSES - current < tabuBestUnsatisfied)
						{
							mapTabuList[tempVector[tempVector.size() - 1]] = NUMBER_OF_CLAUSES - current;
							vectorTabu.push_back(tempVector[tempVector.size() - 1]);
						}

						else if(mapTabuList[tempVector[tempVector.size() - 1]] == 0 && NUMBER_OF_CLAUSES - current > tabuBestUnsatisfied)
						{
							mapTabuList[tempVector[tempVector.size() - 1]] = tabuBestUnsatisfied;
							vectorTabu.push_back(tempVector[tempVector.size() - 1]);
						}

						else if(mapTabuList[tempVector[tempVector.size() - 1]] == 0 && NUMBER_OF_CLAUSES - current == tabuBestUnsatisfied)
						{
							mapTabuList[tempVector[tempVector.size() - 1]] = tabuBestUnsatisfied;
							vectorTabu.push_back(tempVector[tempVector.size() - 1]);
						}
					}
				}

				//Update tabu list
                for(int k = 1; k <= mapTabuList.size(); k++)
                {
                    iteratorVector = std::find(vectorTabu.begin(), vectorTabu.end(), k);

                    if(iteratorVector == vectorTabu.end() && mapTabuList[k] != 0)
                            mapTabuList[k] -= 1;
                }

				//Reset
				vectorLookUp.clear();
			}
		}

		//Reset
		vectorUnsatisfiedClauses.clear();

		//Reset
		vectorVisitedLiterals.clear();

		//Clear
		vectorClauseUnvisitedLiterals.clear();

		//Reset
		mapBestLiterals.clear();

		//Reset
		vectorSequence.clear();
				
		//Reset
		vectorFinalSequence.clear();
				
		//Reset
		vectorTabu.clear();

		//Reset
		stringLiteral = "";

		//End time of iteration
		clock_t endIterationTime = clock();

		//Update total iteration time
		totalIterationTime += te.GetTimeElapsed(endIterationTime, beginIterationTime) / 1000;

		//Show time elapsed
		std::cout << "Time elapsed: " << totalIterationTime << " seconds" << std::endl << std::endl;		
		foutput << "Time elapsed: " << totalIterationTime << " seconds\n\n";
	}
	**TS VERSION 2 END**/

	/**TS VERSION 3 START**
	while(current < NUMBER_OF_CLAUSES && totalIterationTime < maxIterationTime)
	{
		//Start time of iteration
		clock_t beginIterationTime = clock();

		//Set initial gains
		bestGain = -999;
		gain = 0;

		//Find unsatisfied clauses
		for(iteratorMap = mapClauseValues.begin(); iteratorMap != mapClauseValues.end(); iteratorMap++) 
		{
			if(iteratorMap->second == false)
				vectorUnsatisfiedClauses.push_back(iteratorMap->first);
		}

		for(int i = 0; i < NUMBER_OF_CLAUSES - current; i++)
		{
			//Reset
			stringLiteral = "";

			//Pick an unsatisfied clause
			stringClause = vectorUnsatisfiedClauses[i];

			//Pick an unvisited literal inside the clause
			for(int i = 0; i < stringClause.length(); i++)
			{
				if(stringClause[i] == ' ' || i == stringClause.length() - 1)
				{
					if(stringClause[i] != ' ' && i == stringClause.length() - 1)
						stringLiteral += stringClause[i];

					if(atoi(stringLiteral.c_str()) < 0)
					{
						int temp = atoi(stringLiteral.c_str()) * -1;
						char sizeTempOpposite = (char)temp;
						char bufferTempOpposite[sizeof(sizeTempOpposite)/sizeof(char) + 10];
						std::string oppositeLiteral = itoa(temp, bufferTempOpposite, 10);
						stringLiteral = oppositeLiteral;
					}

					vectorClauseUnvisitedLiterals.push_back(stringLiteral);

					//Reset
					stringLiteral = "";
				}
				else
					stringLiteral += stringClause[i];
			}

			//Pick a randomly unvisited literal
			std::random_shuffle(vectorClauseUnvisitedLiterals.begin(), vectorClauseUnvisitedLiterals.end());
			stringLiteral = vectorClauseUnvisitedLiterals[0];
			vectorClauseUnvisitedLiterals.erase(vectorClauseUnvisitedLiterals.begin());

			//Check if visited
			iteratorVector = std::find(vectorVisitedLiterals.begin(), vectorVisitedLiterals.end(), atoi(stringLiteral.c_str()));

			//While visited, pick another
			while(iteratorVector != vectorVisitedLiterals.end())
			{
				std::random_shuffle(vectorClauseUnvisitedLiterals.begin(), vectorClauseUnvisitedLiterals.end());
				stringLiteral = vectorClauseUnvisitedLiterals[0];
				vectorClauseUnvisitedLiterals.erase(vectorClauseUnvisitedLiterals.begin());

				if(vectorClauseUnvisitedLiterals.size() == 0)
				{
					stringLiteral = "";
					break;
				}
			}

			if(stringLiteral != "")
			{
				//Put in visited literals vector
				vectorVisitedLiterals.push_back(atoi(stringLiteral.c_str()));

				//Flip
				flip(mapLiteralValues, stringLiteral);

				std::cout << "Calculating gains....." << std::endl << std::endl;
	
				//Put in tempVector
				tempVector.push_back(stringLiteral);

				//Opposite value
				int oppositeLiteral = atoi(stringLiteral.c_str()) * -1;

				//int -> string
				char sizeTemp = (char)oppositeLiteral;
				char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
				
				std::string oppositeStringLiteral = itoa(oppositeLiteral, bufferTemp, 10);

				//Put in tempVector
				tempVector.push_back(oppositeStringLiteral);

				//Find the clauses that have this literal and update number of true clauses found
				for(int j = 0; j < tempVector.size(); j++)
				{
					tempStringLiteral2 = tempVector[j];

					//Find the clauses that have this literal
					multiMapIterator = multiMapLiteralClause.equal_range(tempStringLiteral2);

					//Check each clause
					for(std::multimap<std::string, std::vector<std::string>>::iterator multiMapIterator2 = multiMapIterator.first; multiMapIterator2 != multiMapIterator.second; ++multiMapIterator2)
					{
					   //Next clause
					   vectorClause = (*multiMapIterator2).second;

					   //Reset
					   clause = "";
		
					   //Check if the clause has been handled already
					   for(int i = 0; i < vectorClause.size(); i++)
					   {
							clause += vectorClause[i];
							clause += " ";
					   }

					   if(clause[clause.length() - 1] == ' ')
						   clause.erase(clause.length() - 1);

					   iteratorVectorString = std::find(vectorClauses.begin(), vectorClauses.end(), clause);

					   //Not found, handle it
					   if(iteratorVectorString == vectorClauses.end())
					   {
						   clause = "";

						   for(int i = 0; i < vectorClause.size(); i++)
						   {
								tempStringLiteral = vectorClause[i];

								//Check this literal
								iteratorVectorString = std::find(tempVector.begin(), tempVector.end(), tempStringLiteral);

								if(mapLiteralValues[tempStringLiteral] == true && iteratorVectorString != tempVector.end())
									helpVariable *= 1;

								else if(mapLiteralValues[tempStringLiteral] == true && iteratorVectorString == tempVector.end())
									helpVariable *= 0;
					
								else if(mapLiteralValues[tempStringLiteral] == false && iteratorVectorString != tempVector.end())
									helpVariable *= 0;
					
								else if(mapLiteralValues[tempStringLiteral] == false && iteratorVectorString == tempVector.end())
									helpVariable *= 1;

								if(mapLiteralValues[tempStringLiteral] == true)
									trueLiteralCounter++;

								clause += tempStringLiteral;
								clause += " ";
						   }

						   if(clause[clause.length() - 1] == ' ')
							   clause.erase(clause.length() - 1);
				
						   vectorClauses.push_back(clause);

							if(trueLiteralCounter == 0)
								trueClauseCounter--;
			
							else if(helpVariable == 1)
								trueClauseCounter++;

							//Reset
							helpVariable = 1;

							//Reset
							trueLiteralCounter = 0;
					   }
					}
				}

				//Set the gain
				gain = trueClauseCounter;

				//Clear
				tempVector.clear();

				//Reset
				vectorClause.clear();

				//Reset
				vectorClauses.clear();

				//Reset
				tempStringLiteral = "";

				//Reset
				tempStringLiteral2 = "";

				//Reset
				clause = "";

				//Reset
				trueClauseCounter = 0;

				//Tabu
				if(mapTabuList[atoi(stringLiteral.c_str())] > 0)
				{
					//Store literal and its gain
					mapBestLiterals[gain] = atoi(stringLiteral.c_str());
				}

				//Not Tabu
				else
				{
					if(gain == bestGain)
					{
						bestGain = gain;

						//Choose random
						int newRandomIndex = rand() % 2;

						if(yesNo[newRandomIndex] == true)
							//Store literal and its gain
							mapBestLiterals[bestGain] = atoi(stringLiteral.c_str());
					}

					else if(gain > bestGain)
					{
						//Update best gain
						bestGain = gain;

						//Clear
						mapBestLiterals.clear();
					
						//Store literal and its gain
						mapBestLiterals[bestGain] = atoi(stringLiteral.c_str());
					}
				}

				//Flip back
				flip(mapLiteralValues, stringLiteral);

				//Reset
				vectorClauseUnvisitedLiterals.clear();
			}
		}

		//Find best literal
		if(mapBestLiterals.size() != 0)
		{
			std::map<int, int>::iterator iteratorMapMax;
			iteratorMapMax = std::max_element(mapBestLiterals.begin(), mapBestLiterals.end());
			int bestLiteral = iteratorMapMax->second;

			//int -> string
			char sizeTemp = (char)bestLiteral;
			char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
			stringLiteral = itoa(bestLiteral, bufferTemp, 10);

			//If no improvement and best literal is tabu, then don't flip
			if(iteratorMapMax->first <= 0 && mapTabuList[atoi(stringLiteral.c_str())] > 0)
			{
				//Print satisfied clauses
				trueClauseCounter = 0;
				for(iteratorMap = mapClauseValues.begin(); iteratorMap != mapClauseValues.end(); iteratorMap++) 
				{
					if(iteratorMap->second == true)
						trueClauseCounter++;
				}

				current = trueClauseCounter;

				//Reset
				trueClauseCounter = 0;

				std::cout << "Number of TRUE clauses: " << std::endl << std::endl << current << std::endl << std::endl;
				foutput << "Number of TRUE clauses: \n";
				foutput << current << "\n";
				foutput << "Gain: "<< 0 << "\n";

				if(current == NUMBER_OF_CLAUSES)
				{
					satisfaction = "SATISFIED";
					std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << " at LEVEL " << levelCounter << "." << std::endl << std::endl;
					foutput << "SATISFIED!\n";
				}
				else
				{
					satisfaction = "NOT SATISFIED";
					std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << " at LEVEL " << levelCounter << "." << std::endl << std::endl;
					foutput << "NOT SATISFIED!\n";
				}
			}
			else
			{
				//Flip best literal
				flip(mapLiteralValues, stringLiteral);

				std::cout << "UPDATING SAT FORMULA WITH NEW LITERAL VALUES....." << std::endl << std::endl;

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

						if(trueCounter >= 1)
							//Set the clause to TRUE
							mapClauseValues[stringClause] = true;
						
						else
							//Set the clause to FALSE
							mapClauseValues[stringClause] = false;

						//Reset
						stringClause = "";
						trueCounter = 0;
					}
				}

				//Print satisfied clauses
				trueClauseCounter = 0;
				for(iteratorMap = mapClauseValues.begin(); iteratorMap != mapClauseValues.end(); iteratorMap++) 
				{
					if(iteratorMap->second == true)
						trueClauseCounter++;
				}

				current = trueClauseCounter;

				//Reset
				trueClauseCounter = 0;

				std::cout << "Number of TRUE clauses: " << std::endl << std::endl << current << std::endl << std::endl;
				foutput << "Number of TRUE clauses: \n";
				foutput << current << "\n";
				foutput << "Gain: "<< iteratorMapMax->first << "\n";

				if(current == NUMBER_OF_CLAUSES)
				{
					satisfaction = "SATISFIED";
					std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << " at LEVEL " << levelCounter << "." << std::endl << std::endl;
					foutput << "SATISFIED!\n";
				}
				else
				{
					satisfaction = "NOT SATISFIED";
					std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << " at LEVEL " << levelCounter << "." << std::endl << std::endl;
					foutput << "NOT SATISFIED!\n";
				}
			}

			//Finished!
			if(satisfaction == "SATISFIED")
				break;

			else
			{
				if(current >= bestSoFar)
				{
					//Update best so far
					bestSoFar = current;
				}

				//Make best literal Tabu (if not tabu)
				if(mapTabuList[atoi(stringLiteral.c_str())] == 0)
					mapTabuList[atoi(stringLiteral.c_str())] = 1;

				//Update tabu list
				for(int i = 1; i <= mapTabuList.size(); i++)
				{
					if(i != atoi(stringLiteral.c_str()) && mapTabuList[i] != 0)
					{
						mapTabuList[i] -= 1;
					}
				}
			}
		}

		//Reset
		vectorUnsatisfiedClauses.clear();

		//Reset
		vectorVisitedLiterals.clear();

		//Clear
		vectorClauseUnvisitedLiterals.clear();

		//Reset
		mapBestLiterals.clear();

		//Reset
		stringLiteral = "";

		//End time of iteration
		clock_t endIterationTime = clock();

		//Update total iteration time
		totalIterationTime += te.GetTimeElapsed(endIterationTime, beginIterationTime) / 1000;

		//Show time elapsed
		std::cout << "Time elapsed: " << totalIterationTime << " seconds" << std::endl << std::endl;		
		foutput << "Time elapsed: " << totalIterationTime << " seconds\n\n";
	}	
	**TS VERSION 3 END**/

	/**TS VERSION 4 START**
	while(current < NUMBER_OF_CLAUSES && totalIterationTime < maxIterationTime)
	{
		//Start time of iteration
		clock_t beginIterationTime = clock();

		//Set initial gains
		bestGain = -999;
		gain = 0;

		//Find unsatisfied clauses
		for(iteratorMap = mapClauseValues.begin(); iteratorMap != mapClauseValues.end(); iteratorMap++) 
		{
			if(iteratorMap->second == false)
				vectorUnsatisfiedClauses.push_back(iteratorMap->first);
		}

		for(int i = 0; i < NUMBER_OF_CLAUSES - current; i++)
		{
			//Reset
			stringLiteral = "";

			//Pick an unsatisfied clause
			stringClause = vectorUnsatisfiedClauses[i];

			//Pick an unvisited literal inside the clause
			for(int i = 0; i < stringClause.length(); i++)
			{
				if(stringClause[i] == ' ' || i == stringClause.length() - 1)
				{
					if(stringClause[i] != ' ' && i == stringClause.length() - 1)
						stringLiteral += stringClause[i];

					if(atoi(stringLiteral.c_str()) < 0)
					{
						int temp = atoi(stringLiteral.c_str()) * -1;
						char sizeTempOpposite = (char)temp;
						char bufferTempOpposite[sizeof(sizeTempOpposite)/sizeof(char) + 10];
						std::string oppositeLiteral = itoa(temp, bufferTempOpposite, 10);
						stringLiteral = oppositeLiteral;
					}

					vectorClauseUnvisitedLiterals.push_back(stringLiteral);

					//Reset
					stringLiteral = "";
				}
				else
					stringLiteral += stringClause[i];
			}

			//Pick a randomly unvisited literal
			std::random_shuffle(vectorClauseUnvisitedLiterals.begin(), vectorClauseUnvisitedLiterals.end());
			stringLiteral = vectorClauseUnvisitedLiterals[0];
			vectorClauseUnvisitedLiterals.erase(vectorClauseUnvisitedLiterals.begin());

			//Check if visited
			iteratorVector = std::find(vectorVisitedLiterals.begin(), vectorVisitedLiterals.end(), atoi(stringLiteral.c_str()));

			//While visited, pick another
			while(iteratorVector != vectorVisitedLiterals.end())
			{
				std::random_shuffle(vectorClauseUnvisitedLiterals.begin(), vectorClauseUnvisitedLiterals.end());
				stringLiteral = vectorClauseUnvisitedLiterals[0];
				vectorClauseUnvisitedLiterals.erase(vectorClauseUnvisitedLiterals.begin());

				if(vectorClauseUnvisitedLiterals.size() == 0)
				{
					stringLiteral = "";
					break;
				}
			}

			if(stringLiteral != "")
			{
				//Put in visited literals vector
				vectorVisitedLiterals.push_back(atoi(stringLiteral.c_str()));

				//Flip
				flip(mapLiteralValues, stringLiteral);

				std::cout << "Calculating gains....." << std::endl << std::endl;
	
				//Put in tempVector
				tempVector.push_back(stringLiteral);

				//Opposite value
				int oppositeLiteral = atoi(stringLiteral.c_str()) * -1;

				//int -> string
				char sizeTemp = (char)oppositeLiteral;
				char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
				
				std::string oppositeStringLiteral = itoa(oppositeLiteral, bufferTemp, 10);

				//Put in tempVector
				tempVector.push_back(oppositeStringLiteral);

				//Find the clauses that have this literal and update number of true clauses found
				for(int j = 0; j < tempVector.size(); j++)
				{
					tempStringLiteral2 = tempVector[j];

					//Find the clauses that have this literal
					multiMapIterator = multiMapLiteralClause.equal_range(tempStringLiteral2);

					//Check each clause
					for(std::multimap<std::string, std::vector<std::string>>::iterator multiMapIterator2 = multiMapIterator.first; multiMapIterator2 != multiMapIterator.second; ++multiMapIterator2)
					{
					   //Next clause
					   vectorClause = (*multiMapIterator2).second;

					   //Reset
					   clause = "";
		
					   //Check if the clause has been handled already
					   for(int i = 0; i < vectorClause.size(); i++)
					   {
							clause += vectorClause[i];
							clause += " ";
					   }

					   if(clause[clause.length() - 1] == ' ')
						   clause.erase(clause.length() - 1);

					   iteratorVectorString = std::find(vectorClauses.begin(), vectorClauses.end(), clause);

					   //Not found, handle it
					   if(iteratorVectorString == vectorClauses.end())
					   {
						   clause = "";

						   for(int i = 0; i < vectorClause.size(); i++)
						   {
								tempStringLiteral = vectorClause[i];

								//Check this literal
								iteratorVectorString = std::find(tempVector.begin(), tempVector.end(), tempStringLiteral);

								if(mapLiteralValues[tempStringLiteral] == true && iteratorVectorString != tempVector.end())
									helpVariable *= 1;

								else if(mapLiteralValues[tempStringLiteral] == true && iteratorVectorString == tempVector.end())
									helpVariable *= 0;
					
								else if(mapLiteralValues[tempStringLiteral] == false && iteratorVectorString != tempVector.end())
									helpVariable *= 0;
					
								else if(mapLiteralValues[tempStringLiteral] == false && iteratorVectorString == tempVector.end())
									helpVariable *= 1;

								if(mapLiteralValues[tempStringLiteral] == true)
									trueLiteralCounter++;

								clause += tempStringLiteral;
								clause += " ";
						   }

						   if(clause[clause.length() - 1] == ' ')
							   clause.erase(clause.length() - 1);
				
						   vectorClauses.push_back(clause);

							if(trueLiteralCounter == 0)
								trueClauseCounter--;
			
							else if(helpVariable == 1)
								trueClauseCounter++;

							//Reset
							helpVariable = 1;

							//Reset
							trueLiteralCounter = 0;
					   }
					}
				}

				//Set the gain
				gain = trueClauseCounter;

				//Clear
				tempVector.clear();

				//Reset
				vectorClause.clear();

				//Reset
				vectorClauses.clear();

				//Reset
				tempStringLiteral = "";

				//Reset
				tempStringLiteral2 = "";

				//Reset
				clause = "";

				//Reset
				trueClauseCounter = 0;

				//Tabu
				if(mapTabuList[atoi(stringLiteral.c_str())] > 0)
				{
					//Store literal and its gain
					mapBestLiterals[gain] = atoi(stringLiteral.c_str());
				}

				//Not Tabu
				else
				{
					if(gain == bestGain)
					{
						bestGain = gain;

						//Choose random
						int newRandomIndex = rand() % 2;

						if(yesNo[newRandomIndex] == true)
							//Store literal and its gain
							mapBestLiterals[bestGain] = atoi(stringLiteral.c_str());
					}

					else if(gain > bestGain)
					{
						//Update best gain
						bestGain = gain;

						//Clear
						mapBestLiterals.clear();
					
						//Store literal and its gain
						mapBestLiterals[bestGain] = atoi(stringLiteral.c_str());
					}
				}

				//Flip back
				flip(mapLiteralValues, stringLiteral);

				//Reset
				vectorClauseUnvisitedLiterals.clear();
			}
		}

		//Find best literal
		if(mapBestLiterals.size() != 0)
		{
			std::map<int, int>::iterator iteratorMapMax;
			iteratorMapMax = std::max_element(mapBestLiterals.begin(), mapBestLiterals.end());
			int bestLiteral = iteratorMapMax->second;

			//int -> string
			char sizeTemp = (char)bestLiteral;
			char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
			stringLiteral = itoa(bestLiteral, bufferTemp, 10);

			//If no improvement and best literal is tabu, then don't flip
			if(iteratorMapMax->first <= 0 && mapTabuList[atoi(stringLiteral.c_str())] > 0)
			{
				//Print satisfied clauses
				trueClauseCounter = 0;
				for(iteratorMap = mapClauseValues.begin(); iteratorMap != mapClauseValues.end(); iteratorMap++) 
				{
					if(iteratorMap->second == true)
						trueClauseCounter++;
				}

				current = trueClauseCounter;

				//Reset
				trueClauseCounter = 0;

				std::cout << "Number of TRUE clauses: " << std::endl << std::endl << current << std::endl << std::endl;
				foutput << "Number of TRUE clauses: \n";
				foutput << current << "\n";
				foutput << "Gain: "<< 0 << "\n";

				if(current == NUMBER_OF_CLAUSES)
				{
					satisfaction = "SATISFIED";
					std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << " at LEVEL " << levelCounter << "." << std::endl << std::endl;
					foutput << "SATISFIED!\n";
				}
				else
				{
					satisfaction = "NOT SATISFIED";
					std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << " at LEVEL " << levelCounter << "." << std::endl << std::endl;
					foutput << "NOT SATISFIED!\n";
				}
			}
			else
			{
				//Flip best literal
				flip(mapLiteralValues, stringLiteral);

				std::cout << "UPDATING SAT FORMULA WITH NEW LITERAL VALUES....." << std::endl << std::endl;

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

						if(trueCounter >= 1)
							//Set the clause to TRUE
							mapClauseValues[stringClause] = true;
						
						else
							//Set the clause to FALSE
							mapClauseValues[stringClause] = false;

						//Reset
						stringClause = "";
						trueCounter = 0;
					}
				}

				//Print satisfied clauses
				trueClauseCounter = 0;
				for(iteratorMap = mapClauseValues.begin(); iteratorMap != mapClauseValues.end(); iteratorMap++) 
				{
					if(iteratorMap->second == true)
						trueClauseCounter++;
				}

				current = trueClauseCounter;

				//Reset
				trueClauseCounter = 0;

				std::cout << "Number of TRUE clauses: " << std::endl << std::endl << current << std::endl << std::endl;				
				foutput << "Number of TRUE clauses: \n";
				foutput << current << "\n";
				foutput << "Gain: "<< iteratorMapMax->first << "\n";

				if(current == NUMBER_OF_CLAUSES)
				{
					satisfaction = "SATISFIED";
					std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << " at LEVEL " << levelCounter << "." << std::endl << std::endl;
					foutput << "SATISFIED!\n";
				}
				else
				{
					satisfaction = "NOT SATISFIED";
					std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << " at LEVEL " << levelCounter << "." << std::endl << std::endl;
					foutput << "NOT SATISFIED!\n";
				}
			}

			//Finished!
			if(satisfaction == "SATISFIED")
				break;

			else
			{
				if(current >= bestSoFar)
				{
					//Update best so far
					bestSoFar = current;
				}

				//Make best literal Tabu (if not tabu)
				int optimalTabuLength = 0.01875 * atoi(numberOfXLiterals.c_str()) + 2.8125;

				if(mapTabuList[atoi(stringLiteral.c_str())] == 0)
					mapTabuList[atoi(stringLiteral.c_str())] = optimalTabuLength;

				//Update tabu list
				for(int i = 1; i <= mapTabuList.size(); i++)
				{
					if(i != atoi(stringLiteral.c_str()) && mapTabuList[i] != 0)
					{
						mapTabuList[i] -= 1;
					}
				}
			}
		}

		//Reset
		vectorUnsatisfiedClauses.clear();

		//Reset
		vectorVisitedLiterals.clear();

		//Clear
		vectorClauseUnvisitedLiterals.clear();

		//Reset
		mapBestLiterals.clear();

		//Reset
		stringLiteral = "";

		//End time of iteration
		clock_t endIterationTime = clock();

		//Update total iteration time
		totalIterationTime += te.GetTimeElapsed(endIterationTime, beginIterationTime) / 1000;

		//Show time elapsed
		std::cout << "Time elapsed: " << totalIterationTime << " seconds" << std::endl << std::endl;
		foutput << "Time elapsed: " << totalIterationTime << " seconds\n\n";
	}	
	**TS VERSION 4 END**/
}

void TabuSearch::flip(std::map<std::string, bool> &mapLiteralValues, std::string stringLiteral)
{	
	std::cout << "Flipping....." << std::endl << std::endl;

	//Flip TRUE to FALSE
	if(mapLiteralValues[stringLiteral] == true)
	{
		//Flip cluster to FALSE
		mapLiteralValues[stringLiteral] = false;

		//Opposite literal value to TRUE
		int temp = atoi(stringLiteral.c_str()) * -1;
		char sizeTempOpposite = (char)temp;
		char bufferTempOpposite[sizeof(sizeTempOpposite)/sizeof(char) + 10];
		std::string oppositeTempLiteral = itoa(temp, bufferTempOpposite, 10);

		mapLiteralValues[oppositeTempLiteral] = true;
	}
		
	//Flip FALSE to TRUE
	else
	{
		//Flip cluster to TRUE
		mapLiteralValues[stringLiteral] = true;

		//Opposite literal value to FALSE
		int temp = atoi(stringLiteral.c_str()) * -1;
		char sizeTempOpposite = (char)temp;
		char bufferTempOpposite[sizeof(sizeTempOpposite)/sizeof(char) + 10];
		std::string oppositeTempLiteral = itoa(temp, bufferTempOpposite, 10);

		mapLiteralValues[oppositeTempLiteral] = false;
	}
}

TabuSearch::~TabuSearch(void)
{
}