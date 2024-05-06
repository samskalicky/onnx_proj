#include <vector>
#include <string>
#include <iostream>

class Tensor {
public:
  Tensor(void* ptr_, int dtype_, int ndim_) : ptr(ptr_), datatype(dtype_) {
    dims.resize(ndim_);
  }

  void setDim(int idx, int val) {
    dims[idx] = val;
  }

  const std::vector<int> getDims() {
    return dims;
  }

  void* dptr() {
    return ptr;
  }

  int dtype() {
    return datatype;
  }

private:
  void* ptr;
  std::vector<int> dims;
  int datatype;
};

class CustomKernel {
public:
  CustomKernel(const char* name_) : name(name_) {}

  CustomKernel& setCompute(void(*func)()) {
    compute = (void*)func;
    return *this;
  }

  std::string& getName() {
    return name;
  }

  void* getCompute() {
    return compute;
  }

private:
  std::string name;
  void* compute;
};

template <class T>
class Registry {
public:
  // singleton getter
  static Registry* get() {
    static Registry inst;
    return &inst;
  }

  T& add(const char* name) {
    T* entry = new T(name);
    entries.push_back(entry);
    return *entry;
  }

  int size() {
    return entries.size();
  }

  T& getEntry(int idx) {
    return *(entries.at(idx));
  }

private:
  std::vector<T*> entries;
};


#define STR_CONCAT_(__a, __b) __a##__b
#define STR_CONCAT(__a, __b)  STR_CONCAT_(__a, __b)

#define STRINGIFY(x) #x
#define TOSTRING(x)  STRINGIFY(x)

#define REGISTER_DEF_(Name)  CustomKernel My##_CustomKernel##_##__COUNTER__

#define REGISTER_KERNEL(Name)                              \
  STR_CONCAT(REGISTER_DEF_(Name), __COUNTER__) = \
      Registry<CustomKernel>::get()->add(TOSTRING(Name))

extern "C" {

int regSize() {
  return Registry<CustomKernel>::get()->size();
}

void regGet(int idx, const char** name, void** compute) {
  CustomKernel& kernel = Registry<CustomKernel>::get()->getEntry(idx);
  *name = kernel.getName().c_str();
  *compute = kernel.getCompute();
}

} // end extern "C"