cmake_minimum_required( VERSION 2.8 )
project( DOM_OESM )

set(CMAKE_CXX_FLAGS "-std=c++11")

#build type
set(CMAKE_BUILD_TYPE "Debug")


# 添加GDAL依赖
find_package( GDAL 2.4.2 REQUIRED )
include_directories( ${GDAL_INCLUDE_DIRS} )
link_directories( ${GDAL_LIBRARY_DIRS})
add_definitions(${GDAL_DEFINITIONS})

# 添加OpenCV依赖
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories( ${OpenCV_LIBRARY_DIRS})
add_definitions(${OpenCV_DEFINITIONS})


include_directories(includes) #添加头文件
 
FILE(GLOB_RECURSE CURRENT_INCLUDE includes/*.hpp)
FILE(GLOB_RECURSE CURRENT_SOURCES src/*.cpp)     #aux_source_directory(src CURRENT_SOURCES)
add_executable(dom_oesm ${CURRENT_SOURCES} ${CURRENT_INCLUDE})  #制定生成目标
target_link_libraries( dom_oesm ${OpenCV_LIBRARIES} ${GDAL_LIBRARIES} )
