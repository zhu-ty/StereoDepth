

set(stereo_exe_name "StereoAcquisition")

# Camera Driver files
# camera SDK
# pointgrey camera sdk
if ( WIN32 )
    set(PTGREY_SDK_INCLUDE_DIR "C:/Program Files/Point Grey Research/Spinnaker/include")
	set(PTGREY_SDK_LIBRARY_DEBUG debug "C:/Program Files/Point Grey Research/Spinnaker/lib64/vs2015/Spinnakerd_v140.lib")
	set(PTGREY_SDK_LIBRARY_RELEASE  optimized "C:/Program Files/Point Grey Research/Spinnaker/lib64/vs2015/Spinnaker_v140.lib")
else()
	set(PTGREY_SDK_INCLUDE_DIR $ENV{HOME}/zhu-ty/spinnaker/include)
    set(PTGREY_SDK_INCLUDE_DIR ${PTGREY_SDK_INCLUDE_DIR} "/usr/include/spinnaker/")
    if(EXISTS $ENV{HOME}/zhu-ty/spinnaker/lib/libSpinnaker.so)
        set(PTGREY_SDK_LIBRARY_DEBUG debug $ENV{HOME}/zhu-ty/spinnaker/lib/libSpinnaker.so)
        set(PTGREY_SDK_LIBRARY_RELEASE optimized $ENV{HOME}/zhu-ty/spinnaker/lib/libSpinnaker.so)
    else()
    set(PTGREY_SDK_LIBRARY_DEBUG debug "libSpinnaker.so")
    set(PTGREY_SDK_LIBRARY_RELEASE optimized "libSpinnaker.so")
    endif()
endif()
include_directories(${PTGREY_SDK_INCLUDE_DIR})

# XIMEA camera
if ( WIN32 )
    set(XIMEA_SDK_INCLUDE_DIR "C:/XIMEA/API")
    set(XIMEA_SDK_LIBRARY_DEBUG debug "C:/XIMEA/API/x64/xiapi64.lib")
    set(XIMEA_SDK_LIBRARY_RELEASE optimized "C:/XIMEA/API/x64/xiapi64.lib")
else ()
    set(XIMEA_SDK_INCLUDE_DIR "/opt/XIMEA/include")
    set(XIMEA_SDK_LIBRARY_DEBUG debug "/usr/lib/libm3api.so")
    set(XIMEA_SDK_LIBRARY_RELEASE optimized "/usr/lib/libm3api.so")
endif()
include_directories(${XIMEA_SDK_INCLUDE_DIR})


# General camera driver
set(CAMERA_DRIVER_DIR ${PROJECT_SOURCE_DIR}/GenCameraDriver)

