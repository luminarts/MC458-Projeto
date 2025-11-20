#include <iostream>
#include <vector>
#include <chrono>
#include <windows.h>
#include <psapi.h>

#include "DenseMatrix.h"
#include "SparseMatrixHash.h"

// Estrutura 2: Árvore binária com cada nó tendo o número em si e a sua posição numa matriz

// CHECK FINAL
enum Color { RED, BLACK };

// CHECK FINAL
struct TreeNode {
  int value;
  int row, column;

  Color color;
  TreeNode *left, *right, *parent;

  TreeNode(int v = 0, int rw = 0, int col = 0, Color clr = BLACK, TreeNode *l = nullptr, TreeNode *r = nullptr,
           TreeNode *p = nullptr) : value(v), row(rw), column(col), color(clr), left(l), right(r), parent(p) {
  };

  ~TreeNode() {
    delete(left);
    delete(right);
  }
};

// CHECK FINAL
bool isLessThan(int i1, int j1, int i2, int j2) {
  if (i1 < i2) {
    return true;
  } else if (i1 == i2) {
    return j1 < j2;
  }
  return false;
}

// (1, 1) < (1, 2) < .. < (1, n) < (2, 1)

// Funções da árvore rubro-negra (IMPLEMENTAR LEFT E RIGHT ROTATIONS)

bool isRed(TreeNode *node) {
  if (node == nullptr) {
    return false;
  }
  return node->color == RED;
}

bool isBlack(TreeNode *node) {
  if (node == nullptr) {
    return 1;
  } else {
    return node->color == BLACK;
  }
}

TreeNode *rotateLeft(TreeNode *root) {
  TreeNode *node = root->right;
  root->right = node->left;
  node->left = root;
  node->color = root->color;
  root->color = RED;
  return node;
}

TreeNode *rotateRight(TreeNode *root) {
  TreeNode *node = root->left;
  root->left = node->right;
  node->right = root;
  node->color = root->color;
  root->color = RED;
  return node;
}

void riseRed(TreeNode *root) {
  root->color = RED;
  root->left->color = BLACK;
  root->right->color = BLACK;
}

TreeNode *insertRBTree(TreeNode *root, int i, int j, int valueToInsert) {
  TreeNode *node = nullptr;
  if (root == nullptr) {
    node = new TreeNode(valueToInsert, i, j, RED);
    return node;
  }

  // Fix for transpose
  if (isLessThan(i, j, root->row, root->column)) {
    root->left = insertRBTree(root->left, i, j, valueToInsert);
  } else {
    root->right = insertRBTree(root->right, i, j, valueToInsert);
  }

  if (isRed(root->right) && isBlack(root->left)) {
    root = rotateLeft(root);
  }
  if (isRed(root->left) && isRed(root->left->left)) {
    root = rotateRight(root);
  }
  if (isRed(root->left) && isRed(root->right)) {
    riseRed(root);
  }

  return root;
}

TreeNode *insert(TreeNode *root, int i, int j, int valueToInsert) {
  root = insertRBTree(root, i, j, valueToInsert);
  root->color = BLACK;
  return root;
}


// Funções para matriz (ADAPTAR TUDO PRA TER ARVORE RUBRO-NEGRA)

// CHECK - NÃO PRECISA DE ADAPTAÇÃO PRA RUBRONEGRA
TreeNode *findElement(TreeNode *node, int i, int j, bool transpose) {
  while (node) {
    int nodeRow = transpose ? node->column : node->row;
    int nodeColumn = transpose ? node->row : node->column;

    if (i == nodeRow && j == nodeColumn) {
      return node;
    }

    if (i < nodeRow || (i == nodeRow && j < nodeColumn))
      node = node->left;
    else {
      node = node->right;
    }
  }

  return nullptr;
}

// CHECK - NÃO PRECISA DE ADAPTAÇÃO PRA RUBRONEGRA
void inorderGet(TreeNode *root, bool transpose, std::vector<TreeNode *> &resultingTreeVec) {
  if (!root) {
    return;
  }

  inorderGet(root->left, transpose, resultingTreeVec);
  resultingTreeVec.push_back(root);
  inorderGet(root->right, transpose, resultingTreeVec);
}

