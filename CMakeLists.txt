cmake_minimum_required(VERSION 3.13)
project(JSPAlgorithms)

set(CMAKE_CXX_STANDARD 17)

#set(EIGEN3_INCLUDE_DIR "d:/Program Files/libs/eigen")
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Project directory: ${PROJECT_SOURCE_DIR}")
message(STATUS "${CMAKE_MODULE_PATH}")

include_directories(include)

AUX_SOURCE_DIRECTORY(src DIR_SRCS)

#find_package(Eigen3 REQUIRED)
#if (EIGEN3_FOUND)
#    message(STATUS "Found Eigen3:\"${EIGEN3_INCLUDE_DIR}\",ENABLE_EIGEN3 Macro is enabled!")
#    include_directories(${EIGEN3_INCLUDE_DIR})
#    add_definitions(-DENABLE_EIGEN3)
#endif (EIGEN3_FOUND)

add_executable(taboo_search include/jsp.h src/taboo_search.cpp src/jsp.cpp)
#add_executable(eigen_test src/eigen_test.cpp)