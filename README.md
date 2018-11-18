# Budowanie
W katalogu build zrob:

    env CC=gcc7 CXX=g++7 cmake .. -DATLAS_DIR=`pwd`/../

Zmienne CC i CXX są opcjonalne i można je pominąć, jeśli domyślnym kompilatorem
jest cc i c++.
