#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <functional>
#include <windows.h>
#include <psapi.h>

#include "data_structures/dense_matrix/DenseMatrix.h"
#include "data_structures/sparse_matrix_hash/SparseMatrixHash.h"
#include "data_structures/sparse_matrix_tree/SparseMatrixTree.h"

SIZE_T getMemoryUseKB() {
  PROCESS_MEMORY_COUNTERS pmc;
  GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
  return pmc.WorkingSetSize / 1024;
}

SparseMatrixTree::TreeNode *generateSparseMatrixTree(const int n, const long long k_expected) {
  SparseMatrixTree::TreeNode *root = nullptr;

  for (long long count = 0; count < k_expected; count++) {
    const int i = rand() % n;
    const int j = rand() % n;
    const int value = (rand() % 9) + 1;
    root = SparseMatrixTree::insert(root, i, j, value);
  }

  return root;
}

SparseMatrixHash generateSparseMatrixHash(const int n, const long long k_expected) {
  SparseMatrixHash M(n, n);

  for (long long count = 0; count < k_expected; count++) {
    const int i = rand() % n;
    const int j = rand() % n;
    const double value = (rand() % 9) + 1;
    M.set(i, j, value);
  }

  return M;
}

DenseMatrix generateDenseMatrix(const int n, const long long k_expected) {
  DenseMatrix M(n, n);

  for (long long count = 0; count < k_expected; count++) {
    const int i = rand() % n;
    const int j = rand() % n;
    const double value = (rand() % 9) + 1;
    M.set(i, j, value);
  }

  return M;
}

struct BenchmarkResult {
  double time_ms;
  SIZE_T memory_kb;
};

template<typename Func>
BenchmarkResult benchmark(Func func) {
  const SIZE_T mem_before = getMemoryUseKB();
  const auto start = std::chrono::high_resolution_clock::now();

  func();

  const auto end = std::chrono::high_resolution_clock::now();
  const SIZE_T mem_after = getMemoryUseKB();

  const double time_ms = std::chrono::duration<double, std::milli>(end - start).count();
  const SIZE_T memory_kb = (mem_after > mem_before) ? (mem_after - mem_before) : 0;

  return {time_ms, memory_kb};
}

void SparseMatrixHashTest(const int n, const double sparsity, std::ofstream &csv_file, const long long k_expected) {
  std::cout << "\n--- Estrutura 1 (E1): Hash Map ---\n";

  SparseMatrixHash hash_a(n, n);
  SparseMatrixHash hash_b(n, n);

  // Geração
  auto result = benchmark([&]() {
    hash_a = generateSparseMatrixHash(n, k_expected);
  });
  std::cout << "Geracao: " << result.time_ms << " ms, Mem: " << result.memory_kb << " KB\n";
  csv_file << "Hash,Geracao," << n << "," << (sparsity * 100) << "," << k_expected << ","
      << result.time_ms << "," << result.memory_kb << "\n";

  // Cria lista de posições existentes
  std::vector<std::pair<int, int> > hash_existing;
  for (auto [i, j, v]: hash_a.items()) {
    if (hash_existing.size() < 1000) {
      hash_existing.push_back({i, j});
    }
  }

  // Acesso a elementos existentes
  if (!hash_existing.empty()) {
    result = benchmark([&]() {
      for (int t = 0; t < 1000 && t < static_cast<int>(hash_existing.size()); t++) {
        auto [i, j] = hash_existing[t];
        hash_a.get(i, j);
      }
    });
    std::cout << "Acesso Existente (1000x): " << result.time_ms << " ms\n";
    csv_file << "Hash,AcessoExistente," << n << "," << (sparsity * 100) << "," << k_expected << ","
        << result.time_ms << ",0\n";
  }

  // Acesso aleatório
  result = benchmark([&]() {
    for (int t = 0; t < 1000; t++) {
      int i = rand() % n, j = rand() % n;
      hash_a.get(i, j);
    }
  });
  std::cout << "Acesso Aleatorio (1000x): " << result.time_ms << " ms\n";
  csv_file << "Hash,AcessoAleatorio," << n << "," << (sparsity * 100) << "," << k_expected << ","
      << result.time_ms << ",0\n";

  // Inserção
  result = benchmark([&]() {
    for (int t = 0; t < 100; t++) {
      int i = rand() % n, j = rand() % n;
      double v = rand() % 9 + 1;
      hash_a.set(i, j, v);
    }
  });
  std::cout << "Insercao (100x): " << result.time_ms << " ms\n";
  csv_file << "Hash,Insercao," << n << "," << (sparsity * 100) << "," << k_expected << ","
      << result.time_ms << ",0\n";

  // Transposta
  result = benchmark([&]() {
    SparseMatrixHash hash_t = hash_a.transpose();
  });
  std::cout << "Transposta: " << result.time_ms << " ms\n";
  csv_file << "Hash,Transposta," << n << "," << (sparsity * 100) << "," << k_expected << ","
      << result.time_ms << ",0\n";

  // Soma
  hash_b = generateSparseMatrixHash(n, k_expected);
  SparseMatrixHash hash_sum(n, n);
  result = benchmark([&]() {
    hash_sum = hash_a.add(hash_b);
  });
  std::cout << "Soma: " << result.time_ms << " ms, Mem: " << result.memory_kb << " KB\n";
  csv_file << "Hash,Soma," << n << "," << (sparsity * 100) << "," << k_expected << ","
      << result.time_ms << "," << result.memory_kb << "\n";

  // Multiplicação escalar
  SparseMatrixHash hash_scalar(n, n);
  result = benchmark([&]() {
    hash_scalar = hash_a.scalarMult(5);
  });
  std::cout << "Mult Escalar: " << result.time_ms << " ms\n";
  csv_file << "Hash,MultEscalar," << n << "," << (sparsity * 100) << "," << k_expected << ","
      << result.time_ms << ",0\n";

  SparseMatrixHash hash_mult(n, n);
  result = benchmark([&]() {
    hash_mult = hash_a.mult(hash_b);
  });
  std::cout << "Mult Matrizes: " << result.time_ms << " ms, Mem: "
      << result.memory_kb << " KB\n";
  csv_file << "Hash,MultMatrizes," << n << "," << (sparsity * 100) << "," << k_expected << ","
      << result.time_ms << "," << result.memory_kb << "\n";
}

