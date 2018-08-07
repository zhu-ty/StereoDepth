# CMake file for StereoDepth Cpp Project
# Author: zhu-ty
# E-mail: zhu-ty@outlook.com
#
message(
"
       
           ┌─┐       ┌─┐
        ┌──┘ ┴───────┘ ┴──┐
        │                 │
        │       ───       │
        │  ─┬┘       └┬─  │
        │                 │
        │       ─┴─       │
        │                 │
        └───┐         ┌───┘
            │         │
            │         │
            │         │
            │         └──────────────┐
            │                        │
            │                        ├─┐
            │                        ┌─┘    
            │                        │
            └─┐  ┐  ┌───────┬──┐  ┌──┘         
              │ ─┤ ─┤       │ ─┤ ─┤         
              └──┴──┘       └──┴──┘ 
                  神兽  保佑 
                  代码无BUG! 
  
")

cmake_minimum_required (VERSION 3.10)
set(name "StereoDepthCpp")
project ( ${name} )

# set output path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/bin/Debug)    
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR}/bin/Release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/bin/Debug)    
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR}/bin/Release)  

# cmake module
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# C++ version
set (CMAKE_CXX_STANDARD 11)

# C++ library postfix
set (CMAKE_DEBUG_POSTFIX "_d")

#opencv 
if ( WIN32 )
	set(OpenCV_DIR "E:/libs/OpenCV/x64/vc14/lib")
	set(OpenCV_CONFIG_PATH "E:/libs/OpenCV")
endif()

# OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

#cuda
find_package(CUDA)
set(CUDA_NVCC_FLAGS
    ${CUDA_NVCC_FLAGS}
#    -O3 
	-gencode=arch=compute_61,code=\"sm_61,compute_61\"
    )

# include directories
include_directories(${PROJECT_SOURCE_DIR})
set(COMMON_DIR ${PROJECT_SOURCE_DIR}/Common)
include_directories(${COMMON_DIR})

file (GLOB headers_h "${COMMON_DIR}/*.h")
file (GLOB headers_hpp "${COMMON_DIR}/*.hpp")
set (COMMON_HEADER_FILES
${headers_h}
${headers_hpp}
)
#message("My headers: " ${COMMON_HEADER_FILES})


# add sub-directory
add_subdirectory("DisparityWarping")
add_subdirectory("GenCameraDriver")
add_subdirectory("ImageRectify")
add_subdirectory("StereoAcquisition")
add_subdirectory("StereoCalibration")