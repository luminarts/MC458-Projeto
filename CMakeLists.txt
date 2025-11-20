cmake_minimum_required(VERSION 4.0)
project(MC458_Projeto)

set(CMAKE_CXX_STANDARD 17)

add_executable(MC458_Projeto
        src/Main.cpp
        src/data_structures/sparse_matrix_hash/SparseMatrixHash.cpp
        src/data_structures/dense_matrix/DenseMatrix.cpp
        src/data_structures/sparse_matrix_tree/SparseMatrixTree.cpp)
