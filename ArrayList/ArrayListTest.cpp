#include <iostream>
#include "Arraylist.h"

int main()
{
	ArrayList<int> arr;
	arr.add(1);
	arr.add(2);
	arr.remove(0);

	int i = arr.get(3);

	const ArrayList<int> constArr(arr);
	size_t len = constArr.size();

	std::cout << arr[0] << std::endl;
	getchar();
	return 0;
}