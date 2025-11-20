cmake_minimum_required(VERSION 4.0)
project(MC458_Projeto)

set(CMAKE_CXX_STANDARD 17)

add_executable(MC458_Projeto
        sparsematrix.cpp
        SparseMatrixHash.cpp
        DenseMatrix.cpp)