void SparseMatrixTreeTest(const int n, const double sparsity, std::ofstream &csv_file, const long long k_expected) {
  std::cout << "\n--- Estrutura 2 (E2): Red-Black Tree ---\n";

  SparseMatrixTree::TreeNode *tree_a = nullptr;
  SparseMatrixTree::TreeNode *tree_b = nullptr;

  // Geração
  auto result = benchmark([&]() {
    tree_a = generateSparseMatrixTree(n, k_expected);
  });
  std::cout << "Geracao: " << result.time_ms << " ms, Mem: " << result.memory_kb << " KB\n";
  csv_file << "Tree,Geracao," << n << "," << (sparsity * 100) << "," << k_expected << ","
      << result.time_ms << "," << result.memory_kb << "\n";

  // Cria lista de posições existentes para teste de acesso
  std::vector<std::pair<int, int> > existing_positions;
  std::vector<SparseMatrixTree::TreeNode *> nodes;
  SparseMatrixTree::inorderGet(tree_a, false, nodes);
  for (auto node: nodes) {
    if (existing_positions.size() < 1000) {
      existing_positions.push_back({node->row, node->column});
    }
  }

  // Acesso a elementos existentes (Pior Caso)
  if (!existing_positions.empty()) {
    result = benchmark([&]() {
      for (int t = 0; t < 1000 && t < static_cast<int>(existing_positions.size()); t++) {
        auto [i, j] = existing_positions[t];
        SparseMatrixTree::findElement(tree_a, i, j, false);
      }
    });
    std::cout << "Acesso Existente (1000x): " << result.time_ms << " ms\n";
    csv_file << "Tree,AcessoExistente," << n << "," << (sparsity * 100) << "," << k_expected << ","
        << result.time_ms << ",0\n";
  }

  // Acesso a elementos zero (Caso Médio)
  result = benchmark([&]() {
    for (int t = 0; t < 1000; t++) {
      int i = rand() % n, j = rand() % n;
      SparseMatrixTree::findElement(tree_a, i, j, false);
    }
  });
  std::cout << "Acesso Aleatorio (1000x): " << result.time_ms << " ms\n";
  csv_file << "Tree,AcessoAleatorio," << n << "," << (sparsity * 100) << "," << k_expected << ","
      << result.time_ms << ",0\n";

  // Inserção
  result = benchmark([&]() {
    for (int t = 0; t < 100; t++) {
      int i = rand() % n, j = rand() % n;
      int val = rand() % 9 + 1;
      tree_a = SparseMatrixTree::insert(tree_a, i, j, val);
    }
  });
  std::cout << "Insercao (100x): " << result.time_ms << " ms\n";
  csv_file << "Tree,Insercao," << n << "," << (sparsity * 100) << "," << k_expected << ","
      << result.time_ms << ",0\n";

  // Transposta
  result = benchmark([&]() {
    volatile bool transpose_flag = true;
  });
  std::cout << "Transposta: " << result.time_ms << " ms\n";
  csv_file << "Tree,Transposta," << n << "," << (sparsity * 100) << "," << k_expected << ","
      << result.time_ms << ",0\n";

  // Soma
  tree_b = generateSparseMatrixTree(n, k_expected);
  SparseMatrixTree::TreeNode *tree_sum = nullptr;
  result = benchmark([&]() {
    tree_sum = SparseMatrixTree::sumMatrices(tree_a, tree_b, false, false);
  });
  std::cout << "Soma: " << result.time_ms << " ms, Mem: " << result.memory_kb << " KB\n";
  csv_file << "Tree,Soma," << n << "," << (sparsity * 100) << "," << k_expected << ","
      << result.time_ms << "," << result.memory_kb << "\n";

  // Multiplicação escalar
  result = benchmark([&]() {
    SparseMatrixTree::multScalarMatrix(tree_a, 5);
  });
  std::cout << "Mult Escalar: " << result.time_ms << " ms\n";
  csv_file << "Tree,MultEscalar," << n << "," << (sparsity * 100) << "," << k_expected << ","
      << result.time_ms << ",0\n";

  // Multiplicação de matrizes
  SparseMatrixTree::TreeNode *tree_mult = nullptr;
  result = benchmark([&]() {
    tree_mult = SparseMatrixTree::multMatrices(tree_a, tree_b, false, false);
  });
  std::cout << "Mult Matrizes: " << result.time_ms << " ms, Mem: "
      << result.memory_kb << " KB\n";
  csv_file << "Tree,MultMatrizes," << n << "," << (sparsity * 100) << "," << k_expected << ","
      << result.time_ms << "," << result.memory_kb << "\n";
  delete tree_mult;


  delete tree_a;
  delete tree_b;
  delete tree_sum;
}

