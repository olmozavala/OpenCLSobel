//
// Book:      OpenCL(R) Programming Guide
// Authors:   Aaftab Munshi, Benedict Gaster, Timothy Mattson, James Fung, Dan Ginsburg
// ISBN-10:   0-321-74964-2
// ISBN-13:   978-0-321-74964-2
// Publisher: Addison-Wesley Professional
// URLs:      http://safari.informit.com/9780132488006/
//            http://www.openclprogrammingguide.com
//


// vecadd.cpp
//
//    This is a simple example that demonstrates use OpenCL C++ Wrapper API.

// Enable OpenCL C++ exceptions
#define __CL_ENABLE_EXCEPTIONS


#include <CL/cl.hpp>
#include <ErrorCodes.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include "FreeImage.h"

#define BUFFER_SIZE 10

using namespace std;
int A[BUFFER_SIZE];
int B[BUFFER_SIZE];
int C[BUFFER_SIZE];

char* openFile(const char* fileName, int& m_size){
    ifstream progtxt (fileName, ios::in | ios::ate);
    if(!progtxt.is_open()){
        cerr<<"Failed to open file for reading: "<< fileName << endl;
        return NULL;
    }

    m_size = size_t(progtxt.tellg()) + 1;
    char* m_source = new char[m_size];
    progtxt.seekg( 0, std::ios::beg );
    progtxt.read( m_source, m_size-1 );
    progtxt.close();
    m_source[m_size-1] = '\0';

    return m_source;
}

char* loadImage(char* fileName, int& width, int& height){
    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(fileName,0);
    FIBITMAP* image = FreeImage_Load(format, fileName);

//    FIBITMAP* temp = image;
    image = FreeImage_ConvertTo32Bits(image);

    width = FreeImage_GetWidth(image);
    height = FreeImage_GetHeight(image);

    char *buffer = new char[width*height*4];

    return buffer;
}

int
main(void)
{
    cl_int err;

    // Initialize A, B, C
    for (int i = 0; i < BUFFER_SIZE; i++) {
        A[i] = i;
        B[i] = i * 2;
        C[i] = 0;
    }

    try {
        vector<cl::Platform> platformList;

        // Pick platform
        cl::Platform::get(&platformList);

        // Pick first platform
        cl_context_properties cprops[] = {
                        CL_CONTEXT_PLATFORM, 
                        (cl_context_properties)(platformList[0])(), 0};
        cl::Context context(CL_DEVICE_TYPE_GPU, cprops);

        // Query the set of devices attched to the context
        vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
        
        // Create and program from source
        int size=0;
        char* kernelSource = openFile("SobelOZ.cl",size);

        cl::Program::Sources sources(1, make_pair(kernelSource, size));
        cl::Program program(context, sources);

        // Build program
        program.build(devices);

        // Create buffer for A and copy host contents
        cl::Buffer aBuffer = cl::Buffer(
            context, 
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
            BUFFER_SIZE * sizeof(int), 
            (void *) &A[0]);

        // Create buffer for B and copy host contents
        cl::Buffer bBuffer = cl::Buffer(
            context, 
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
            BUFFER_SIZE * sizeof(int), 
            (void *) &B[0]);

        // Create buffer for that uses the host ptr C
        cl::Buffer cBuffer = cl::Buffer(
            context, 
            CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, 
            BUFFER_SIZE * sizeof(int), 
            (void *) &C[0]);

        // Create kernel object
        cl::Kernel kernel(program, "vadd");

        // Set kernel args
        kernel.setArg(0, aBuffer);
        kernel.setArg(1, bBuffer);
        kernel.setArg(2, cBuffer);

        //----------------- Running the Sobel Kernel
        int width =0;
        int height =0;
        const char* fileName = "test.jpg";
//        char* imgbuf = loadImage("test.jpg",width, height);

/*        FREE_IMAGE_FORMAT format = FreeImage_GetFileType(fileName,0);
        FIBITMAP* image = FreeImage_Load(format, fileName);

        FIBITMAP* temp = image;
        image = FreeImage_ConvertTo32Bits(image);
        FreeImage_Unload(temp);

        width = FreeImage_GetWidth(image);
        height = FreeImage_GetHeight(image);

        char *imgbuf = new char[width*height*4];
        memcpy(imgbuf, FreeImage_GetBits(image), width*height*4);
        FreeImage_Unload(image);
        
        //Defining image format 
        cl::ImageFormat clformat(CL_RGBA,CL_UNORM_INT8);

        cout << "Width:" << width << endl << "Height:" << height << endl;
        cl::Image2D inImg (context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                clformat, width, height,0, &imgbuf, &err);
*/

        //----------------- Running the Sobel Kernel

        // Create command queue
        cl::CommandQueue queue(context, devices[0]);
 
        // Do the work
        queue.enqueueNDRangeKernel(
            kernel, 
            cl::NullRange, 
            cl::NDRange(BUFFER_SIZE), 
            cl::NullRange);
 

        // Map cBuffer to host pointer. This enforces a sync with 
        // the host backing space, remember we choose GPU device.
        int * output = (int *) queue.enqueueMapBuffer(
            cBuffer,
            CL_TRUE, // block 
            CL_MAP_READ,
            0,
            BUFFER_SIZE * sizeof(int));

        for (int i = 0; i < BUFFER_SIZE; i++) {
            cout << C[i] << " ";
        }
        cout << endl;

        // Finally release our hold on accessing the memory
        err = queue.enqueueUnmapMemObject(
            cBuffer,
            (void *) output);
 
        // There is no need to perform a finish on the final unmap
        // or release any objects as this all happens implicitly with
        // the C++ Wrapper API.
    } 
    catch (cl::Error ex) {
         ErrorCodes obj;

         cerr
             << "ERROR: "
             << ex.what() << endl;
         cerr << "Specific: " << obj.print_cl_error(ex.err()) << endl;
         return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
