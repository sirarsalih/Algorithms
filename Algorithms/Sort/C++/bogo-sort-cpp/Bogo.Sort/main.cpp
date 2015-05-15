#include <iostream>
#include <iterator>
#include <algorithm>

void print(int(&numbers)[10])
{
	std::copy(numbers,
		numbers + sizeof(numbers) / sizeof(numbers[0]),
		std::ostream_iterator<short>(std::cout, " "));
	std::cout << std::endl;
}

int main()
{
	int numbers[10] = { 4, 3, 2, 4, 1, 8, 2, 3, 1, 2 };
	while (true) {
		bool isSorted = true;
		for (int i = 0; i < sizeof(numbers) / sizeof(numbers[0]) - 1; i++) {
			if (numbers[i] > numbers[i + 1]) {
				isSorted = false;
				break;
			}
		} if (isSorted) {
			break;
		}
		std::random_shuffle(&numbers[0], &numbers[10]);
	}
	print(numbers);
	system("pause");
	return 0;
}