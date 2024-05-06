#include <iostream>

#include "myKernel.h"

void myMatMulCompute() {
  std::cout << "inside myMatMulCompute" << std::endl;
}

REGISTER_KERNEL(MatMul)
.setCompute(myMatMulCompute);