TreeNode *sumMatrices(TreeNode *root_a, TreeNode *root_b, bool transpose_a, bool transpose_b) {
  std::vector<TreeNode *> a, b;

  inorderGet(root_a, transpose_a, a);
  inorderGet(root_b, transpose_b, b);

  TreeNode *result = nullptr;
  int i = 0, j = 0;

  while (i < (int) a.size() || j < (int) b.size()) {
    int a_row = 0, a_col = 0, a_value = 0;
    int b_row = 0, b_col = 0, b_value = 0;

    if (i < (int) a.size()) {
      a_row = transpose_a ? a[i]->column : a[i]->row;
      a_col = transpose_a ? a[i]->row : a[i]->column;
      a_value = a[i]->value;
    }

    if (j < (int) b.size()) {
      b_row = transpose_b ? b[j]->column : b[j]->row;
      b_col = transpose_b ? b[j]->row : b[j]->column;
      b_value = b[j]->value;
    }

    // Caso 1: tem coordenada em A e em B
    if (i < (int) a.size() && j < (int) b.size() && a_row == b_row && a_col == b_col) {
      int sum = a_value + b_value;
      if (sum != 0) {
        result = insert(result, a_row, a_col, sum);
      }
      i++;
      j++;
    } else if (j >= (int) b.size() || (i < (int) a.size() && (
                                         a_row < b_row || (a_row == b_row && (a_col < b_col))))) {
      // Caso 2: só tem a coordenada em A
      result = insert(result, a_row, a_col, a_value);
      i++;
    } else {
      // Caso 3: só tem na coordenada B:
      result = insert(result, b_row, b_col, b_value);
      j++;
    }
  }
  return result;
}

// NÃO PRECISA DE ADAPTAÇÃO PRA RUBRONEGRA
void multScalarMatrix(TreeNode *root, int multiplier) {
  if (!root) {
    return;
  }
  root->value = root->value * multiplier;
  multScalarMatrix(root->left, multiplier);
  multScalarMatrix(root->right, multiplier);
}

TreeNode *auxMultMatrices(TreeNode *node, int i, int j, int val) {
  TreeNode *n = findElement(node, i, j, false);

  if (n) {
    n->value += val;
    return node;
  }

  return insert(node, i, j, val);
}

// NÃO PRECISA DE ADAPTAÇÃO PRA RUBRONEGRA
// PRECISA IMPLEMENTAR MULTIPLICAÇÃO
TreeNode *multMatrices(TreeNode *root_a, TreeNode *root_b, bool transpose_a, bool transpose_b) {
  std::vector<TreeNode *> a, b;
  inorderGet(root_a, transpose_a, a);
  inorderGet(root_b, transpose_b, b);
  TreeNode *result = nullptr;

  for (TreeNode *ai: a) {
    int ai_row = transpose_a ? ai->column : ai->row;
    int ai_column = transpose_a ? ai->row : ai->column;
    int ai_value = ai->value;
    for (TreeNode *bj: b) {
      int bj_row = transpose_a ? bj->column : bj->row;
      int bj_col = transpose_a ? bj->row : bj->column;
      int bj_value = bj->value;

      if (ai_column == bj_row) {
        int val = ai_value * bj_value;
        if (val != 0) {
          result = auxMultMatrices(result, ai_row, bj_col, val);
        }
      }
    }
  }

  return result;
}

void printTree(TreeNode *root, bool transpose) {
  if (!root) {
    return;
  }
  printTree(root->left, transpose);
  int nodeRow = transpose ? root->column : root->row;
  int nodeColumn = transpose ? root->row : root->column;
  std::cout << "(" << nodeRow << ", " << nodeColumn << ") = " << root->value << std::endl;
  printTree(root->right, transpose);
}

TreeNode *generateSparseMatrix(int n, double sparsity) {
  TreeNode *root = nullptr;

  int nonZeros = (int) (n * n * sparsity);

  for (int k = 0; k < nonZeros; k++) {
    int i = rand() % n;
    int j = rand() % n;
    int val = (rand() % 9) + 1;

    root = insert(root, i, j, val);
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

  TreeNode *a = generateSparseMatrix(n, sparsity);
  TreeNode *b = generateSparseMatrix(n, sparsity);

  bool transpose_a = false;
  bool transpose_b = false;

  std::cout << "Matriz A:" << "\n";
  printTree(a, transpose_a);

  std::cout << "Matriz B:" << "\n";
  printTree(b, transpose_b);

  auto startMM = std::chrono::high_resolution_clock::now();
  SIZE_T MM_mem_start = getMemoryUseKB();

  TreeNode *ABMult = multMatrices(a, b, transpose_a, transpose_b);
  std::cout << "Resultado multiplicacao de matrizes" << "\n";
  printTree(ABMult, false);

  SIZE_T MM_mem_end = getMemoryUseKB();
  auto endMM = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> multMatricesTime = endMM - startMM;
  std::cout << "Multiplicacao de matrizes " << n << "x" << n << " com esparsidade " << sparsity * 100 << "%" << "\n";
  std::cout << "Tempo: " << multMatricesTime.count() << "s\n";
  std::cout << "Memória: " << MM_mem_end - MM_mem_start;
  // TreeNode* ABSum = sumMatrices(a, b, transpose_a, transpose_b);
  // std::cout << "Resultado soma matrizes:" << "\n";
  // printTree(ABSum, false);

  // int scalar = 5;
  // multScalarMatrix(a, scalar);
  // std::cout << "Resultado multiplicacao escalar:" << "\n";
  // printTree(a, transpose_a);
}
