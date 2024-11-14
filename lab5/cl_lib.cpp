
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <stdexcept>

#ifdef __APPLE__
    #include <OpenCL/opencl.h>
#else
    #include <CL/cl.h>
#endif


void cl_error(cl_int code, std::string msg)
{
	if (code != CL_SUCCESS){
		throw std::runtime_error(msg);
	}
}
  


class Cl_function
{
    cl_kernel kernel;

public:

    Cl_function (cl_context context, std::string name, std::string source_code)
    {
        cl_int err;
        auto fileSize = source_code.size();

        cl_program Program = clCreateProgramWithSource(context, 1, (const char **) &source_code, &fileSize, &err);
        cl_error(err, "Failed to create a program"+name+"with source\n");

        err = clBuildProgram(Program, 0, NULL, NULL, NULL, NULL);
        cl_error (err, "Failed to build program "+name+"\n");
        kernel = clCreateKernel(Program, name.c_str(), &err);
    }

    cl_kernel get()
    {
        return kernel;
    }
};


class Cl_runtime
{
    cl_context context;
    cl_command_queue queue;

public:

    ~Cl_runtime()
    {
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
    }

    Cl_runtime()
    {
        int err;                            	// error code returned from api calls
        size_t t_buf = 50;			// size of str_buffer
        char str_buffer[t_buf];		// auxiliary buffer	
        size_t e_buf;				// effective size of str_buffer in use
                
        size_t global_size;                      	// global domain size for our calculation
        size_t local_size;                       	// local domain size for our calculation

        const cl_uint num_platforms_ids = 10;				// max of allocatable platforms
        cl_platform_id platforms_ids[num_platforms_ids];		// array of platforms
        cl_uint n_platforms;						// effective number of platforms in use
        const cl_uint num_devices_ids = 10;				// max of allocatable devices
        cl_device_id devices_ids[num_platforms_ids][num_devices_ids];	// array of devices
        cl_uint n_devices[num_platforms_ids];				// effective number of devices in use for each platform
            
        cl_device_id device_id;             				// compute device id 

        // 1. Scan the available platforms:
        err = clGetPlatformIDs (num_platforms_ids, platforms_ids, &n_platforms);
        cl_error(err, "Error: Failed to Scan for Platforms IDs");
        //printf("Number of available platforms: %d\n\n", n_platforms);

        for (int i = 0; i < n_platforms; i++ )
        {
            err= clGetPlatformInfo(platforms_ids[i], CL_PLATFORM_NAME, sizeof(str_buffer), str_buffer, NULL);
            cl_error (err, "Error: Failed to get info of the platform\n");
            //printf( "\t[%d]-Platform Name: %s\n", i, str_buffer);

            // Task: Print more information about the platform
            clGetPlatformInfo(platforms_ids[i], CL_PLATFORM_VENDOR, sizeof(str_buffer), str_buffer, NULL);
            //printf("\t\tPlatform Vendor: %s\n", str_buffer);
            
            clGetPlatformInfo(platforms_ids[i], CL_PLATFORM_VERSION, sizeof(str_buffer), str_buffer, NULL);
            //printf("\t\tPlatform Version: %s\n", str_buffer);

            clGetPlatformInfo(platforms_ids[i], CL_PLATFORM_PROFILE, sizeof(str_buffer), str_buffer, NULL);
            //printf("\t\tPlatform Profile: %s\n", str_buffer);

            clGetPlatformInfo(platforms_ids[i], CL_PLATFORM_HOST_TIMER_RESOLUTION, sizeof(e_buf), &e_buf, NULL);
            //printf("\t\tHost Timer Resolution: %zu\n", e_buf);
        }
        //printf("\n");
        // ***Task***: print on the screen the name, host_timer_resolution, vendor, versionm, ...
            
        // 2. Scan for devices in each platform
        for (int i = 0; i < n_platforms; i++ )
        {
            err = clGetDeviceIDs(platforms_ids[i], CL_DEVICE_TYPE_ALL, num_devices_ids, devices_ids[i], &(n_devices[i]));
            cl_error(err, "Error: Failed to Scan for Devices IDs");
            //printf("\t[%d]-Platform. Number of available devices: %d\n", i, n_devices[i]);

            for(int j = 0; j < n_devices[i]; j++)
            {
                err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_NAME, sizeof(str_buffer), str_buffer, NULL);
                cl_error(err, "clGetDeviceInfo: Getting device name");
                //printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_NAME: %s\n", i, j,str_buffer);

                cl_uint max_compute_units_available;
                err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(max_compute_units_available), &max_compute_units_available, NULL);
                cl_error(err, "clGetDeviceInfo: Getting device max compute units available");
                //printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_MAX_COMPUTE_UNITS: %d\n\n", i, j, max_compute_units_available);

                // Print additional device information
                size_t global_mem_size;
                err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(global_mem_size), &global_mem_size, NULL);
                cl_error(err, "clGetDeviceInfo: Getting device global memory size");
                //printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_GLOBAL_MEM_SIZE: %zu bytes\n", i, j, global_mem_size);

