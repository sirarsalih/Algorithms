#include "LearningAutomata.h"
#include "TimeElapsed.h"
#include <sys/stat.h>
#include <sstream>

TabuSearch::TabuSearch(void)
{
}

void TabuSearch::run(std::ofstream &foutput, std::map<std::string, int> &mapClustersStates, int &bestSoFar, std::string noLettersCollectionString, int &numberOfLiterals, int levelCounter, std::map<std::string, std::string> mapLevelClusters, std::vector<std::string> vectorStringNumbers, std::vector<std::string> &initialClusterCollection, std::vector<std::string> clusterCollection, std::map<std::string, bool> &mapLiteralValues, std::multimap<std::string, bool> &mapClauseValues, std::string numberOfXLiterals, std::string numberOfClauses, std::multimap<std::string, std::vector<std::string>> &multiMapLiteralClause, clock_t begin)
{
	std::cout << "Running Learning Automata....." << std::endl << std::endl;

	const int NUMBER_OF_CLAUSES = atoi(numberOfClauses.c_str());	//Number of clauses
	std::map<std::string, int> mapTabuList;							//Map contains tabu list
	std::map<int, std::string> mapBestClusters;						//Map contains best literals
	std::map<int, int> mapBestLiterals;								//Map contains best literals
	std::vector<std::string> vectorLookUp;
	//std::vector<int> vectorNoImprovement;
	std::vector<std::string> vectorCheckedClauses;
	std::vector<std::string> vectorSatisfiedClauses;
	std::vector<std::string> vectorUnsatisfiedClauses;
	std::vector<std::string> vectorClauseUnvisitedLiterals;			//Vector contains unvisited literals from unsatisfied clause
	std::vector<int> vectorVisitedLiterals;							//Vector contains visited literals
	int clusterSize = clusterCollection.size();
	std::pair<std::multimap<std::string, std::vector<std::string>>::iterator, std::multimap<std::string, std::vector<std::string>>::iterator> multiMapIterator;
	std::vector<std::string> vectorClause;
	std::vector<std::string> vectorClauses;
	std::vector<std::string> tempVector;
	std::vector<std::string> vectorClusterLiterals;
	std::string stringLiteral = "";
	std::string tempStringLiteral = "";
	std::string tempStringLiteral2 = "";
	std::string stringClause = "";
	std::string clause = "";
	int trueLiteralCounter = 0;
	int helpVariable = 1;
	bool yesNo[2] = {true, false};
	bool firstTime = true;
	std::string satisfaction = "";
	int trueClauseCounter = 0;
	int current = bestSoFar;
	int bestGain = 0;
	int gain = 0;
	int randomIndex = 0;
	std::string cluster = "";
	double flipCounter = 0;
	double powerCounter = 1;
	double numberOfFlips = pow(10.0, 6) / (levelCounter + 1);

	//Iterator vector, to help find visited literals
	std::vector<int>::iterator iteratorVector;

	//Iterator vector, to help find elements
	std::vector<std::string>::iterator iteratorVectorString;

	//Iterator vector, to help find elements
	std::vector<int>::iterator iteratorVectorIndex;

	//Total iteration time
	//double maxIterationTime = 3600;
	double totalIterationTime = 0;

	//Object to calculate time
	TimeElapsed te;

	/**Initialize Tabu list and vector look up**/
	for(int i = 0; i < clusterSize; i++)
	{
		mapTabuList[clusterCollection[i]] = 0;
		vectorLookUp.push_back(clusterCollection[i]);
	}

	while(current < NUMBER_OF_CLAUSES)
	{
		//Start time of iteration
		clock_t beginIterationTime = clock();

		//If we have arrived at the last level, then normal LATS will run
		if(levelCounter == 0)
		{
			if(firstTime)
			{
				firstTime = false;

				//Reset for safety
				mapClauseValues.clear();
				vectorUnsatisfiedClauses.clear();
				vectorSatisfiedClauses.clear();

				//Update SAT and find satisfied + unsatisfied clauses
				std::cout << "UPDATING SAT FORMULA....." << std::endl << std::endl;

				trueClauseCounter = 0;
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
								vectorSatisfiedClauses.push_back(stringClause);
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

				current = trueClauseCounter;

				if(current >= bestSoFar)
				{
					//Update best so far
					bestSoFar = current;
				}

				//Reset
				trueClauseCounter = 0;
				vectorCheckedClauses.clear();
			}

			//Clear
			vectorClauseUnvisitedLiterals.clear();

			/**Learning Automata**/

			//Randomly pick an unsatisfied clause
			randomIndex = rand() % vectorUnsatisfiedClauses.size();
			stringClause = vectorUnsatisfiedClauses[randomIndex];

			//Randomly pick a literal or its negation from inside the clause
			stringLiteral = "";

			for(int i = 0; i < stringClause.length(); i++)
			{
				if(stringClause[i] == ' ' || i == stringClause.length() - 1)
				{
					if(stringClause[i] != ' ' && i == stringClause.length() - 1)
						stringLiteral += stringClause[i];

					stringLiteral += '#';

					vectorClauseUnvisitedLiterals.push_back(stringLiteral);

					stringLiteral.erase(stringLiteral.length() - 1);

					int temp = atoi(stringLiteral.c_str()) * -1;
					char sizeTempOpposite = (char)temp;
					char bufferTempOpposite[sizeof(sizeTempOpposite)/sizeof(char) + 10];
					std::string oppositeLiteral = itoa(temp, bufferTempOpposite, 10);
					stringLiteral = oppositeLiteral;

					vectorClauseUnvisitedLiterals.push_back(stringLiteral);

					//Reset
					stringLiteral = "";
				}
				else
					stringLiteral += stringClause[i];
			}

			randomIndex = rand() % vectorClauseUnvisitedLiterals.size();
			stringLiteral = vectorClauseUnvisitedLiterals[randomIndex];

			//Clear
			vectorClauseUnvisitedLiterals.clear();
			stringClause = "";		

			//Increase state
			if(stringLiteral[stringLiteral.length() - 1] == '#' && mapClustersStates[stringLiteral] < NUMBER_OF_CLAUSES - current)
			{
				stringLiteral.erase(stringLiteral.length() - 1);

				mapClustersStates[stringLiteral] += 1;

				if(mapClustersStates[stringLiteral] == 0)
				{
					//Flip
					flip(mapLiteralValues, stringLiteral);

					//Increase the flip counter
					flipCounter++;

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

									iteratorVectorString = std::find(vectorSatisfiedClauses.begin(), vectorSatisfiedClauses.end(), clause);
									int index = iteratorVectorString - vectorSatisfiedClauses.begin();

									if(iteratorVectorString != vectorSatisfiedClauses.end())
										vectorSatisfiedClauses.erase(vectorSatisfiedClauses.begin() + index);
								}
			
								//Satisfied, remove it from unsatisfied vector
								else if(helpVariable == 1)
								{
									trueClauseCounter++;
									iteratorVectorString = std::find(vectorUnsatisfiedClauses.begin(), vectorUnsatisfiedClauses.end(), clause);
									int index = iteratorVectorString - vectorUnsatisfiedClauses.begin();

									if(iteratorVectorString != vectorUnsatisfiedClauses.end())
										vectorUnsatisfiedClauses.erase(vectorUnsatisfiedClauses.begin() + index);

									vectorSatisfiedClauses.push_back(clause);
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
					foutput << levelCounter << "                         " << bestSoFar << "                         " << totalIterationTime << "                         " << flipCounter << "\n";

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

					if(flipCounter >= numberOfFlips && totalIterationTime >= 3600)
					{
						foutput << levelCounter << "                         " << bestSoFar << "                         " << totalIterationTime << "                         " << flipCounter << "\n";
						powerCounter = 1;
						break;
					}

					else if(flipCounter >= pow(10, powerCounter) && powerCounter < 6)
					{
						foutput << levelCounter << "                         " << bestSoFar << "                         " << totalIterationTime << "                         " << flipCounter << "\n";
						powerCounter++;
					}
				}
			}
			//Decrease state
			else if(stringLiteral[stringLiteral.length() - 1] != '#' && mapClustersStates[stringLiteral] > -(NUMBER_OF_CLAUSES - current))
			{
				mapClustersStates[stringLiteral] -= 1;

				if(mapClustersStates[stringLiteral] == -1)
				{
					//Flip
					flip(mapLiteralValues, stringLiteral);

					//Increase the flip counter
					flipCounter++;

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

									iteratorVectorString = std::find(vectorSatisfiedClauses.begin(), vectorSatisfiedClauses.end(), clause);
									int index = iteratorVectorString - vectorSatisfiedClauses.begin();

									if(iteratorVectorString != vectorSatisfiedClauses.end())
										vectorSatisfiedClauses.erase(vectorSatisfiedClauses.begin() + index);
								}
			
								//Satisfied, remove it from unsatisfied vector
								else if(helpVariable == 1)
								{
									trueClauseCounter++;
									iteratorVectorString = std::find(vectorUnsatisfiedClauses.begin(), vectorUnsatisfiedClauses.end(), clause);
									int index = iteratorVectorString - vectorUnsatisfiedClauses.begin();

									if(iteratorVectorString != vectorUnsatisfiedClauses.end())
										vectorUnsatisfiedClauses.erase(vectorUnsatisfiedClauses.begin() + index);

									vectorSatisfiedClauses.push_back(clause);
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
					foutput << levelCounter << "                         " << bestSoFar << "                         " << totalIterationTime << "                         " << flipCounter << "\n";

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

					if(flipCounter >= numberOfFlips && totalIterationTime >= 3600)
					{
						foutput << levelCounter << "                         " << bestSoFar << "                         " << totalIterationTime << "                         " << flipCounter << "\n";
						powerCounter = 1;
						break;
					}

					else if(flipCounter >= pow(10, powerCounter) && powerCounter < 6)
					{
						foutput << levelCounter << "                         " << bestSoFar << "                         " << totalIterationTime << "                         " << flipCounter << "\n";
						powerCounter++;
					}
				}
			}

			//Clear
			vectorClauseUnvisitedLiterals.clear();

			//Randomly pick a satisfied clause
			randomIndex = rand() % vectorSatisfiedClauses.size();
			stringClause = vectorSatisfiedClauses[randomIndex];

			//Randomly pick a literal or its negation from inside the clause
			stringLiteral = "";

			for(int i = 0; i < stringClause.length(); i++)
			{
				if(stringClause[i] == ' ' || i == stringClause.length() - 1)
				{
					if(stringClause[i] != ' ' && i == stringClause.length() - 1)
						stringLiteral += stringClause[i];

					stringLiteral += '#';

					vectorClauseUnvisitedLiterals.push_back(stringLiteral);

					stringLiteral.erase(stringLiteral.length() - 1);

					int temp = atoi(stringLiteral.c_str()) * -1;
					char sizeTempOpposite = (char)temp;
					char bufferTempOpposite[sizeof(sizeTempOpposite)/sizeof(char) + 10];
					std::string oppositeLiteral = itoa(temp, bufferTempOpposite, 10);
					stringLiteral = oppositeLiteral;

					vectorClauseUnvisitedLiterals.push_back(stringLiteral);

					//Reset
					stringLiteral = "";
				}
				else
					stringLiteral += stringClause[i];
			}

			randomIndex = rand() % vectorClauseUnvisitedLiterals.size();
			stringLiteral = vectorClauseUnvisitedLiterals[randomIndex];

			//Increase state
			if(stringLiteral[stringLiteral.length() - 1] == '#' && mapClustersStates[stringLiteral] >= 0 && mapClustersStates[stringLiteral] < NUMBER_OF_CLAUSES - current)
			{
				stringLiteral.erase(stringLiteral.length() - 1);
				mapClustersStates[stringLiteral] += 1;		
			}
			//Decrease state
			else if(stringLiteral[stringLiteral.length() - 1] != '#' && mapClustersStates[stringLiteral] < 0 && mapClustersStates[stringLiteral] > -(NUMBER_OF_CLAUSES - current))
			{
				mapClustersStates[stringLiteral] -= 1;
			}

			//Clear
			vectorClauseUnvisitedLiterals.clear();
			stringClause = "";
			stringLiteral = "";

			/**Tabu Search version 2.0**/

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
					if(mapTabuList[stringLiteral] == 0)
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
							if(gain + current <= NUMBER_OF_CLAUSES)
							{
								//Update best gain
								bestGain = gain;

								//Clear
								mapBestLiterals.clear();
					
								//Store literal and its gain
								mapBestLiterals[bestGain] = atoi(stringLiteral.c_str());
							}
						}
					}	

					//Flip back
					flip(mapLiteralValues, stringLiteral);

					//Increase the flip counter
					flipCounter++;

					if(flipCounter >= numberOfFlips && totalIterationTime >= 3600)
					{
						foutput << levelCounter << "                         " << bestSoFar << "                         " << totalIterationTime << "                         " << flipCounter << "\n";
						powerCounter = 1;
						break;
					}

					else if(flipCounter >= pow(10, powerCounter) && powerCounter < 6)
					{
						foutput << levelCounter << "                         " << bestSoFar << "                         " << totalIterationTime << "                         " << flipCounter << "\n";
						powerCounter++;
					}

					//Reset
					vectorClauseUnvisitedLiterals.clear();
				}
			}
		}

		else
		{
			/**Learning Automata**/

			//Randomly pick a cluster from current level
			if(clusterCollection.size() != 0)
			{
				randomIndex = rand() % clusterCollection.size();
				cluster = clusterCollection[randomIndex];
			}
			
			//Re-fill cluster collection and randomly pick a cluster
			else
			{
				//int -> string
				char sizeTemp = (char)levelCounter;
				char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
				std::string stringLevelCounter = itoa(levelCounter, bufferTemp, 10);

				//Fill up cluster collection
				for(int j = 0; j < mapLevelClusters.size(); j++)
				{
					//int -> string
					char sizeTemp = (char)j;
					char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
					std::string counter = itoa(j, bufferTemp, 10);

					if(mapLevelClusters.find(stringLevelCounter + " " + counter) != mapLevelClusters.end())
						clusterCollection.push_back(mapLevelClusters[stringLevelCounter + " " + counter]);
				}

				//Update size
				clusterSize = clusterCollection.size();

				//Randomly pick a cluster
				randomIndex = rand() % clusterCollection.size();
				cluster = clusterCollection[randomIndex];
			}

			std::string stringClusterLiteral = "";

			for(int i = 0; i < cluster.length(); i++)
			{
				if(cluster[i] != ' ')
				{
					stringClusterLiteral += cluster[i];

					if(i == cluster.length() - 1)
					{
						vectorClusterLiterals.push_back(stringClusterLiteral);

						//Reset
						stringClusterLiteral = "";
					}
				}

				else if(cluster[i] == ' ' || i ==  cluster.length() - 1)
				{
					while(cluster[cluster.length() - 1] == ' ')
						cluster.erase(cluster.length() - 1);

					vectorClusterLiterals.push_back(stringClusterLiteral);

					//Reset
					stringClusterLiteral = "";
				}
			}

			std::vector<std::string> vectorTemp;
			std::vector<std::string> vectorStringClause;
			std::string stringClause = "";

			//Randomly pick a literal or its negation
			for(int i = 0; i < vectorClusterLiterals.size(); i++)
			{
				vectorClusterLiterals[i] += '#';

				vectorTemp.push_back(vectorClusterLiterals[i]);

				vectorClusterLiterals[i].erase(vectorClusterLiterals[i].length() - 1);

				int temp = atoi(vectorClusterLiterals[i].c_str()) * -1;
				char sizeTempOpposite = (char)temp;
				char bufferTempOpposite[sizeof(sizeTempOpposite)/sizeof(char) + 10];
				std::string oppositeLiteral = itoa(temp, bufferTempOpposite, 10);

				vectorTemp.push_back(oppositeLiteral);

				randomIndex = rand() % 2;
				stringClusterLiteral = vectorTemp[randomIndex];
				vectorTemp.clear();

				//Pick a clause that has this literal or its negation
				if(stringClusterLiteral[stringClusterLiteral.length() - 1] == '#')
				{
					stringClusterLiteral.erase(stringClusterLiteral.length() - 1);
					vectorStringClause = multiMapLiteralClause.find(stringClusterLiteral)->second;
					std::reverse(vectorStringClause.begin(), vectorStringClause.end());
					stringClusterLiteral += '#';
				}
				else
				{
					vectorStringClause = multiMapLiteralClause.find(stringClusterLiteral)->second;
					std::reverse(vectorStringClause.begin(), vectorStringClause.end());
				}

				for(int j = 0; j < vectorStringClause.size(); j++)
				{
					stringClause += vectorStringClause[j];
					
					if(j != vectorStringClause.size() - 1)
						stringClause += ' ';
				}

				bool clauseValue = mapClauseValues.find(stringClause)->second;
				stringClause = "";

				//Unsatisfied clause
				if(clauseValue == false)
				{
					//Increase state
					if(stringClusterLiteral[stringClusterLiteral.length() - 1] == '#' && mapClustersStates[stringLiteral] < NUMBER_OF_CLAUSES - current)
					{
						stringClusterLiteral.erase(stringClusterLiteral.length() - 1);

						mapClustersStates[stringClusterLiteral] += 1;

						if(mapClustersStates[stringClusterLiteral] == 0)
						{
							//Flip
							flip(mapLiteralValues, stringClusterLiteral);

							//Increase the flip counter
							flipCounter++;

							std::cout << "UPDATING SAT FORMULA WITH NEW LITERAL VALUES....." << std::endl << std::endl;

							//Put in tempVector
							tempVector.push_back(stringClusterLiteral);

							//Opposite value
							int oppositeLiteral = atoi(stringClusterLiteral.c_str()) * -1;

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

										//Unsatisfied
										if(trueLiteralCounter == 0)
											trueClauseCounter--;
			
										//Satisfied
										else if(helpVariable == 1)
											trueClauseCounter++;

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
							foutput << levelCounter << "                         " << bestSoFar << "                         " << totalIterationTime << "                         " << flipCounter << "\n";

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

							if(flipCounter >= numberOfFlips)
							{
								foutput << levelCounter << "                         " << bestSoFar << "                         " << totalIterationTime << "                         " << flipCounter << "\n";
								powerCounter = 1;
								break;
							}

							else if(flipCounter >= pow(10, powerCounter))
							{
								foutput << levelCounter << "                         " << bestSoFar << "                         " << totalIterationTime << "                         " << flipCounter << "\n";
								powerCounter++;
							}
						}
					}
					//Decrease state
					else if(stringClusterLiteral[stringClusterLiteral.length() - 1] != '#' && mapClustersStates[stringClusterLiteral] > -(NUMBER_OF_CLAUSES - current))
					{
						mapClustersStates[stringClusterLiteral] -= 1;

						if(mapClustersStates[stringClusterLiteral] == -1)
						{
							//Flip
							flip(mapLiteralValues, stringClusterLiteral);

							//Increase the flip counter
							flipCounter++;

							std::cout << "UPDATING SAT FORMULA WITH NEW LITERAL VALUES....." << std::endl << std::endl;

							//Put in tempVector
							tempVector.push_back(stringClusterLiteral);

							//Opposite value
							int oppositeLiteral = atoi(stringClusterLiteral.c_str()) * -1;

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

										//Unsatisfied
										if(trueLiteralCounter == 0)
											trueClauseCounter--;
			
										//Satisfied
										else if(helpVariable == 1)
											trueClauseCounter++;

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
							foutput << levelCounter << "                         " << bestSoFar << "                         " << totalIterationTime << "                         " << flipCounter << "\n";

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

							if(flipCounter >= numberOfFlips)
							{
								foutput << levelCounter << "                         " << bestSoFar << "                         " << totalIterationTime << "                         " << flipCounter << "\n";
								powerCounter = 1;
								break;
							}

							else if(flipCounter >= pow(10, powerCounter))
							{
								foutput << levelCounter << "                         " << bestSoFar << "                         " << totalIterationTime << "                         " << flipCounter << "\n";
								powerCounter++;
							}
						}
					}
				}

				//Satisfied clause
				else
				{
					//Increase state
					if(stringClusterLiteral[stringClusterLiteral.length() - 1] == '#' && mapClustersStates[stringClusterLiteral] >= 0 && mapClustersStates[stringClusterLiteral] < NUMBER_OF_CLAUSES - current)
					{
						stringClusterLiteral.erase(stringClusterLiteral.length() - 1);
						mapClustersStates[stringClusterLiteral] += 1;		
					}
					//Decrease state
					else if(stringClusterLiteral[stringClusterLiteral.length() - 1] != '#' && mapClustersStates[stringClusterLiteral] < 0 && mapClustersStates[stringClusterLiteral] > -(NUMBER_OF_CLAUSES - current))
					{
						mapClustersStates[stringClusterLiteral] -= 1;
					}
				}				
			}

			/**Tabu Search version 2.0**/
			
			//Set initial gains
			bestGain = -999;
			gain = 0;

			for(int i = 0; i < clusterSize; i++)
			{
				if(clusterCollection.size() != 0)
				{
					//Pick a random cluster
					randomIndex = rand() % clusterCollection.size();
					cluster = clusterCollection[randomIndex];
			
					//Remove that cluster (it is now visited)
					if(clusterCollection.size() == 1)
						clusterCollection.erase(clusterCollection.begin());
					else if(clusterCollection.size() > 1)
						clusterCollection.erase(clusterCollection.begin() + randomIndex);
					
					//Flip
					flip(mapLiteralValues, cluster);

					std::cout << "Calculating gains....." << std::endl << std::endl;
	
					//Literal counter to count the number of literals
					int literalCounter = 0;

					for(int j = 0; j < cluster.length(); j++)
					{
						if(cluster[j] != ' ')
							tempStringLiteral2 += cluster[j];

						if(cluster[j] == ' ' || j == cluster.length() - 1)
						{
							if(tempStringLiteral2 != "")
							{
								literalCounter++;

								//Put in tempVector
								tempVector.push_back(tempStringLiteral2);

								//Opposite value
								int oppositeLiteral = atoi(tempStringLiteral2.c_str()) * -1;

								//int -> string
								char sizeTemp = (char)oppositeLiteral;
								char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
				
								std::string oppositeStringLiteral = itoa(oppositeLiteral, bufferTemp, 10);

								//Put in tempVector
								tempVector.push_back(oppositeStringLiteral);
							}
			
							//Reset
							tempStringLiteral2 = "";
						}
					}

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
					if(mapTabuList[cluster] == 0)
					{
						if(gain == bestGain)
						{
							bestGain = gain;

							//Choose random
							int newRandomIndex = rand() % 2;

							if(yesNo[newRandomIndex] == true)
								//Store cluster and its gain
								mapBestClusters[bestGain] = cluster;
						}

						else if(gain > bestGain)
						{
							if(gain + current <= NUMBER_OF_CLAUSES)
							{
								//Update best gain
								bestGain = gain;

								//Clear
								mapBestClusters.clear();
					
								//Store cluster and its gain
								mapBestClusters[bestGain] = cluster;
							}
						}
					}

					//Increase the flip counter by the number of literals inside the cluster
					flipCounter += literalCounter;

					//Reset
					literalCounter = 0;

					if(flipCounter >= numberOfFlips)
					{
						foutput << levelCounter << "                         " << bestSoFar << "                         " << totalIterationTime << "                         " << flipCounter << "\n";
						powerCounter = 1;
						break;
					}

					else if(flipCounter >= pow(10, powerCounter))
					{
						foutput << levelCounter << "                         " << bestSoFar << "                         " << totalIterationTime << "                         " << flipCounter << "\n";
						powerCounter++;
					}

					//Flip back
					flip(mapLiteralValues, cluster);
				}

				//Re-fill cluster collection and randomly pick a cluster
				else
				{
					//int -> string
					char sizeTemp = (char)levelCounter;
					char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
					std::string stringLevelCounter = itoa(levelCounter, bufferTemp, 10);

					//Fill up cluster collection
					for(int j = 0; j < mapLevelClusters.size(); j++)
					{
						//int -> string
						char sizeTemp = (char)j;
						char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
						std::string counter = itoa(j, bufferTemp, 10);

						if(mapLevelClusters.find(stringLevelCounter + " " + counter) != mapLevelClusters.end())
							clusterCollection.push_back(mapLevelClusters[stringLevelCounter + " " + counter]);
					}

					//Update size
					clusterSize = clusterCollection.size();

					//Pick a random cluster
					randomIndex = rand() % clusterCollection.size();
					cluster = clusterCollection[randomIndex];
			
					//Remove that cluster (it is now visited)
					if(clusterCollection.size() == 1)
						clusterCollection.erase(clusterCollection.begin());
					else if(clusterCollection.size() > 1)
						clusterCollection.erase(clusterCollection.begin() + randomIndex);
				}
			}
		}

		if(levelCounter == 0)
		{
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
				if(iteratorMapMax->first + current < bestSoFar && mapTabuList[stringLiteral] > 0)
				{
					std::cout << "Number of TRUE clauses: " << std::endl << std::endl << bestSoFar << std::endl << std::endl;
					foutput << levelCounter << "                         " << bestSoFar << "                         " << totalIterationTime << "                         " << flipCounter << "\n";

					if(bestSoFar == NUMBER_OF_CLAUSES)
					{
						satisfaction = "SATISFIED";
						std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << " at LEVEL " << levelCounter << "." << std::endl << std::endl;
						//foutput << "LEVEL " << levelCounter << " SATISFIED!\n";
					}
					else
					{
						satisfaction = "NOT SATISFIED";
						std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << " at LEVEL " << levelCounter << "." << std::endl << std::endl;					
						//foutput << "LEVEL " << levelCounter << " NOT SATISFIED!\n";
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

									iteratorVectorString = std::find(vectorSatisfiedClauses.begin(), vectorSatisfiedClauses.end(), clause);
									int index = iteratorVectorString - vectorSatisfiedClauses.begin();

									if(iteratorVectorString != vectorSatisfiedClauses.end())
										vectorSatisfiedClauses.erase(vectorSatisfiedClauses.begin() + index);
								}
			
								//Satisfied, remove it from unsatisfied vector
								else if(helpVariable == 1)
								{
									trueClauseCounter++;
									iteratorVectorString = std::find(vectorUnsatisfiedClauses.begin(), vectorUnsatisfiedClauses.end(), clause);
									int index = iteratorVectorString - vectorUnsatisfiedClauses.begin();

									if(iteratorVectorString != vectorUnsatisfiedClauses.end())
										vectorUnsatisfiedClauses.erase(vectorUnsatisfiedClauses.begin() + index);

									vectorSatisfiedClauses.push_back(clause);
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
					foutput << levelCounter << "                         " << bestSoFar << "                         " << totalIterationTime << "                         " << flipCounter << "\n";

					if(bestSoFar == NUMBER_OF_CLAUSES)
					{
						satisfaction = "SATISFIED";
						std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << " at LEVEL " << levelCounter << "." << std::endl << std::endl;
						//foutput << "LEVEL " << levelCounter << " SATISFIED!\n";
					}
					else
					{
						satisfaction = "NOT SATISFIED";
						std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << " at LEVEL " << levelCounter << "." << std::endl << std::endl;
						//foutput << "LEVEL " << levelCounter << " NOT SATISFIED!\n";
					}

					//Find the Tabu that has best unsatisfied clauses
					std::vector<int> vectorLookUp;

					for(int i = 1; i <= mapTabuList.size(); i++)
					{
						//int -> string
						char sizeTemp = (char)i;
						char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
						std::string stringLiteral = itoa(i, bufferTemp, 10);

						vectorLookUp.push_back(mapTabuList[stringLiteral]);
					}
                        
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
						mapTabuList[stringLiteral] = NUMBER_OF_CLAUSES - current;

					else
					{
						tabuBestUnsatisfied = vectorLookUp[0];

						if(mapTabuList[stringLiteral] == 0 && NUMBER_OF_CLAUSES - current < tabuBestUnsatisfied)
							mapTabuList[stringLiteral] = NUMBER_OF_CLAUSES - current;

						else if(mapTabuList[stringLiteral] == 0 && NUMBER_OF_CLAUSES - current > tabuBestUnsatisfied)
							mapTabuList[stringLiteral] = tabuBestUnsatisfied;

						else if(mapTabuList[stringLiteral] == 0 && NUMBER_OF_CLAUSES - current == tabuBestUnsatisfied)
							mapTabuList[stringLiteral] = tabuBestUnsatisfied;
					}

					//Reset
					vectorLookUp.clear();
				}

				//Update tabu list
				for(int i = 1; i <= mapTabuList.size(); i++)
				{
					//int -> string
					char sizeTemp = (char)i;
					char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
					std::string stringTemp = itoa(i, bufferTemp, 10);

					if(i != atoi(stringLiteral.c_str()) && mapTabuList[stringTemp] != 0)
					{
						mapTabuList[stringTemp] -= 1;
					}
				}

				//No improvement
				/*if(iteratorMapMax->first <= 0)
					vectorNoImprovement.push_back(iteratorMapMax->first);
				else
					vectorNoImprovement.clear();*/
			}

			//Reset
			vectorVisitedLiterals.clear();

			//Clear
			vectorClauseUnvisitedLiterals.clear();

			//Reset
			mapBestLiterals.clear();

			//Reset
			stringLiteral = "";
		}

		else
		{
			//Find best cluster
			if(mapBestClusters.size() != 0)
			{
				std::map<int, std::string>::iterator iteratorMapMax;
				iteratorMapMax = std::max_element(mapBestClusters.begin(), mapBestClusters.end());
				cluster = iteratorMapMax->second;

				//If no improvement and best cluster is tabu, then don't flip
				if(iteratorMapMax->first + current < bestSoFar && mapTabuList[cluster] > 0)
				{
					std::cout << "Number of TRUE clauses: " << std::endl << std::endl << bestSoFar << std::endl << std::endl;
					foutput << levelCounter << "                         " << bestSoFar << "                         " << totalIterationTime << "                         " << flipCounter << "\n";

					if(bestSoFar == NUMBER_OF_CLAUSES)
					{
						satisfaction = "SATISFIED";
						std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << " at LEVEL " << levelCounter << "." << std::endl << std::endl;
						//foutput << "LEVEL " << levelCounter << " SATISFIED!\n";
					}
					else
					{
						satisfaction = "NOT SATISFIED";
						std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << " at LEVEL " << levelCounter << "." << std::endl << std::endl;
						//foutput << "LEVEL " << levelCounter << " NOT SATISFIED!\n";
					}
				}

				else
				{
					//Flip best cluster
					flip(mapLiteralValues, cluster);

					current += iteratorMapMax->first;

					if(current >= bestSoFar)
					{
						//Update best so far
						bestSoFar = current;
					}

					std::cout << "Number of TRUE clauses: " << std::endl << std::endl << bestSoFar << std::endl << std::endl;
					foutput << levelCounter << "                         " << bestSoFar << "                         " << totalIterationTime << "                         " << flipCounter << "\n";

					if(bestSoFar == NUMBER_OF_CLAUSES)
					{
						satisfaction = "SATISFIED";
						std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << " at LEVEL " << levelCounter << "." << std::endl << std::endl;						
						//foutput << "LEVEL " << levelCounter << " SATISFIED!\n";
					}

					else
					{
						satisfaction = "NOT SATISFIED";
						std::cout << "SAT with " << numberOfXLiterals << " literals and " <<  NUMBER_OF_CLAUSES << " clauses is " << satisfaction << " at LEVEL " << levelCounter << "." << std::endl << std::endl;
						//foutput << "LEVEL " << levelCounter << " NOT SATISFIED!\n";
					}

					//Find the Tabu that has best unsatisfied clauses
					std::vector<int> vectorLookUp2;

					for(int i = 0; i < mapTabuList.size(); i++)
						vectorLookUp2.push_back(mapTabuList[vectorLookUp[i]]);
                        
					iteratorVector = std::unique(vectorLookUp2.begin(), vectorLookUp2.end());
					vectorLookUp2.resize(iteratorVector - vectorLookUp2.begin());
					std::sort(vectorLookUp2.begin(), vectorLookUp2.end());

					int tabuBestUnsatisfied = 0;

					while(vectorLookUp2[0] == 0)
					{
						vectorLookUp2.erase(vectorLookUp2.begin());
                                
						//Assign the first Tabu
						if(vectorLookUp2.size() == 0)
							break;
					}

					if(vectorLookUp2.size() == 0)
						mapTabuList[cluster] = NUMBER_OF_CLAUSES - current;

					else
					{
						tabuBestUnsatisfied = vectorLookUp2[0];

						if(mapTabuList[cluster] == 0 && NUMBER_OF_CLAUSES - current < tabuBestUnsatisfied)
							mapTabuList[cluster] = NUMBER_OF_CLAUSES - current;

						else if(mapTabuList[cluster] == 0 && NUMBER_OF_CLAUSES - current > tabuBestUnsatisfied)
							mapTabuList[cluster] = tabuBestUnsatisfied;

						else if(mapTabuList[cluster] == 0 && NUMBER_OF_CLAUSES - current == tabuBestUnsatisfied)
							mapTabuList[cluster] = tabuBestUnsatisfied;
					}

					//Reset
					vectorLookUp2.clear();
				}

				//Update tabu list
				for(int i = 0; i < mapTabuList.size(); i++)
				{
					if(vectorLookUp[i] != cluster && mapTabuList[vectorLookUp[i]] != 0)
						mapTabuList[vectorLookUp[i]] -= 1;
				}

				//No improvement
				/*if(iteratorMapMax->first <= 0)
					vectorNoImprovement.push_back(iteratorMapMax->first);
				else
					vectorNoImprovement.clear();*/
			}

			//Reset
			mapBestClusters.clear();
		}

		//End time of iteration
		clock_t endIterationTime = clock();

		//Update total iteration time
		totalIterationTime += te.GetTimeElapsed(endIterationTime, beginIterationTime) / 1000;

		//Show time elapsed
		std::cout << "Time elapsed: " << totalIterationTime << " seconds" << std::endl << std::endl;
		//foutput << "Time elapsed: " << totalIterationTime << " seconds\n\n";

		//Finished
		if(satisfaction == "SATISFIED" || levelCounter == 0 && flipCounter / numberOfFlips >= 1 && totalIterationTime >= 3600)
			break;

		//Update level counter, cluster size, reset tabu list and re-initialize tabu list
		else if(levelCounter > 0 && flipCounter >= numberOfFlips)
		{
			levelCounter--;

			//Reset
			flipCounter = 0;			

			std::cout << "LEVEL " << levelCounter << std::endl << std::endl;

			//int -> string
			char sizeTemp = (char)levelCounter;
			char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
			std::string stringLevelCounter = itoa(levelCounter, bufferTemp, 10);

			//Clear
			if(clusterCollection.size() != 0)
				clusterCollection.clear();

			//Fill up cluster collection
			for(int j = 0; j < mapLevelClusters.size(); j++)
			{
				//int -> string
				char sizeTemp = (char)j;
				char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
				std::string counter = itoa(j, bufferTemp, 10);

				if(mapLevelClusters.find(stringLevelCounter + " " + counter) != mapLevelClusters.end())
					clusterCollection.push_back(mapLevelClusters[stringLevelCounter + " " + counter]);
			}

			//Update size
			clusterSize = clusterCollection.size();

			//Reset
			vectorLookUp.clear();

			//Reset
			mapTabuList.clear();

			//Last level is reached
			if(levelCounter == 0)
			{
				/**Initialize Tabu list**/
				for(int i = 1; i <= atoi(numberOfXLiterals.c_str()); i++)
				{
					//int -> string
					char sizeTemp = (char)i;
					char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
					std::string stringLiteral = itoa(i, bufferTemp, 10);

					mapTabuList[stringLiteral] = 0;
				}
			}

			else
			{
				/**Initialize Tabu list and vector look up for clusters**/
				for(int i = 0; i < clusterSize; i++)
				{
					mapTabuList[clusterCollection[i]] = 0;
					vectorLookUp.push_back(clusterCollection[i]);
				}
			}
		}
	}
}

