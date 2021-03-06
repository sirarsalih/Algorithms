#include "TimeElapsed.h"
#include "OpenFileWindow.h"
#include "FileInput.h"
#include "MultilevelClustering.h"
#include "TabuSearch.h"

int main()
{
	std::string stringFilePath = "";					//FileInput Path
	std::vector<std::string> vectorStringNumbers;		//Literals
	int numberOfLiterals = 0;							//Assigned from the size of vectorNumbers
	std::string numberOfXLiterals = "";					//Number of X unique literals
	std::string numberOfClauses = "";					//Will be read from text file
	std::multimap<std::string, 
	std::vector<std::string>> multiMapLiteralClause;	//Multi map for data structure
	std::multimap<std::string, bool> mapClauseValues;	    //TRUE/FALSE SAT clauses map
	std::map<std::string, bool> mapLiteralValues;	    //TRUE/FALSE literals map
	std::vector<std::string> initialClusterCollection;  //Cluster collection that contains initial clusters (made randomly)
	std::vector<std::string> clusterCollection;			//Cluster collection that contains clusters
	int levelCounter = 0;							    //Level counter
	std::map<std::string, std::string> mapLevelClusters;//Map that contains level and the clusters within that level
	int bestSoFar = 0;
	double timeElapsedMultilevelClustering = 0;
	double timeElapsedTabuSearch = 0;
	double totalTimeElapsed = 0;
	std::string outputFile = "";
	std::string problemName = "";

	/**The objects that we need**/
	TimeElapsed te;
	OpenFileWindow ofw;
	FileInput fileInput;
	MultilevelClustering mc;
	TabuSearch ts;

	/**OpenFileDialog**/
	TCHAR * filePath = ofw.open();
	char charFilePath[260] = {0};
	
	if(filePath != NULL)
	{	
		for(int i = 0; i < wcslen((wchar_t*)filePath); i++)
			charFilePath[i] = filePath[i];
		for(int j = 0; j < sizeof(charFilePath)/sizeof(char); j++)
			stringFilePath += charFilePath[j];

		std::string fileFormat = "";
		int counter = 0;
		for(int i = stringFilePath.length() - 1; i > 0; i--)
		{
			if(counter == 3)
				break;

			if(stringFilePath[i] != 0)
			{
				fileFormat += stringFilePath[i];
				counter++;
			}
		}	

		if(fileFormat == "txt")
		{
			/**Read text file (Input)**/
			std::string collectionString = fileInput.readFile(stringFilePath);
			std::string noLettersCollectionString = fileInput.removeLetters(collectionString, numberOfXLiterals, numberOfClauses);

			/**Output file**/
			for(int i = stringFilePath.length() - 1; i > 0; i--)
			{
				if(stringFilePath[i] == '\\')
					break;
				
				if(stringFilePath[i] != 0)
					outputFile += stringFilePath[i];
			}

			std::reverse(outputFile.begin(), outputFile.end());

			while(outputFile[outputFile.length() - 1] != '.')
				outputFile.erase(outputFile.end() - 1);
			
			if(outputFile[outputFile.length() - 1] == '.')
				outputFile.erase(outputFile.end() - 1);

			problemName = outputFile;
			outputFile += "_output.txt";
			std::ofstream foutput(outputFile);
			foutput << "Problem: " << problemName << "\n";

			/**Initialize literals**/
			std::vector<int> vectorNumbers = mc.initializeNumbers(noLettersCollectionString, numberOfLiterals);

			/**Initialize data structure**/
			mc.initializeDataStructure(vectorStringNumbers, vectorNumbers, multiMapLiteralClause);

			/**Start timer**/
			clock_t begin = clock();

			/**Start clustering (Multilevel)**/
			mc.cluster(foutput, bestSoFar, numberOfXLiterals, levelCounter, mapLevelClusters, vectorStringNumbers, numberOfLiterals, numberOfClauses, initialClusterCollection, clusterCollection, mapLiteralValues, mapClauseValues, begin);

			/**Stop timer**/
			clock_t end = clock();

			timeElapsedMultilevelClustering = te.GetTimeElapsed(end, begin)/1000;

			/**Start timer**/
			begin = clock();

			/**Run Tabu Search, print out results**/
			ts.run(foutput, bestSoFar, noLettersCollectionString, numberOfLiterals, levelCounter, mapLevelClusters, vectorStringNumbers, initialClusterCollection, clusterCollection, mapLiteralValues, mapClauseValues, numberOfXLiterals, numberOfClauses, multiMapLiteralClause, begin);
			
			/**Stop timer**/
			end = clock();

			timeElapsedTabuSearch = te.GetTimeElapsed(end, begin)/1000;

			std::cout << "Time elapsed Multilevel: " << timeElapsedMultilevelClustering << " seconds" << std::endl;
			std::cout << "Time elapsed Tabu Search: " << timeElapsedTabuSearch << " seconds" << std::endl;
			std::cout << "Total time elapsed: " << timeElapsedMultilevelClustering + timeElapsedTabuSearch << " seconds" << std::endl << std::endl;
			foutput << "\nTotal time elapsed: " << timeElapsedMultilevelClustering + timeElapsedTabuSearch << " seconds";
			foutput.close();

			//Finished!
			Beep(800, 2000);
			system("pause");
		}
		
		else
		{			
			std::cout << "Incorrect file format. Input must be a text file. Exiting....." << std::endl << std::endl;
			system("pause");
		}
	}
	
	else
	{	
		std::cout << "No file input. Exiting....." << std::endl << std::endl;
		system("pause");
	}

	return 0;
}