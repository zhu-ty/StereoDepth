# CMake file for depth map estimation
# Author: Shane Yuan
# E-mail: yuanxy92@gmail.com
#


set(depth_exe_name "ImageRectify")

set(SK_ENCODER_DIR ${PROJECT_SOURCE_DIR}/SKEncoder)
# C++ version
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file (GLOB SKE_headers_h ${SK_ENCODER_DIR}/*.h)
file (GLOB SKE_headers_hpp ${SK_ENCODER_DIR}/*.hpp)
set (SKE_headers
	${SKE_headers_h}
	${SKE_headers_hpp}
)
file (GLOB SKE_sources_cpp ${SK_ENCODER_DIR}/*.cpp)
file (GLOB SKE_sources_c ${SK_ENCODER_DIR}/*.c)
set (SKE_sources
	${SKE_sources_cpp}
	${SKE_sources_c}
)
INCLUDE_DIRECTORIES(${SK_ENCODER_DIR})

SOURCE_GROUP(SKEncoder FILES
	${SKE_headers}
	${SKE_sources}
)

FOREACH(item ${SKE_sources})
  IF(${item} MATCHES "main.cpp")
    LIST(REMOVE_ITEM SKE_sources ${item})
  ENDIF(${item} MATCHES "main.cpp")
ENDFOREACH(item)

# cuda
find_package(CUDA)
set(CUDA_NVCC_FLAGS
    ${CUDA_NVCC_FLAGS}
#    -O3 
	-gencode=arch=compute_61,code=\"sm_61,compute_61\"
    )



cuda_add_executable(${depth_exe_name}
    ${COMMON_HEADER_FILES}
	${SKE_headers}
	${SKE_sources}
    StereoRectify.h
    StereoRectify.cpp
	FFVideoReader.h
	FFVideoReader.cpp
    main.cpp
	IRconfig.Sample.ini
)

target_link_libraries(${depth_exe_name}
    # OpenCV
    ${OpenCV_LIBS}
	cuda
)