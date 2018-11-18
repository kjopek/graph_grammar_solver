project(solver)

add_library(ElementSolverLib STATIC EquationSystem.cc Node.cc Solver.cc log.c)
add_executable(ElementSolver main.cc)

target_link_libraries(ElementSolverLib -lalapack -lcblas -lf77blas)
target_link_libraries(ElementSolver -lalapack -lcblas -lf77blas ElementSolverLib LLVMSupport)
