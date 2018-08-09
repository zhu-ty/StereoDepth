# CMake file for stereo giga calibration project
# Author: zhu-ty
# E-mail: zhu-ty@outlook.com
#

set(stereo_exe_name "DisparityWarping")

# General camera driver
set(OPENGL_WARPER_DIR ${PROJECT_SOURCE_DIR}/OpenGLImageWarper)

file(GLOB_RECURSE OPENGL_WARPER_FILE_CPP ${OPENGL_WARPER_DIR}/*.cpp)
file(GLOB_RECURSE OPENGL_WARPER_FILE_CU ${OPENGL_WARPER_DIR}/*.cu)
file(GLOB_RECURSE OPENGL_WARPER_FILE_H ${OPENGL_WARPER_DIR}/*.h)
file(GLOB_RECURSE OPENGL_WARPER_FILE_HPP ${OPENGL_WARPER_DIR}/*.hpp)

set(OPENGL_WARPER_FILE 
${OPENGL_WARPER_FILE_CPP}
${OPENGL_WARPER_FILE_CU}
${OPENGL_WARPER_FILE_H}
${OPENGL_WARPER_FILE_HPP})

list(REMOVE_ITEM OPENGL_WARPER_FILE ${OPENGL_WARPER_DIR}/main.cpp)

include_directories(
	${OPENGL_WARPER_DIR}
)

# OpenGL
find_package(OpenGL REQUIRED)
# Ask the user to give the paths to GLFW, GLEW and GLM.
# On Linux this can be found automatically with pkg-config; 
# see http://stackoverflow.com/questions/6889264/cmake-flags-for-opengl-using-glew-and-glfw, for instance.
if ( WIN32 )
	set(GLFW_INCLUDE_PATH "E:/libs/OpenGL/include/GLFW")
	set(GLFW_LIBRARY "E:/libs/OpenGL/lib/glfw3.lib" )
	set(GLEW_INCLUDE_PATH "E:/libs/OpenGL/include")
	set(GLEW_LIBRARY_DEBUG debug "E:/libs/OpenGL/lib/glew32d.lib")
	set(GLEW_LIBRARY_RELEASE optimized "E:/libs/OpenGL/lib/glew32.lib")
	set(GLM_INCLUDE_PATH "E:/libs/OpenGL/include")
endif()

include_directories(
	${GLFW_INCLUDE_PATH}
	${GLEW_INCLUDE_PATH}
	${GLM_INCLUDE_PATH}
)

#cuda
find_package(CUDA)
set(CUDA_NVCC_FLAGS
    ${CUDA_NVCC_FLAGS}
#    -O3 
	-gencode=arch=compute_61,code=\"sm_61,compute_61\"
)

cuda_add_executable(${stereo_exe_name}
    ${COMMON_HEADER_FILES}
    ${OPENGL_WARPER_FILE}
    main.cpp
)

target_link_libraries(${stereo_exe_name}
    # OpenCV
    ${OpenCV_LIBS}
    ${OPENGL_LIBRARY}
    ${GLFW_LIBRARY}
	${GLEW_LIBRARY_DEBUG}
	${GLEW_LIBRARY_RELEASE}
	${OpenCV_LIBS}
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
)