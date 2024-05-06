#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>

#include <torch/torch.h>

#include "dlfcn.h"

std::unordered_map<std::string, void*> kernels;

std::vector<torch::Tensor> my_exec(std::vector<torch::Tensor> t_in, std::string op_type) {
  if(kernels.count(op_type) == 0)
    throw std::runtime_error("Unknown op_type: " + op_type);

  void(*func)() = (void(*)())kernels[op_type];
  func();

  return t_in;
}

TORCH_LIBRARY(my_ops, m) {
  m.def("my_exec", & my_exec);
}

extern "C" {

void load(char* path) {
  void* lib = dlopen(path, RTLD_LAZY | RTLD_LOCAL);
  int(*regSize)() = (int(*)())dlsym(lib, "regSize");

  void(*regGet)(int,char**,void**) = (void(*)(int,char**,void**))dlsym(lib, "regGet");

  int num_entries = regSize();

  for(int i=0; i<num_entries; i++) {
    char* name;
    void* compute;
    regGet(i, &name, &compute);
    kernels[name] = compute;
    std::cout << "reg " << i << ": " << name << std::endl;
  }
}

}