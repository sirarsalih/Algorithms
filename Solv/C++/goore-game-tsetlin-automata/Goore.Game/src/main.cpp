#include "Tsetlin.h"
#include "Referee.h"

int main()
{
	Referee referee;
	std::vector<Tsetlin> tsetlinAutomata;
	
	Tsetlin tsetlinAutomatonOne(10);
	Tsetlin tsetlinAutomatonTwo(10);
	Tsetlin tsetlinAutomatonThree(10);
	
	tsetlinAutomata.push_back(tsetlinAutomatonOne);
	tsetlinAutomata.push_back(tsetlinAutomatonTwo);
	tsetlinAutomata.push_back(tsetlinAutomatonThree);

	referee.decide(tsetlinAutomata);

	system("pause");

	return 0;
}