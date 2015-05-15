#include <stdio.h>
#include <iostream>

int main()
{
	const int NUM_OF_ELEMENTS = 101;
	const int N = rand() % NUM_OF_ELEMENTS;
	int head = 0;
	int tail = NUM_OF_ELEMENTS;
	int middle = head + tail / 2;

	while (N != middle && head < tail) {
		if (N < middle) {
			tail = middle - 1;
		} else {
			head = middle + 1;
		}
		middle = (head + tail) / 2;
	} if (N == middle) {
		printf("Found number %d in %d\n", N, NUM_OF_ELEMENTS - 1);
		system("pause");
	}
	return 0;
}