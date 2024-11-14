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

  std::string s_flipMatrix = "__kernel void flipMatrix("
        "__global float *in,"
        "__global float *out,"
        "const unsigned int rows,"
        "const unsigned int cols){"

        "int i = get_global_id(0);"
        "int j = get_global_id(1);"

        // Flip on X axis
        "if(i < rows && j < cols){"
        "  out[i*cols + cols-1-j] = in[i*cols + j];"
        "}"
      "}";


  Cl_function f_pow_of_two = runtime.createFunction("pow2", s_pow_of_two);
  Cl_function f_flipMatrix = runtime.createFunction("flipMatrix", s_flipMatrix);

  const size_t count = 3;
  std::vector<float> in(count*count);

  for (int i = 0; i < count*count; i++){
    in[i] = i+1;
  }

  std::vector<std::vector<float>> matrix = {
    {1.0f, 2.0f, 3.0f},
    {4.0f, 9.0f, 5.0f},
    {6.0f, 8.0f, 0.0f}
  };

  std::vector<std::vector<float>> kernel = {
    {0.0625f, 0.125f, 0.0625f},
    {0.125f, 0.25f, 0.125f},
    {0.0625f, 0.125f, 0.0625f}
  };

  auto out = runtime.convolution(matrix, kernel);
  

  for (int i = 0; i < count*count; i++){
    printf("%f ", in[i]);
  
    if (i % count == count-1){
      printf("\n");
    }
  }
  printf("\n");

  for (int i = 0; i < count*count; i++){
    printf("%f ", out[i]);
  
    if (i % count == count-1){
      printf("\n");
    }
  }


  return 0;
}

