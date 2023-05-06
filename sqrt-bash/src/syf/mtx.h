#pragma once
#include <vector>
#include <iostream>

// mtx[row][columns]

class mtx {
public:
	float** container;
	uint16_t mtx_size;
	
public:
	mtx(uint16_t size);
	~mtx();
	float* val(uint16_t row, uint16_t column);		// val(wiersze, kolumny)
	void fillRow(uint16_t row, float values[]);

	friend std::ostream & operator << (std::ostream &out, const mtx &c);	
};
