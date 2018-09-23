cmake_minimum_required(VERSION 2.8)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g -std=c++11")

set(BLA_VENDOR ATLAS)
find_package(ATLAS REQUIRED)

include_directories(${ATLAS_INCLUDE_DIR})
include_directories(/usr/local/include/)

link_directories(/usr/local/lib/)
link_directories(/opt/local/lib/) # For Atlas installed via OS X MacPorts 

add_executable(ElementSolver EquationSystem.cpp Analysis.cpp Node.cpp Mesh.cpp log.c main.cpp)
target_link_libraries(ElementSolver -lalapack -lcblas -lf77blas)
