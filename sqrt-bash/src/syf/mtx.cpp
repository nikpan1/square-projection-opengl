#include "mtx.h"
#include <iostream>

mtx::mtx(uint16_t size) {
    container = (float**)malloc(sizeof(float) * size * size);
    mtx_size = size;
}

mtx::~mtx() {
    // poof! destroy
}

float* mtx::val(uint16_t row, uint16_t column) {
    return &container[row * mtx_size + column + 1];
}

void mtx::fillRow(uint16_t row, float values[]) {
    for (auto i = 0; i < mtx_size; i++) {
        container[i * mtx_size + row - 1] = values[i];
    }
}


std::ostream& operator << (std::ostream& out, const mtx& c) {
    for (uint16_t i = 0; i < c.mtx_size; i ++) {
        for (uint16_t j = 0; j < c.mtx_size; j++) {
            out << c.container[c.mtx_size * j + i] <<" ";
        }
        out << "\n";
    }

    return out;
}
