# CMake file for stereo giga calibration project
# Author: zhu-ty
# E-mail: zhu-ty@outlook.com
#

set(stereo_exe_name "DisparityWarping")

add_executable(${stereo_exe_name}
    ${COMMON_HEADER_FILES}
    main.cpp
)

target_link_libraries(${stereo_exe_name}
    # OpenCV
    ${OpenCV_LIBS}
)