cmake_minimum_required(VERSION 3.10.2)
project("opencl_webinar")

set(CMAKE_CXX_STANDARD 14)

option(ONLY_VECTOR "Build only vector_add program" OFF)
option(ONLY_IMAGE "Build only color2gray program" OFF)

find_package(OpenCL REQUIRED)

if(NOT ${ONLY_VECTOR})
    find_package(OpenCV REQUIRED)
endif()

include_directories(
        common/
        implementations/
        ${OpenCL_INCLUDE_DIR}
        ${OpenCV_INCLUDE_DIRS}
)

file(GLOB vector_src_files
        main_vector.cpp
        common/*.cpp
        implementations/vector_add.cpp
)

file(GLOB color2gray_src_files
        main_color2gray.cpp
        common/*.cpp
        implementations/color2gray.cpp
)

if(NOT ${ONLY_IMAGE})
    add_executable(vector_add
        ${vector_src_files}
    )

    target_link_libraries(vector_add
        ${OpenCL_LIBRARIES}
    )
endif()

if(NOT ${ONLY_VECTOR})
    add_executable(color2gray
        ${color2gray_src_files}
    )

    target_link_libraries(color2gray
        ${OpenCL_LIBRARIES}
        ${OpenCV_LIBS}
    )
endif()
