//
// Created by victo on 20/11/2025.
//

#ifndef MC458_PROJETO_DENSEMATRIX_H
#define MC458_PROJETO_DENSEMATRIX_H
#include <vector>

class DenseMatrix {
  int n, m;
  std::vector<std::vector<double> > data;

public:
  DenseMatrix(int n, int m);

  void set(int i, int j, double value);

  double get(int i, int j) const;

  DenseMatrix add(const DenseMatrix &B) const;

  DenseMatrix operator+(const DenseMatrix &B) const;

  DenseMatrix scalarMult(double alpha) const;

  DenseMatrix operator*(double alpha) const;

  DenseMatrix mult(const DenseMatrix &B) const;

  DenseMatrix operator*(const DenseMatrix &B) const;

  DenseMatrix transpose() const;
};

#endif //MC458_PROJETO_DENSEMATRIX_H
