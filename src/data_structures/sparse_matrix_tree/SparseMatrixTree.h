#ifndef MC458_PROJETO_SPARSEMATRIXTREE_H
#define MC458_PROJETO_SPARSEMATRIXTREE_H
#include <vector>

class SparseMatrixTree {
  enum Color { RED, BLACK };

public:
  struct TreeNode {
    int value;
    int row, column;
    Color color;
    TreeNode *left, *right, *parent;

    TreeNode(int v = 0, int rw = 0, int col = 0, Color clr = BLACK,
             TreeNode *l = nullptr, TreeNode *r = nullptr, TreeNode *p = nullptr);

    ~TreeNode();
  };

  // Main operations
  static TreeNode *insert(TreeNode *root, int i, int j, int valueToInsert);

  static TreeNode *findElement(TreeNode *node, int i, int j, bool transpose);

  static void inorderGet(TreeNode *root, bool transpose, std::vector<TreeNode *> &resultingTreeVec);

  // Matrix operations
  static TreeNode *sumMatrices(TreeNode *root_a, TreeNode *root_b, bool transpose_a, bool transpose_b);

  static void multScalarMatrix(TreeNode *root, int multiplier);

  static TreeNode *multMatrices(TreeNode *root_a, TreeNode *root_b, bool transpose_a, bool transpose_b);

  // Utility
  static void printTree(const TreeNode *root, bool transpose);

private:
  // Helper functions
  static bool isLessThan(int i1, int j1, int i2, int j2);

  static bool isRed(const TreeNode *node);

  static bool isBlack(const TreeNode *node);

  static TreeNode *rotateLeft(TreeNode *root);

  static TreeNode *rotateRight(TreeNode *root);

  static void riseRed(TreeNode *root);

  static TreeNode *insertRBTree(TreeNode *root, int i, int j, int valueToInsert);

  static TreeNode *auxMultMatrices(TreeNode *node, int i, int j, int val);
};
#endif //MC458_PROJETO_SPARSEMATRIXTREE_H
