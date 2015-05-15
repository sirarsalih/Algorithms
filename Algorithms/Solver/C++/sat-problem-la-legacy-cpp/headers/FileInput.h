#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>

class FileInput
{
public:
	FileInput(void);
	~FileInput(void);
	std::string readFile(const std::string filePath);
	std::string FileInput::removeLetters(std::string &collectionString, std::string &numberOfXLiterals, std::string &numberOfClauses); 
};

