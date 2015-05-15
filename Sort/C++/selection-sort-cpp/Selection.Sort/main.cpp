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
	for (int i = 1; i < (sizeof(numbers) / sizeof(numbers[0])); i++) {
		int current = numbers[i - 1];
		int min = numbers[i];
		int minIndex = i;
		for (int j = i + 1; j < (sizeof(numbers) / sizeof(numbers[0])); j++) {			
			if (numbers[j] < min) {
				min = numbers[j];
				minIndex = j;
			}
		} if (min < current) {
			numbers[i - 1] = min;
			numbers[minIndex] = current;
		}		
	}
	print(numbers);
	system("pause");
	return 0;
}