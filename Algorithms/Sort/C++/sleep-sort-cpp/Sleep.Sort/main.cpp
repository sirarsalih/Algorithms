#include <stdio.h>
#include <thread>
#include <vector>

void sleep(int seconds)
{
	clock_t endwait;
	endwait = clock() + seconds * CLOCKS_PER_SEC;
	while (clock() < endwait) {}
	printf("%d ", seconds);
}

int main()
{
	int numbers[10] = { 4, 3, 2, 4, 1, 8, 2, 3, 1, 2 };
	std::vector<std::thread*> threads;

	for (int i = 0; i < sizeof(numbers)/sizeof(numbers[0]); i++) {
		std::thread* t = new std::thread(sleep, numbers[i]);
		threads.push_back(t);
	}

	for (std::vector<std::thread*>::iterator t = threads.begin(); t != threads.end(); t++) {
		(*t)->join();
		delete (*t);
	}
	printf("\n");
	system("pause");
	return 0;
}