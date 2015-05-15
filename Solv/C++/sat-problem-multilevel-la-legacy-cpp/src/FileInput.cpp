#include "FileInput.h"

FileInput::FileInput(void)
{
}

std::string FileInput::readFile(const std::string filePath)
{
	std::string string = "";
	std::string collectionString = "";
	std::ifstream input;
	input.open(filePath);

	std::cout << "Reading file....." << std::endl;

	while(!input.eof()) //To get you all the lines
    {
	    std::getline(input, string); //Get string
		collectionString += string;	  //Saves the string in collectionString
    }
	
	input.close();

	return collectionString;
}

std::string FileInput::removeLetters(std::string &collectionString, std::string &numberOfXLiterals, std::string &numberOfClauses) 
{
	std::cout << "Removing letters....." << std::endl;

	char * collectionChar;
	collectionChar = new char[collectionString.length() + 1];
	strcpy(collectionChar, collectionString.c_str());

	//Char -> string
	std::string noCharsCollectionString = std::string(collectionChar);		

	//Remove alpha chars
	noCharsCollectionString.erase(std::remove_if(noCharsCollectionString.begin(), noCharsCollectionString.end(), std::isalpha), noCharsCollectionString.end());

	int counter = 1;
	int k = 0;

	while(noCharsCollectionString[0] == ' ' || noCharsCollectionString[0] == '?' || noCharsCollectionString[0] == '=')
		noCharsCollectionString.erase(noCharsCollectionString.begin());

	//Get the number of X unique literals and clauses
	for(k; k < noCharsCollectionString.length(); k++)
	{
		if(noCharsCollectionString[k] == '#')
		{
			while(noCharsCollectionString[k - counter] != ' ')
			{
				numberOfClauses += noCharsCollectionString[k - counter];
				counter++;

				if(k - counter < 0)
					break;
			}
			break;
		}		
	}
	counter++;

	for(k; k < noCharsCollectionString.length(); k++)
	{
		while(noCharsCollectionString[k - counter] != ' ')
		{
			numberOfXLiterals += noCharsCollectionString[k - counter];
			counter++;

			if(k - counter < 0)
			{				
				counter = 1;
				break;
			}
		}
		break;
	}

	//Remove the stuff infront
	for(k; k < noCharsCollectionString.length(); k++)
	{
		while(k - counter != 0)
		{
			noCharsCollectionString.erase(noCharsCollectionString.begin() + k - counter);
			counter++;

			if(k - counter < 0)
			{				
				counter = 1;
				break;
			}
		}
		noCharsCollectionString.erase(noCharsCollectionString.begin());
		break;
	}
	counter = 1;

	//Reverse the clauses string value to get correct amount
	std::reverse(numberOfClauses.begin(), numberOfClauses.end());

	//Reverse the literals string value to get correct amount
	std::reverse(numberOfXLiterals.begin(), numberOfXLiterals.end());

	//Remove the first elements
	for(int i = 0; i < noCharsCollectionString.length(); i++)
	{
		if(noCharsCollectionString[i] == '#')			
			break;
		else
			noCharsCollectionString[i] = ' ';
	}

	delete[] collectionChar;
	
	return noCharsCollectionString;
}


FileInput::~FileInput(void)
{
}