file(GLOB_RECURSE CAMERA_DRIVER_FILE_CPP ${CAMERA_DRIVER_DIR}/*.cpp)
file(GLOB_RECURSE CAMERA_DRIVER_FILE_C ${CAMERA_DRIVER_DIR}/*.c)
file(GLOB_RECURSE CAMERA_DRIVER_FILE_CU ${CAMERA_DRIVER_DIR}/*.cu)
file(GLOB_RECURSE CAMERA_DRIVER_FILE_H ${CAMERA_DRIVER_DIR}/*.h)
file(GLOB_RECURSE CAMERA_DRIVER_FILE_HPP ${CAMERA_DRIVER_DIR}/*.hpp)

set(CAMERA_DRIVER_FILE 
${CAMERA_DRIVER_FILE_CPP}
${CAMERA_DRIVER_FILE_CU}
${CAMERA_DRIVER_FILE_C}
${CAMERA_DRIVER_FILE_H}
${CAMERA_DRIVER_FILE_HPP})

list(REMOVE_ITEM CAMERA_DRIVER_FILE ${CAMERA_DRIVER_DIR}/SKEncoder/main.cpp)
list(REMOVE_ITEM CAMERA_DRIVER_FILE ${CAMERA_DRIVER_DIR}/main.cpp)

IF(WIN32)
	file(GLOB_RECURSE REALTIME_DEPTH_H ${CAMERA_DRIVER_DIR}/Stereo/RealTimeDepth/*.h)
	file(GLOB_RECURSE REALTIME_DEPTH_CPP ${CAMERA_DRIVER_DIR}/Stereo/RealTimeDepth/*.cpp)
	file(GLOB_RECURSE REALTIME_DEPTH_C ${CAMERA_DRIVER_DIR}/Stereo/RealTimeDepth/*.c)
	list(REMOVE_ITEM CAMERA_DRIVER_FILE ${REALTIME_DEPTH_H})
	list(REMOVE_ITEM CAMERA_DRIVER_FILE ${REALTIME_DEPTH_CPP})
	list(REMOVE_ITEM CAMERA_DRIVER_FILE ${REALTIME_DEPTH_C})
ELSE()
	FIND_PACKAGE(Eigen3 REQUIRED) 
	include_directories(${EIGEN3_INCLUDE_DIR})
	list(REMOVE_ITEM CAMERA_DRIVER_FILE ${CAMERA_DRIVER_DIR}/Stereo/RealTimeDepth/run_dense.cpp)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wno-unknown-pragmas -std=c++11 -msse4")  #-Wall
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wno-unknown-pragmas -msse4")  #-Wall
    include_directories(${CAMERA_DRIVER_DIR}/Stereo/RealTimeDepth)
ENDIF()


list(REMOVE_ITEM CAMERA_DRIVER_FILE ${CAMERA_DRIVER_DIR}/main.cpp)

set(CAMERA_DRIVER_LIB
	${PTGREY_SDK_LIBRARY}
	${XIMEA_SDK_LIBRARY}
)
include_directories(
	${CAMERA_DRIVER_DIR}
	${PTGREY_SDK_INCLUDE_DIR}
	${XIMEA_SDK_INCLUDE_DIR}
	${CAMERA_DRIVER_DIR}/FileCamera
	${CAMERA_DRIVER_DIR}/SKEncoder
	${CAMERA_DRIVER_DIR}/SKEncoder/NvEncoder
)



# C++ header file
set(MY_HEADER_FILES
    # C++ header file
	#${PROJECT_SOURCE_DIR}/Common/SysUtil.hpp
	${COMMON_HEADER_FILES}
    # StereoAcquisition.h
)

set(MY_SOURCE_FILES 
	# C++ code
    # StereoAcquisition.cpp
    # StereoAcquisition.cu
	${CAMERA_DRIVER_FILE}
)

cuda_add_executable(${stereo_exe_name}
    ${MY_HEADER_FILES}
    ${MY_SOURCE_FILES}
    main.cpp
)

target_link_libraries(${stereo_exe_name}
    # OpenCV
    ${OpenCV_LIBS}
    # cuda libs
	cuda
	${NVCUVID_LIB}
    ${CUDA_LIBRARY} 	
	${CUDA_npp_LIBRARY} 
	${CUDA_nppc_LIBRARY} 
	${CUDA_nppicc_LIBRARY}
	${CUDA_nppicom_LIBRARY}
	${CUDA_nppidei_LIBRARY}
	${CUDA_nppif_LIBRARY}
	${CUDA_nppig_LIBRARY}
	${CUDA_nppim_LIBRARY}
	${CUDA_nppist_LIBRARY}
	${CUDA_nppisu_LIBRARY}
	${CUDA_nppitc_LIBRARY}
	${CUDA_npps_LIBRARY} 
	# camera libs
	${PTGREY_SDK_LIBRARY_DEBUG}
	${PTGREY_SDK_LIBRARY_RELEASE}
	${XIMEA_SDK_LIBRARY_DEBUG}
	${XIMEA_SDK_LIBRARY_RELEASE}
)
