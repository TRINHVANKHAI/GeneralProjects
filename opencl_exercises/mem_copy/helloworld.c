#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <CL/cl.h>
#define FSLCL_ERROR -1
#define FSLCL_SUCCESS CL_SUCCESS

cl_mem GPU_buffer_input = NULL;
cl_mem GPU_buffer_output = NULL;
cl_kernel buffer_kernel = NULL;
size_t buffer_size = 0;

#define IMAGE_WIDTH 1536
#define IMAGE_HEIGHT 1024
#define GLOBAL_WIDTH 128
#define GLOBAL_HEIGHT 128
int buffer_width = IMAGE_WIDTH;
int buffer_height = IMAGE_HEIGHT;
int block_width =  IMAGE_WIDTH/GLOBAL_WIDTH;
int block_height = IMAGE_HEIGHT/GLOBAL_HEIGHT;

uint16_t input_array[IMAGE_HEIGHT*IMAGE_WIDTH];

uint16_t output_array[GLOBAL_HEIGHT*GLOBAL_WIDTH];

struct fsl_kernel_src_str
{
    char *src;
    long size;
};

typedef struct fsl_kernel_src_str fsl_kernel_src;

cl_int FSLCL_LoadKernelSource (char *filename, fsl_kernel_src *kernel)
{
    FILE *fp = NULL;
    fp = fopen (filename, "rb");
    if (fp == NULL)
    {
        printf ("\nFailed to open: %s\n", filename);
        return FSLCL_ERROR;
    }
    
    fseek (fp, 0, SEEK_END);
    kernel->size = ftell (fp);
    rewind (fp);
    kernel->src = (char *) malloc (sizeof (char) * kernel->size);
    if (! kernel->src)
    {
        printf ("\nError Allocating memory to load CL program");
        return FSLCL_ERROR;
    }
    
    fread (kernel->src, 1, kernel->size, fp);
    kernel->src[kernel->size] = '\0';
    fclose (fp);
    return FSLCL_SUCCESS;
}


int main (int argc, char **argv)
{
    int dimension = 2;
    size_t global[2] = {GLOBAL_WIDTH, GLOBAL_HEIGHT};
    size_t local[2] = {8, 8};
    cl_int ret;
    cl_int platforms;

    int row, col;


    for(row=0;row<IMAGE_HEIGHT;row++) {
        for(col=0;col<IMAGE_WIDTH;col++) {
            input_array[row*IMAGE_WIDTH+col]=col%12;
        }
    }
    memset(output_array, 0, sizeof(output_array));
    //Get an OpenCL platform
    cl_platform_id clPlatform;
    ret = clGetPlatformIDs(1, &clPlatform, NULL);
    assert (ret == CL_SUCCESS);
    
    
    // Get a GPU device
    cl_device_id cdDevice;
    ret = clGetDeviceIDs(clPlatform, CL_DEVICE_TYPE_GPU, 1, &cdDevice, NULL);
    assert (ret == CL_SUCCESS);
    
    
    char cBuffer[1024];
    clGetDeviceInfo(cdDevice, CL_DEVICE_NAME, sizeof(cBuffer), &cBuffer, NULL);
    printf("CL_DEVICE_NAME: %s\n", cBuffer);

    clGetDeviceInfo(cdDevice, CL_DRIVER_VERSION, sizeof(cBuffer), &cBuffer, NULL);
    printf("CL_DRIVER_VERSION: %s\n\n", cBuffer);



    // Create a context to run OpenCL enabled GPU
    cl_context GPUContext = clCreateContextFromType(0, CL_DEVICE_TYPE_GPU, NULL, NULL, NULL);
     
    // Create a command-queue on the GPU device
    cl_command_queue cqCommandQueue = clCreateCommandQueue(GPUContext, cdDevice, 0, NULL);
    

    printf("Load kernel source\n");
    fsl_kernel_src app_kernel;
    ret = FSLCL_LoadKernelSource ((char *)"helloworld.cl", &app_kernel);
    printf("Compile program\n");
    // Submit the source code of the kernel to OpenCL
    cl_program OpenCLProgram = clCreateProgramWithSource (GPUContext, 1, (const char **)&app_kernel.src, 0, &ret);
    if (ret == CL_SUCCESS) {
    // and compile it (after this we could extract the compiled version)
        ret = clBuildProgram(OpenCLProgram, 0, NULL, NULL, NULL, NULL);
        if (ret < 0)
        {
            printf ("Failed\n");
            printf ("\nReturn: %d\n", ret);
            clGetProgramBuildInfo(OpenCLProgram, cdDevice, CL_PROGRAM_BUILD_LOG, app_kernel.size, app_kernel.src, NULL);
            printf ("\n%s", app_kernel.src);
        }
    }
    
    assert(ret == CL_SUCCESS);

    GPU_buffer_input = clCreateBuffer (GPUContext, CL_MEM_READ_ONLY, sizeof(input_array), NULL, &ret);  
    assert (ret == CL_SUCCESS);
    GPU_buffer_output = clCreateBuffer (GPUContext, CL_MEM_WRITE_ONLY, sizeof(output_array), NULL, &ret);
    assert (ret == CL_SUCCESS);
    // get a handle and map parameters for the kernel
    cl_kernel kernel = clCreateKernel(OpenCLProgram, "helloworld", &ret);
    assert (ret == CL_SUCCESS);
    clSetKernelArg (kernel, 0, sizeof(cl_mem), &GPU_buffer_input);
    clSetKernelArg (kernel, 1, sizeof(cl_mem), &GPU_buffer_output);
    clSetKernelArg (kernel, 2, sizeof(int), &buffer_width);
    clSetKernelArg (kernel, 3, sizeof(int), &buffer_height);
    clSetKernelArg (kernel, 4, sizeof(int), &block_width);
    clSetKernelArg (kernel, 5, sizeof(int), &block_height);

    ret = clEnqueueWriteBuffer (cqCommandQueue, GPU_buffer_input, CL_TRUE, 0, sizeof(input_array), input_array, 0, NULL, NULL);
    assert (ret == CL_SUCCESS);
    
    
    ret = clEnqueueNDRangeKernel(cqCommandQueue, kernel, 2, NULL, global, local, 0, NULL, NULL);
    assert (ret == CL_SUCCESS);
    
    
    ret = clEnqueueReadBuffer(cqCommandQueue, GPU_buffer_output, CL_TRUE, 0, sizeof(output_array), output_array, 0, NULL, NULL);
    assert (ret == CL_SUCCESS);
    
    
    
    printf ("\nResult:\n");
    for (row = 0; row < (GLOBAL_WIDTH*GLOBAL_HEIGHT); row++)
        printf("%d, ", output_array[row]);
    printf("\n");
        
    #if 0
    for (row = 0; row < buffer_height; row++)
    {
        for (col = 0; col < buffer_width; col++)
        {
            printf ("%d -- %d, ", input_array[row*IMAGE_WIDTH+col], output_array[row*IMAGE_WIDTH+col]);
        }
        printf("\n");
    }
    
    ret = memcmp(input_array, output_array, sizeof(input_array));
    if(ret != 0) {
        printf("Compare not match! \n");
    } else {
        printf("Compare ok! \n");
    }
    #endif
    
    
    clFlush( cqCommandQueue);
    clFinish(cqCommandQueue);
    clReleaseMemObject (GPU_buffer_input);
    clReleaseMemObject (GPU_buffer_output);
    clReleaseContext(GPUContext);
    clReleaseKernel(kernel);
    clReleaseProgram(OpenCLProgram);
    clReleaseCommandQueue(cqCommandQueue);
    return 0;
}


