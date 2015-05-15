#include "Tsetlin.h"
#include <iostream>

Tsetlin::Tsetlin()
{	
}

Tsetlin::Tsetlin(int n)
{
	//n is the number of states per action
	this->n = n;

	stateArray[0] = this->n;
	stateArray[1] = this->n + 1;

	//Initial state selected randomly
	int randomIndex = 0;
	for(int i = 0; i < 2; i++)
		randomIndex = rand() % 2;
	
	this->state = stateArray[randomIndex];
}
       
void Tsetlin::reward()
{
	if (this->state <= this->n && this->state > 1)
		this->state -= 1;
	else if(this->state > this->n && this->state < 2 * this->n)
		this->state += 1;
}

void Tsetlin::penalize()
{
	if (this->state <= this->n)
		this->state += 1;
	else if (this->state > this->n)
		this->state -= 1;
}

int Tsetlin::makeDecision()
{         
	if (this->state <= this->n)
		return 1;
	else
		return 2;
} 

int Tsetlin::getN()
{
	return this->n;
}