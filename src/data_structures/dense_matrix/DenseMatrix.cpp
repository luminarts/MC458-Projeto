#include "DenseMatrix.h"
#include <vector>

DenseMatrix::DenseMatrix(const int n, const int m)
  : n(n), m(m), data(n, std::vector(m, 0.0)) {
}

void DenseMatrix::set(const int i, const int j, const double value) {
  data[i][j] = value;
}

double DenseMatrix::get(const int i, const int j) const {
  return data[i][j];
}

DenseMatrix DenseMatrix::add(const DenseMatrix &B) const {
  DenseMatrix C(n, m);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      C.data[i][j] = data[i][j] + B.data[i][j];
    }
  }

  return C;
}

DenseMatrix DenseMatrix::operator+(const DenseMatrix &B) const {
  return add(B);
}

DenseMatrix DenseMatrix::scalarMult(const double alpha) const {
  DenseMatrix C(n, m);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      C.data[i][j] = alpha * data[i][j];
    }
  }

  return C;
}

DenseMatrix DenseMatrix::operator*(const double alpha) const {
  return scalarMult(alpha);
}

DenseMatrix DenseMatrix::mult(const DenseMatrix &B) const {
  DenseMatrix C(n, B.m);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < B.m; j++) {
      for (int k = 0; k < m; k++) {
        C.data[i][j] += data[i][k] * B.data[k][j];
      }
    }
  }

  return C;
}

DenseMatrix DenseMatrix::operator*(const DenseMatrix &B) const {
  return mult(B);
}

DenseMatrix DenseMatrix::transpose() const {
  DenseMatrix C(m, n);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      C.data[j][i] = data[i][j];
    }
  }

  return C;
}
