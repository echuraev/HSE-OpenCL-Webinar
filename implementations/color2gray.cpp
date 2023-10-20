#include "color2gray.h"
#include <common_functions.h>

#include <chrono>

#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

cv::Mat readImage(std::string fileName)
{
    cv::Mat mat = cv::imread("../images/" + fileName, cv::IMREAD_COLOR);
    cv::cvtColor(mat, mat, cv::COLOR_BGR2BGRA);
    return mat;
}

ExecTime color2gray(std::string device_type)
{
    cl_device_id device_id;
    cl_context context;
    cl_command_queue command_queue;
    int err;
    prepareOpenCLDevice(device_id, context, command_queue, device_type);

    std::string kernelSource = readKernel("color2gray.cl");
    const char* str = kernelSource.c_str();

    // Read image and create cl_mem objects
    cv::Mat image = readImage("hse_color_logo.png");

    cl_image_format format;  // structure to define image format
    format.image_channel_data_type = CL_UNORM_INT8;
    format.image_channel_order = CL_RGBA;

    // init image description
    cl_image_desc desc = {0};  // structure to define image description
    desc.image_type = CL_MEM_OBJECT_IMAGE2D;
    desc.image_width = image.cols;
    desc.image_height = image.rows;

    // Allocate input_img image initialized by pixels loaded from .BMP file
    cl_mem input_img =
            clCreateImage(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, &format, &desc, image.data, &err);
    assert(err == CL_SUCCESS);

    // Allocate non-initialized output buffer
    cl_mem output_img = clCreateImage(context, CL_MEM_WRITE_ONLY, &format, &desc, NULL, &err);
    assert(err == CL_SUCCESS);

    // Create program
    cl_program program = clCreateProgramWithSource(context, 1, &str, NULL, &err);
    assert(err == CL_SUCCESS);

    // Build program
    err = clBuildProgramWrapper(program, 1, &device_id);
    assert(err == CL_SUCCESS);

    // Create kernel
    cl_kernel kernel = clCreateKernel(program, "color2gray", &err);
    assert(err == CL_SUCCESS);

    // Set kernel arguments
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&input_img);
    assert(err == CL_SUCCESS);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&output_img);
    assert(err == CL_SUCCESS);

    // Run kernel
    auto cpuStart = std::chrono::high_resolution_clock::now();
    size_t global_work_size[2] = {static_cast<size_t>(image.cols), static_cast<size_t>(image.rows)};  // Define global size of execution
    cl_event event;
    err = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global_work_size, NULL, 0, NULL, &event);
    assert(err == CL_SUCCESS);
    err = clWaitForEvents(1, &event);
    assert(err == CL_SUCCESS);
    err = clFinish(command_queue);
    assert(err == CL_SUCCESS);
    auto cpuEnd = std::chrono::high_resolution_clock::now();

    // Read result image
    size_t origin[3] = {0, 0, 0};
    size_t region[3] = {static_cast<size_t>(image.cols), static_cast<size_t>(image.rows), 1};
    size_t rowPitch = 0;
    size_t slicePitch = 0;
    uchar* p = new uchar[image.cols * image.rows * 4];
    err = clEnqueueReadImage(command_queue, output_img, CL_TRUE, origin, region, rowPitch, slicePitch, p, 0, NULL,
                             NULL);
    assert(err == CL_SUCCESS);
    cv::Mat out(image.size(), CV_8UC4, p);
    cv::imwrite("out.png", out);
    delete[] p;

    // Measure execution time
    cl_ulong time_start;
    cl_ulong time_end;

    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);

    double kernelTimeMS = (time_end - time_start) * 1e-6;  // from ns to ms
    auto cpuTimeMS = std::chrono::duration_cast<std::chrono::nanoseconds>(cpuEnd - cpuStart).count() * 1e-6;

    // Release memory
    err = clReleaseEvent(event);
    assert(err == CL_SUCCESS);
    err = clReleaseMemObject(input_img);
    assert(err == CL_SUCCESS);
    err = clReleaseMemObject(output_img);
    assert(err == CL_SUCCESS);
    err = clReleaseKernel(kernel);
    assert(err == CL_SUCCESS);
    err = clReleaseCommandQueue(command_queue);
    assert(err == CL_SUCCESS);
    err = clReleaseProgram(program);
    assert(err == CL_SUCCESS);
    err = clReleaseContext(context);
    assert(err == CL_SUCCESS);
    err = clReleaseDevice(device_id);
    assert(err == CL_SUCCESS);

    return {cpuTimeMS, kernelTimeMS};
}
