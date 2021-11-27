## General-purpose computing on GPU with OpenCL

Practice from webinar in HSE Master's Programme: Master of Computer Vision.

### Table of Contents

<!-- vim-markdown-toc GFM -->

* [Requirements](#requirements)
* [Building](#building)
* [Project structure](#project-structure)
* [Description of the programs](#description-of-the-programs)
    * [vector_add](#vector_add)
    * [color2gray](#color2gray)
* [Run the programs](#run-the-programs)
* [Performance](#performance)

<!-- vim-markdown-toc -->

### Requirements
- compiler with C++14 features
- cmake
- OpenCL
- OpenCV (if you'd like to build `color2gray` program)

### Building
```bash
$ mkdir build && cd build
$ cmake ..
$ make -j4
```

You can also build only one of two examples by using cmake options:
- `-DONLY_VECTOR=ON` - only `vector_add` will be built.
- `-DONLY_Image=ON` - only `color2gray` will be built.

For example:
```bash
$ mkdir build && cd build
$ cmake -DONLY_VECTOR=ON ..
$ make -j4
```

### Project structure
- `common` - contains definitions and implementations of several common used
    functions.
- `images` - contains input and output image for `color2gray`.
- `implementations` - contains implementations of host programs.
- `kernels` - contains implementations of OpenCL kernels.

### Description of the programs
#### vector_add
This is the easies example of using OpenCL for general-purpose computing on GPU.
This program calculates sum of two input vectors and save the result to the
third vector. Vectors contain 1 million integer elements.

#### color2gray
This program converts color image to grayscale and write the output image to
file `out.png` in the directory from where `color2gray` was executed.

As input we take color logo of HSE with resolution 9917x9917 pixels:
![color logo](images/hse_color_logo.png)

After program execution we will get grayscale image:
![grayscale logo](images/color2gray_output.png)

### Run the programs
Both programs can take the following arguments:
```
Available arguments:
        cpu     Run program on CPU
        gpu     Run program on GPU (default)
        h, help Show this help message
```
By default program will be executed on the first available GPU. In case when no
GPUs available, then the program will be executed on CPU.

Example of execution program with command line arguments:
```bash
./vector_add cpu
```

### Performance
I measured performance of the programs on Ubuntu 20.04 with the following
hardware:
- CPU: `Intel(R) Core(TM) i7-7700K CPU @ 4.20GHz`
- GPU: `Nvidia GeForce GTX 660`

The mechanism of OpenCL events was used to measure the time spent on executing
program on the OpenCL device. More you can read
[here](https://man.opencl.org/clGetEventProfilingInfo.html).

The measured numbers are presented in the table below:

| Program      | CPU (ms) | GPU (ms) |
|--------------|----------|----------|
| `vector_add` | 0.45     | 0.12     |
| `color2gray` | 173.96   | 14.05    |
