__kernel void k_dotprod(
	__global__ float *v1,
	__global__ float *v2,
	__global__ unsigned int nElems,
	__global__ float results)
{
	idx = get_global_id(0);
	stride = get_global_size(0); // Total of work elements in the execution
	group_id = get_group_id(0);

	float accum = 0;
	for (unsigned int i = idx; i < nElems; i += stride) // Access with stride for better cache locality
	{
		accum += v1[i] * v2[i];
	}

	atomic_add(&results[group_id], accum);
}


// Exercise 4 in problems.pdf
// Total cycles: ceil(nElems / (8cus * 128cores * 2)) + 8cus (final reduction)
int main()
{
	std::vector<float> v1 = ...;
	std::vector<float> v2 = ...;

	auto kernel = ...; // Compile k_dotprod
	uint nElems = v1.size();
	uint nWorkGroups = ...; // Something to close to the CUs in the GPU or CPU cores

	cl_mem cl_v1 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * nElems, NULL, &err);
	cl_mem cl_v2 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * nElems, NULL, &err);
	cl_mem partials = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * nWorkGroups, NULL, &err);

	err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &cl_v1);
	err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &cl_v2);
	err = clSetKernelArg(kernel, 2, sizeof(unsigned int), &nElems);
	err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &partials);

	err = clEnqueueWriteBuffer(queue, cl_v1, CL_TRUE, 0, sizeof(float) * nElems, v1.data(), 0, NULL, NULL);
	err = clEnqueueWriteBuffer(queue, cl_v2, CL_TRUE, 0, sizeof(float) * nElems, v2.data(), 0, NULL, NULL);

	size_t global_work_size = (nElems / nWorkGroups) + 1;
	err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_work_size, NULL, 0, NULL, NULL);

	std::vector<float> results(nWorkGroups);
	err = clEnqueueReadBuffer(queue, partials, CL_TRUE, 0, sizeof(float) * nWorkGroups, results.data(), 0, NULL, NULL);

	clFinish(queue);
	// Release memory...

	/*************** Finish openCL code ****************/

	return std::accumulate(results.begin(), results.end(), 0.0f);

	/*float dotprod = 0;
	for (auto &r : results) dotprod += r;
	return dotprod;*/
}

