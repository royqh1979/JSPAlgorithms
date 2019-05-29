cmake_minimum_required(VERSION 3.13)
project(JSPAlgorithms)

set(CMAKE_CXX_STANDARD 17)

#set(EIGEN3_INCLUDE_DIR "d:/Program Files/libs/eigen")
#set(BOOST_ROOT "D:/Program Files/boost_1_70_0")
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Project directory: ${PROJECT_SOURCE_DIR}")
message(STATUS "${CMAKE_MODULE_PATH}")

include_directories(include)

AUX_SOURCE_DIRECTORY(src DIR_SRCS)

find_package(boost REQUIRED)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
else(Boost_FOUND)
    message(FATAL_ERROR "Boost not found!")
endif(Boost_FOUND)

find_package(PkgConfig REQUIRED)
pkg_search_module(Graphviz REQUIRED libgvc)

#find_package(Eigen3 REQUIRED)
#if (EIGEN3_FOUND)
#    message(STATUS "Found Eigen3:\"${EIGEN3_INCLUDE_DIR}\",ENABLE_EIGEN3 Macro is enabled!")
#    include_directories(${EIGEN3_INCLUDE_DIR})
#    add_definitions(-DENABLE_EIGEN3)
#endif (EIGEN3_FOUND)

# 打开vector下标越界检测
SET(CMAKE_CXX_FLAGS "-D_GLIBCXX_CONCEPT_CHECKS -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC")
SET(CMAKE_CXX_FLAGS "-D_DEBUG")
# SET(CMAKE_EXE_LINKER_FLAGS "-D_GLIBCXX_CONCEPT_CHECKS -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC")

add_library(jsp STATIC src/jsp.cpp)
target_link_libraries(jsp ${Graphviz_LIBRARIES})
target_include_directories(jsp PUBLIC ${Graphviz_INCLUDE_DIRS})
# target_compile_options(jsp ${Graphviz_CFLAGS_OTHER})

add_executable(test_read_data src/test_read_data.cpp )
target_link_libraries(test_read_data jsp )
add_executable(global_search src/global_search.cpp )
target_link_libraries(global_search jsp )
#add_executable(eigen_test src/eigen_test.cpp)