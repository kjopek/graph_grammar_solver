cmake_minimum_required(VERSION 2.8)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g -std=c++14")

set(BLA_VENDOR ATLAS)
set(USE_GALOIS FALSE CACHE BOOL "Use Galois scheduler.")

find_package(ATLAS REQUIRED)
find_package(OpenMP)
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

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