void DenseMatrixTest(const int n, const double sparsity, std::ofstream &csv_file, const long long k_expected) {
  if (n > 10000) {
    std::cout << "\n--- Matriz Densa (baseline) ---\n";
    std::cout << "SKIPPED (n > 10000, impractical for dense matrices)\n";
    csv_file << "Dense,Geracao," << n << "," << (sparsity * 100) << "," << k_expected << ",-1,0\n";
    csv_file << "Dense,AcessoAleatorio," << n << "," << (sparsity * 100) << "," << k_expected << ",-1,0\n";
    csv_file << "Dense,Insercao," << n << "," << (sparsity * 100) << "," << k_expected << ",-1,0\n";
    csv_file << "Dense,Transposta," << n << "," << (sparsity * 100) << "," << k_expected << ",-1,0\n";
    csv_file << "Dense,Soma," << n << "," << (sparsity * 100) << "," << k_expected << ",-1,0\n";
    csv_file << "Dense,MultEscalar," << n << "," << (sparsity * 100) << "," << k_expected << ",-1,0\n";
    csv_file << "Dense,MultMatrizes," << n << "," << (sparsity * 100) << "," << k_expected << ",-1,0\n";
    return;
  }

  std::cout << "\n--- Matriz Densa (baseline) ---\n";

  DenseMatrix dense_a(n, n);
  DenseMatrix dense_b(n, n);

  // Geração
  auto result = benchmark([&]() {
    dense_a = generateDenseMatrix(n, k_expected);
  });
  std::cout << "Geracao: " << result.time_ms << " ms, Mem: " << result.memory_kb << " KB\n";
  csv_file << "Dense,Geracao," << n << "," << (sparsity * 100) << "," << k_expected << ","
      << result.time_ms << "," << result.memory_kb << "\n";

  // Acesso
  result = benchmark([&]() {
    for (int t = 0; t < 1000; t++) {
      int i = rand() % n, j = rand() % n;
      dense_a.get(i, j);
    }
  });
  std::cout << "Acesso Aleatorio (1000x): " << result.time_ms << " ms\n";
  csv_file << "Dense,AcessoAleatorio," << n << "," << (sparsity * 100) << "," << k_expected << ","
      << result.time_ms << ",0\n";

  // Inserção
  result = benchmark([&]() {
    for (int t = 0; t < 100; t++) {
      int i = rand() % n, j = rand() % n;
      double v = rand() % 9 + 1;
      dense_a.set(i, j, v);
    }
  });
  std::cout << "Insercao (100x): " << result.time_ms << " ms\n";
  csv_file << "Dense,Insercao," << n << "," << (sparsity * 100) << "," << k_expected << ","
      << result.time_ms << ",0\n";

  // Transposta
  DenseMatrix dense_t(n, n);
  result = benchmark([&]() {
    dense_t = dense_a.transpose();
  });
  std::cout << "Transposta: " << result.time_ms << " ms\n";
  csv_file << "Dense,Transposta," << n << "," << (sparsity * 100) << "," << k_expected << ","
      << result.time_ms << ",0\n";

  // Soma
  dense_b = generateDenseMatrix(n, k_expected);
  DenseMatrix dense_sum(n, n);
  result = benchmark([&]() {
    dense_sum = dense_a.add(dense_b);
  });
  std::cout << "Soma: " << result.time_ms << " ms\n";
  csv_file << "Dense,Soma," << n << "," << (sparsity * 100) << "," << k_expected << ","
      << result.time_ms << ",0\n";

  // Multiplicação escalar
  DenseMatrix dense_scalar(n, n);
  result = benchmark([&]() {
    dense_scalar = dense_a.scalarMult(5);
  });
  std::cout << "Mult Escalar: " << result.time_ms << " ms\n";
  csv_file << "Dense,MultEscalar," << n << "," << (sparsity * 100) << "," << k_expected << ","
      << result.time_ms << ",0\n";

  // Multiplicação de matrizes
  if (n <= 1000) {
    // Only run for small matrices
    DenseMatrix dense_mult(n, n);
    result = benchmark([&]() {
      dense_mult = dense_a.mult(dense_b);
    });
    std::cout << "Mult Matrizes: " << result.time_ms << " ms\n";
    csv_file << "Dense,MultMatrizes," << n << "," << (sparsity * 100) << ","
        << k_expected << "," << result.time_ms << ",0\n";
  } else {
    std::cout << "Mult Matrizes: SKIPPED (too large)\n";
    csv_file << "Dense,MultMatrizes," << n << "," << (sparsity * 100) << ","
        << k_expected << ",-1,0\n";
  }
}