                cl_ulong local_mem_size;
                err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(local_mem_size), &local_mem_size, NULL);
                cl_error(err, "clGetDeviceInfo: Getting device local memory size");
                //printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_LOCAL_MEM_SIZE: %lu bytes\n", i, j, local_mem_size);

                cl_ulong max_mem_alloc_size;
                err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(max_mem_alloc_size), &max_mem_alloc_size, NULL);
                cl_error(err, "clGetDeviceInfo: Getting device max memory allocation size");
                //printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_MAX_MEM_ALLOC_SIZE: %lu bytes\n", i, j, max_mem_alloc_size);

                size_t max_work_group_size;
                err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(max_work_group_size), &max_work_group_size, NULL);
                cl_error(err, "clGetDeviceInfo: Getting device max work group size");
                //printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_MAX_WORK_GROUP_SIZE: %zu\n\n", i, j, max_work_group_size);
            }
        }	
        // ***Task***: print on the screen the cache size, global mem size, local memsize, max work group size, profiling timer resolution and ... of each device


        // 3. Create a context, with a device
        cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platforms_ids[0], 0}; // Using the first platform
        context = clCreateContext(properties, n_devices[0], devices_ids[0], NULL, NULL, &err);
        cl_error(err, "Failed to create a compute context\n");

        // 4. Create a command queue
        cl_command_queue_properties proprt[] = { CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0 };
        queue = clCreateCommandQueueWithProperties(context, devices_ids[0][0], proprt, &err); // Using the first device
        cl_error(err, "Failed to create a command queue\n");
    }

    Cl_function createFunction(std::string name, std::string sourceCode) const
    {
        return Cl_function(context, name, sourceCode);
    }

    std::vector<float> runBufferFunction(Cl_function &f, std::vector<float> &in)
    {
        cl_int err;
        size_t count = in.size();
        std::vector<float> out(count);

        cl_mem input = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * count, NULL, &err);
        cl_error(err, "Failed to create buffer\n");
        cl_mem output = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * count, NULL, &err);
        cl_error(err, "Failed to create buffer\n");

        err = clSetKernelArg(f.get(), 0, sizeof(cl_mem), &input);
        cl_error(err, "Failed to set kernel arg 0\n");
        err = clSetKernelArg(f.get(), 1, sizeof(cl_mem), &output);
        cl_error(err, "Failed to set kernel arg 1\n");
        err = clSetKernelArg(f.get(), 2, sizeof(unsigned int), &count);
        cl_error(err, "Failed to set kernel arg 2\n");

        err = clEnqueueWriteBuffer(queue, input, CL_TRUE, 0, sizeof(float) * count, in.data(), 0, NULL, NULL);
        cl_error(err, "Failed to write buffer\n");
        err = clEnqueueNDRangeKernel(queue, f.get(), 1, NULL, &count, NULL, 0, NULL, NULL);
        cl_error(err, "Failed to enqueue kernel\n");
        err = clEnqueueReadBuffer(queue, output, CL_TRUE, 0, sizeof(float) * count, out.data(), 0, NULL, NULL);
        cl_error(err, "Failed to read buffer\n");

        // Wait for the command queue to finish
        clFinish(queue);
        clReleaseMemObject(input);
        clReleaseMemObject(output);

        return out;
    }


    std::vector<float> runMatrixFunction(Cl_function &f, std::vector<float> &in, size_t rows, size_t cols)
    {
        cl_int err;
        size_t count = rows*cols;
        std::vector<float> out(count);

        cl_mem input = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * count, NULL, &err);
        cl_error(err, "Failed to create buffer\n");
        cl_mem output = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * count, NULL, &err);
        cl_error(err, "Failed to create buffer\n");

        err = clSetKernelArg(f.get(), 0, sizeof(cl_mem), &input);
        cl_error(err, "Failed to set kernel arg 0\n");
        err = clSetKernelArg(f.get(), 1, sizeof(cl_mem), &output);
        cl_error(err, "Failed to set kernel arg 1\n");
        err = clSetKernelArg(f.get(), 2, sizeof(unsigned int), &rows);
        cl_error(err, "Failed to set kernel arg 2\n");
        err = clSetKernelArg(f.get(), 3, sizeof(unsigned int), &cols);
        cl_error(err, "Failed to set kernel arg 3\n");

        err = clEnqueueWriteBuffer(queue, input, CL_TRUE, 0, sizeof(float) * count, in.data(), 0, NULL, NULL);
        cl_error(err, "Failed to write buffer\n");
        
        // Launch 2d kernel
        size_t global_work_size[2] = {rows, cols};
        err = clEnqueueNDRangeKernel(queue, f.get(), 2, NULL, global_work_size, NULL, 0, NULL, NULL);
        cl_error(err, "Failed to enqueue kernel\n");

        err = clEnqueueReadBuffer(queue, output, CL_TRUE, 0, sizeof(float) * count, out.data(), 0, NULL, NULL);
        cl_error(err, "Failed to read buffer\n");

        // Wait for the command queue to finish
        clFinish(queue);
        clReleaseMemObject(input);
        clReleaseMemObject(output);

        return out;
    }
};

