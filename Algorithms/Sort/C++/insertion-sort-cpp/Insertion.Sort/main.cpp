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
		while (numbers[i] < numbers[i - 1]) {
			int temp = numbers[i - 1];
			numbers[i - 1] = numbers[i];
			numbers[i] = temp;
			i--;
		}
	}	
	print(numbers);
	system("pause");
	return 0;
}