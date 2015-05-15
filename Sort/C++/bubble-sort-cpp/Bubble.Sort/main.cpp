#include <iostream>
#include <iterator>

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
	bool swap = true;
	while (swap) {
		swap = false;
		for (int i = 0; i < (sizeof(numbers) / sizeof(numbers[0])) - 1; i++) {
			if (numbers[i] > numbers[i + 1]) {
				int temp = numbers[i];
				numbers[i] = numbers[i + 1];
				numbers[i + 1] = temp;
				swap = true;
			}
		}
	}
	print(numbers);
	system("pause");
	return 0;
}