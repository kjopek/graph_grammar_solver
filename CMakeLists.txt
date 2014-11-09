cmake_minimum_required(VERSION 2.8)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g")

set(BLAS_VENDOR ATLAS)
find_package(BLAS REQUIRED)

shared_library(SolverLib EquationSystem.cpp Analysis.cpp Node.cpp Mesh.cpp)
target_link_libraries(SolverLib -lcblas -llapack -latlas -lsatlas -ltatlas)
