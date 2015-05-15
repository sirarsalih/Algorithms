#include "TravelingSalespersonProblem.h"

/*
**********************************************************************************************************************************
**********************************************************************************************************************************
Solving the Traveling Salesperson Problem, with 7 given cities, 10 random paths and 1 population. 1 Generation pr. iteration.
**********************************************************************************************************************************
**********************************************************************************************************************************
*/

int main(int argc, char * args[])
{
	TravelingSalespersonProblem * tsp = new TravelingSalespersonProblem(argc, args);

	tsp->solve();

	delete tsp;

	return 0;
}