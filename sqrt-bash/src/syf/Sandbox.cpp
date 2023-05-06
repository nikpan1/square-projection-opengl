#include "mtx.h"
#include <iostream>

int main() {
	mtx my_mtx(4);
	float values[] = {
		1, 2, 3, 4
	};

	my_mtx.fillRow(1, values);
	my_mtx.fillRow(2, values);
	std::cout << my_mtx << std::endl;
}