void TabuSearch::flip(std::map<std::string, bool> &mapLiteralValues, std::string cluster)
{	
	std::cout << "Flipping....." << std::endl << std::endl;

	std::string stringLiteral = "";
	int helpCounter = 0;

	for(int j = 0; j < cluster.length(); j++)
	{			
		if(helpCounter == 0)
		{
			helpCounter++;

			while(cluster[j] == ' ')
				cluster.erase(cluster.begin());
		}

		if(cluster[j] != ' ')
			stringLiteral += cluster[j];

		if(cluster[j] == ' ' || j == cluster.length() - 1)
		{
			//Flip TRUE to FALSE
			if(mapLiteralValues[stringLiteral] == true)
			{
				//Flip TRUE literal to FALSE
				mapLiteralValues[stringLiteral] = false;

				int oppositeLiteral = atoi(stringLiteral.c_str()) * -1;
	
				//int -> string
				char sizeTemp = (char)oppositeLiteral;
				char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
				std::string oppositeStringLiteral = itoa(oppositeLiteral, bufferTemp, 10);

				//Opposite literal will be TRUE
				mapLiteralValues[oppositeStringLiteral] = true;

				stringLiteral = "";
			}
				
			//Flip FALSE to TRUE
			else if(mapLiteralValues[stringLiteral] == false)
			{
				//Flip literal to TRUE
				mapLiteralValues[stringLiteral] = true;

				int oppositeLiteral = atoi(stringLiteral.c_str()) * -1;

				//int -> string
				char sizeTemp = (char)oppositeLiteral;
				char bufferTemp[sizeof(sizeTemp)/sizeof(char) + 10];
				std::string oppositeStringLiteral = itoa(oppositeLiteral, bufferTemp, 10);

				//Opposite literal will be FALSE
				mapLiteralValues[oppositeStringLiteral] = false;

				stringLiteral = "";
			}
		}
	}
	stringLiteral = "";
	helpCounter = 0;
}

TabuSearch::~TabuSearch(void)
{
}