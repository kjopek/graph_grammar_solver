# Tests
add_executable(test_schur_chol test_schur_chol.cpp)
target_link_libraries(test_schur_chol -lalapack -lcblas -lf77blas)

add_executable(test_schur_lu test_schur_lu.cpp)
target_link_libraries(test_schur_lu -lalapack -lcblas -lf77blas)
