#include <iostream>
#include <chrono>
#include <windows.h>
#include <psapi.h>

#include "data_structures/dense_matrix/DenseMatrix.h"
#include "data_structures/sparse_matrix_hash/SparseMatrixHash.h"
#include "data_structures/sparse_matrix_tree/SparseMatrixTree.h"


SparseMatrixTree::TreeNode *generateSparseMatrix(int n, double sparsity) {
  SparseMatrixTree::TreeNode *root = nullptr;

  int nonZeros = (int) (n * n * sparsity);

  for (int k = 0; k < nonZeros; k++) {
    int i = rand() % n;
    int j = rand() % n;
    int val = (rand() % 9) + 1;

    root = SparseMatrixTree::insert(root, i, j, val);
  }
  return root;
}

SIZE_T getMemoryUseKB() {
  PROCESS_MEMORY_COUNTERS pmc;
  GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
  return pmc.WorkingSetSize / 1024; // KB
}

DenseMatrix generateDenseMatrix(const int n, const double sparsity) {
  DenseMatrix M(n, n);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (const double r = static_cast<double>(rand()) / RAND_MAX; r < sparsity) {
        const double value = rand() % 9 + 1;
        M.set(i, j, value);
      }
    }
  }

  return M;
}

SparseMatrixHash generateSparseMatrixHash(const int n, const double sparsity) {
  SparseMatrixHash M(n, n);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (const double r = static_cast<double>(rand()) / RAND_MAX; r < sparsity) {
        const double value = rand() % 9 + 1;
        M.set(i, j, value);
      }
    }
  }

  return M;
}

int main() {
  srand(20); // setar a seed pra ter reproducibilidade

  int n = 10;
  double sparsity = 0.1;

  SparseMatrixTree::TreeNode *a = generateSparseMatrix(n, sparsity);
  SparseMatrixTree::TreeNode *b = generateSparseMatrix(n, sparsity);

  bool transpose_a = false;
  bool transpose_b = false;

  std::cout << "Matriz A:" << "\n";
  SparseMatrixTree::printTree(a, transpose_a);

  std::cout << "Matriz B:" << "\n";
  SparseMatrixTree::printTree(b, transpose_b);

  auto startMM = std::chrono::high_resolution_clock::now();
  SIZE_T MM_mem_start = getMemoryUseKB();

  SparseMatrixTree::TreeNode *ABMult = SparseMatrixTree::multMatrices(a, b, transpose_a, transpose_b);
  std::cout << "Resultado multiplicacao de matrizes" << "\n";
  SparseMatrixTree::printTree(ABMult, false);

  SIZE_T MM_mem_end = getMemoryUseKB();
  auto endMM = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> multMatricesTime = endMM - startMM;
  std::cout << "Multiplicacao de matrizes " << n << "x" << n << " com esparsidade " << sparsity * 100 << "%" << "\n";
  std::cout << "Tempo: " << multMatricesTime.count() << "s\n";
  std::cout << "Memória: " << MM_mem_end - MM_mem_start;
  // SparseMatrixTree::TreeNode* ABSum = sumMatrices(a, b, transpose_a, transpose_b);
  // std::cout << "Resultado soma matrizes:" << "\n";
  // SparseMatrixTree::printTree(ABSum, false);

  // int scalar = 5;
  // multScalarMatrix(a, scalar);
  // std::cout << "Resultado multiplicacao escalar:" << "\n";
  // printTree(a, transpose_a);
}
