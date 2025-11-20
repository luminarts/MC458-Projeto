#ifndef MC458_PROJETO_SPARSEMATRIXHASH_H
#define MC458_PROJETO_SPARSEMATRIXHASH_H

#include <unordered_map>
#include <vector>
#include <tuple>
#include <iostream>
#include <utility>

struct PairHash {
  size_t operator()(const std::pair<int, int> &p) const {
    return std::hash<long long>()(
      (static_cast<long long>(p.first) << 32) ^ p.second
    );
  }
};

class SparseMatrixHash {
  int n, m;
  bool transposed;
  std::unordered_map<std::pair<int, int>, double, PairHash> data;

  std::pair<int, int> key(int i, int j) const {
    return transposed ? std::pair{j, i} : std::pair{i, j};
  }

public:
  SparseMatrixHash(int n, int m, bool transposed = false);

  SparseMatrixHash(int n, int m,
                   bool transposed,
                   const std::unordered_map<std::pair<int, int>, double, PairHash> &d);

  double get(int i, int j) const;

  void set(int i, int j, double value);

  SparseMatrixHash transpose() const;

  void transposeSelf();

  std::vector<std::tuple<int, int, double> > items() const;

  SparseMatrixHash add(const SparseMatrixHash &B) const;

  void addInPlace(const SparseMatrixHash &B);

  SparseMatrixHash &operator+=(const SparseMatrixHash &B);

  SparseMatrixHash scalarMult(double alpha) const;

  void scalarMultInPlace(double alpha);

  SparseMatrixHash &operator*=(double alpha);

  SparseMatrixHash mult(const SparseMatrixHash &B) const;

  friend std::ostream &operator<<(std::ostream &os, const SparseMatrixHash &M);
};

#endif //MC458_PROJETO_SPARSEMATRIXHASH_H
