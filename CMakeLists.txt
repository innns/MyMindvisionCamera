cmake_minimum_required(VERSION 3.26)
project(MyMindvisionCamera)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_PREFIX_PATH "C:/Qt/Qt5.14.2/5.14.2/mingw73_64")
#opencv 相关
set(OpenCV_DIR "C:/Projects/Dependencies-mingw/opencv-4.5.4/ming64-build/install")
set(OpenCV_LIBS opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs)
#set(Camera_LIBS CameraSdkStatus)
# 头文件加载
include_directories(
        ${PROJECT_SOURCE_DIR}/include
        "C:/Projects/Dependencies/Mindvision/Demo/VC++/Include"  # 迈德威视相机头文件
        "C:/Projects/MINDVISION/Demo/VC++/OpenCV"
)

# package
find_package(OpenCV REQUIRED)

#外部动态链接库目录,不推荐使用link_directories.

aux_source_directory(src INCLUDE_SRC)
add_library(INCLUDE ${INCLUDE_SRC})

add_library(MVCAMSDK STATIC IMPORTED)
#设置mvcamsdk库文件的路径，在根目录下的lib文件夹中
set_target_properties(MVCAMSDK PROPERTIES IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/lib/MVCAMSDK_X64.lib")

add_executable(MyMindvisionCamera main.cpp
)

if (OpenCV_FOUND)
    include_directories(${OpenCV_INCLUDE_DIRS})
    target_link_libraries(MyMindvisionCamera ${OpenCV_LIBS})
else (OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV lib not found")
endif ()

target_link_libraries(MyMindvisionCamera
        INCLUDE
        MVCAMSDK #迈德相机sdk
)
