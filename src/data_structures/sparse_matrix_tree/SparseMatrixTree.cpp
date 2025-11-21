#include "SparseMatrixTree.h"

#include <iostream>

// Estrutura 2: Árvore binária com cada nó tendo o número em si e a sua posição numa matriz

// CHECK FINAL
// (Color enum is in header)

/// @brief Constructor para o nó da árvore
/// @param v valor associado à coordenada
/// @param rw linha
/// @param col coluna
/// @param clr cor vermelha ou preta para arvore rubronegra
/// @param l filho esquerdo
/// @param r filho direito
/// @param p pai
SparseMatrixTree::TreeNode::TreeNode(int v, int rw, int col, Color clr, TreeNode *l, TreeNode *r, TreeNode *p)
  : value(v), row(rw), column(col), color(clr), left(l), right(r), parent(p) {
}

/// @brief Destructor da memória
SparseMatrixTree::TreeNode::~TreeNode() {
  delete(left);
  delete(right);
}

/// @brief Função auxiliar que realiza comparações de nós a partir de coordenadas da matriz
/// @param i1 linha do elemento 1
/// @param j1 coluna do elemento 1
/// @param i2 linha dp e 2
/// @param j2 coluna da matriz 2
/// @return verdadeiro, se o elemento da
bool SparseMatrixTree::isLessThan(int i1, int j1, int i2, int j2) {
  if (i1 < i2) {
    return true;
  } else if (i1 == i2) {
    return j1 < j2;
  }
  return false;
}


/// @brief Função que verifica se um nó é vermelho
/// @param node nó a ser verificado
/// @return verdadeiro se for vermelho, falso caso contrário
bool SparseMatrixTree::isRed(const TreeNode *node) {
  if (node == nullptr) {
    return false;
  }
  return node->color == RED;
}

/// @brief Função que verifica se um nó é preto
/// @param node nó a ser verificado
/// @return verdadeiro se for preto, falso caso contrário
bool SparseMatrixTree::isBlack(const TreeNode *node) {
  if (node == nullptr) {
    return true;
  }

  return node->color == BLACK;
}

/// @brief Função de rotação para esquerda da árvore para balanceamento da árvore rubronegra
/// @param root nó raíz da rotação
/// @return árvore rotacionada
SparseMatrixTree::TreeNode *SparseMatrixTree::rotateLeft(TreeNode *root) {
  TreeNode *node = root->right;
  root->right = node->left;
  node->left = root;
  node->color = root->color;
  root->color = RED;
  return node;
}

/// @brief Função de rotação para direita da árvore para balanceamento da árvore rubronegra
/// @param root nó raíz da rotação
/// @return árvore rotacionada
SparseMatrixTree::TreeNode *SparseMatrixTree::rotateRight(TreeNode *root) {
  TreeNode *node = root->left;
  root->left = node->right;
  node->right = root;
  node->color = root->color;
  root->color = RED;
  return node;
}

/// @brief Função auxiliar da árvore rubronegra que sobe a cor vermelha para o nó pai e desce a cor preta para nós filhos
/// @param root nó raíz da subida de cor
void SparseMatrixTree::riseRed(TreeNode *root) {
  root->color = RED;
  root->left->color = BLACK;
  root->right->color = BLACK;
}

