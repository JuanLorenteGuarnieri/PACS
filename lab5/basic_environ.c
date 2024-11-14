////////////////////////////////////////////////////////////////////
//File: basic_environ.c
//
//Description: base file for environment exercises with openCL
//
// 
////////////////////////////////////////////////////////////////////

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>

#ifdef __APPLE__
  #include <OpenCL/opencl.h>
#else
  #include <CL/cl.h>
#endif

#include "cl_lib.cpp"
  

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
  cl_int err;
  

  Cl_runtime runtime;

  std::string s_pow_of_two = "__kernel void pow2("
        "__global float *in,"
        "__global float *out,"
        "const unsigned int count){"

        "int i = get_global_id(0);"

        "if(i < count){"
        "  out[i] = in[i] * in[i];"
        "}"
      "}";

  std::string s_printMatrix = "__kernel void printMatrix("
        "__global float *in,"
        "__global float *out,"
        "const unsigned int rows,"
        "const unsigned int cols){"

        "int i = get_global_id(0);"
        "int j = get_global_id(1);"

        "if(i < rows && j < cols){"
        "  printf(\"%i, %i\", i, j);"
        "}"
      "}";


  Cl_function f_pow_of_two = runtime.createFunction("pow2", s_pow_of_two);
  Cl_function f_printMatrix = runtime.createFunction("printMatrix", s_printMatrix);

  const size_t count = 3;
  std::vector<float> in(count*count);

  for (int i = 0; i < count*count; i++){
    in[i] = i;
  }

  auto out = runtime.runMatrixFunction(f_printMatrix, in, count, count);

  return 0;
}

