#ifndef TSETLIN
#define TSETLIN

class Tsetlin{

	public:
		Tsetlin();
		Tsetlin(int n);
		void reward();
		void penalize();
		int makeDecision();
		int getN();

	private:
		int n;
		int state;
		int stateArray[2];
};

#endif