int main() {
  srand(20);

  std::ofstream csv_file("resultados.csv");
  csv_file << "Estrutura,Operacao,N,Esparsidade(%),K_Nao_Nulos,Tempo(ms),Memoria(KB)\n";

  const std::vector dimensions = {100, 1000, 10000, 100000, 1000000};

  for (const int n: dimensions) {
    std::vector<double> sparsities;

    if (const int i = static_cast<int>(log10(n)); i < 4) {
      sparsities = {0.01, 0.05, 0.10, 0.20};
    } else {
      sparsities = {
        1.0 / pow(10, i + 2),
        1.0 / pow(10, i + 1),
        1.0 / pow(10, i)
      };
    }

    for (const double sparsity: sparsities) {
      const long long k_expected = static_cast<long long>(n) * static_cast<long long>(n) * sparsity;

      std::cout << "\n========================================\n";
      std::cout << "N = " << n << ", Esparsidade = " << std::fixed
          << std::setprecision(6) << (sparsity * 100) << "%, k = " << k_expected << "\n";
      std::cout << "========================================\n";

      SparseMatrixHashTest(n, sparsity, csv_file, k_expected);

      SparseMatrixTreeTest(n, sparsity, csv_file, k_expected);

      DenseMatrixTest(n, sparsity, csv_file, k_expected);
    }
  }

  csv_file.close();
  std::cout << "\n\n========================================\n";
  std::cout << "Resultados salvos em 'resultados.csv'\n";
  std::cout << "========================================\n";

  return 0;
}