/// @brief Função de inserção na árvore rubronegra
/// @param root nó atual
/// @param i valor de linha para o novo nó a ser inserido
/// @param j valor de coluna para o novo nó a ser inserido
/// @param valueToInsert valor do dado para o novo nó a ser inserido
/// @return árvore com novo nó inserido ou atualizado
SparseMatrixTree::TreeNode *SparseMatrixTree::insertRBTree(TreeNode *root, int i, int j, int valueToInsert) {
  if (root == nullptr) {
    TreeNode *node = nullptr;
    node = new TreeNode(valueToInsert, i, j, RED);
    return node;
  }

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


/// @brief Função wrapper para inserção da árvore rubronegra
/// @param root nó raiz
/// @param i valor de linha do novo nó
/// @param j valor de coluna do novo nó
/// @param valueToInsert valor do novo nó
/// @return árvore com o novo nó inserido
SparseMatrixTree::TreeNode *SparseMatrixTree::insert(TreeNode *root, int i, int j, int valueToInsert) {
  root = insertRBTree(root, i, j, valueToInsert);
  root->color = BLACK;
  return root;
}

/// @brief Função que procura se uma posição da matriz possui nó na árvore
/// @param node nó investigado
/// @param i valor de linha procurado
/// @param j valor de coluna procurado
/// @param transpose flag para identificar se a matriz que a árvore representa é a sua transposta
/// @return nó procurado ou nulo
SparseMatrixTree::TreeNode *SparseMatrixTree::findElement(TreeNode *node, int i, int j, bool transpose) {
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

/// @brief Função auxiliar da soma de matrizes que faz o percurso inorder da árvore e coloca em um vetor
/// @param root
/// @param transpose
/// @param resultingTreeVec
void SparseMatrixTree::inorderGet(TreeNode *root, bool transpose, std::vector<TreeNode *> &resultingTreeVec) {
  if (!root) {
    return;
  }

  inorderGet(root->left, transpose, resultingTreeVec);
  resultingTreeVec.push_back(root);
  inorderGet(root->right, transpose, resultingTreeVec);
}

/// @brief Função que realiza soma de duas matrizes representadas por árvores rubronegras
/// @param root_a nó raiz da matriz A
/// @param root_b nó raiz da matriz B
/// @param transpose_a flag que identifica se matriz A é transposta
/// @param transpose_b flag que identifica se matriz B é transposta
/// @return árvore da matriz resultante
SparseMatrixTree::TreeNode *SparseMatrixTree::sumMatrices(TreeNode *root_a, TreeNode *root_b, bool transpose_a,
                                                          bool transpose_b) {
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

/// @brief Função que multiplica os valores de uma matriz na árvore por um escalar
/// @param root nó raíz da árvore em questão
/// @param multiplier escalar que fará a multiplicação
void SparseMatrixTree::multScalarMatrix(TreeNode *root, int multiplier) {
  if (!root) {
    return;
  }
  root->value = root->value * multiplier;
  multScalarMatrix(root->left, multiplier);
  multScalarMatrix(root->right, multiplier);
}

/// @brief Função auxiliar da multiplicação de matrizes que realiza a soma parcial dos valores de uma operação de linha x coluna
/// @param node nó em questão
/// @param i linha
/// @param j coluna
/// @param val valor a ser somado
/// @return nó com a soma atualizada
SparseMatrixTree::TreeNode *SparseMatrixTree::auxMultMatrices(TreeNode *node, int i, int j, int val) {
  if (TreeNode *n = findElement(node, i, j, false)) {
    n->value += val;
    return node;
  }

  return insert(node, i, j, val);
}

/// @brief Função que realiza multiplicação de matrizes
/// @param root_a nó raíz da matriz A
/// @param root_b nó raiz da matriz B
/// @param transpose_a flag para transposição
/// @param transpose_b flag para transposição
/// @return árvore resultante do resultado da operações
SparseMatrixTree::TreeNode *SparseMatrixTree::multMatrices(TreeNode *root_a, TreeNode *root_b, bool transpose_a,
                                                           bool transpose_b) {
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

/// @brief Função auxiliar para imprimir os valores da matriz de forma inorder
/// @param root nó raíz
/// @param transpose flag de tranposição da matriz em questão
void SparseMatrixTree::printTree(const TreeNode *root, bool transpose) {
  if (!root) {
    return;
  }
  printTree(root->left, transpose);
  int nodeRow = transpose ? root->column : root->row;
  int nodeColumn = transpose ? root->row : root->column;
  std::cout << "(" << nodeRow << ", " << nodeColumn << ") = " << root->value << std::endl;
  printTree(root->right, transpose);
}
