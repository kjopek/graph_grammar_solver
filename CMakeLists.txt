cmake_minimum_required(VERSION 2.8)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g -std=c++14")

set(BLA_VENDOR ATLAS)
find_package(ATLAS REQUIRED)
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${ATLAS_INCLUDE_DIR})
include_directories(/usr/local/include/)

link_directories(/usr/local/lib/)
link_directories(/opt/local/lib/) # For Atlas installed via OS X MacPorts 
link_directories(${LLVM_LIBRARY_DIRS})

add_definitions(${LLVM_DEFINITIONS})

add_library(ElementSolverLib STATIC EquationSystem.cc Node.cc Solver.cc log.c)
add_executable(ElementSolver main.cc)

target_link_libraries(ElementSolverLib -lalapack -lcblas -lf77blas)
target_link_libraries(ElementSolver -lalapack -lcblas -lf77blas ElementSolverLib LLVMSupport)
