# CMake file for depth map estimation
# Author: Shane Yuan
# E-mail: yuanxy92@gmail.com
#


set(depth_exe_name "ImageRectify")

add_executable(${depth_exe_name}
    ${COMMON_HEADER_FILES}
    StereoRectify.h
    StereoRectify.cpp
    main.cpp
	IRconfig.Sample.ini
)

target_link_libraries(${depth_exe_name}
    # OpenCV
    ${OpenCV_LIBS}
)