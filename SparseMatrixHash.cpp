#include "SparseMatrixHash.h"
#include <cassert>

SparseMatrixHash::SparseMatrixHash(const int n, const int m, const bool transposed)
  : n{n}, m{m}, transposed{transposed} {
}

SparseMatrixHash::SparseMatrixHash(const int n, const int m,
                                   const bool transposed,
                                   const std::unordered_map<std::pair<int, int>, double, PairHash> &d)
  : n{n}, m{m}, transposed{transposed}, data{d} {
}

double SparseMatrixHash::get(const int i, const int j) const {
  const auto k = key(i, j);
  const auto it = data.find(k);
  return it == data.end() ? 0.0 : it->second;
}

void SparseMatrixHash::set(const int i, const int j, const double value) {
  const auto k = key(i, j);
  if (value == 0.0)
    data.erase(k);
  else
    data[k] = value;
}

SparseMatrixHash SparseMatrixHash::transpose() const {
  return SparseMatrixHash(m, n, !transposed, {});
}

void SparseMatrixHash::transposeSelf() {
  transposed = !transposed;
}

std::vector<std::tuple<int, int, double> > SparseMatrixHash::items() const {
  std::vector<std::tuple<int, int, double> > items;
  items.reserve(data.size());

  for (const auto &[key, value]: data) {
    if (!transposed) {
      items.emplace_back(key.first, key.second, value);
    } else {
      items.emplace_back(key.second, key.first, value);
    }
  }

  return items;
}

SparseMatrixHash SparseMatrixHash::add(const SparseMatrixHash &B) const {
  assert(n == B.n && m == B.m);

  SparseMatrixHash C(n, m);
  C.data = this->data;

  for (auto [i, j, value]: B.items()) {
    std::pair key{i, j};
    C.data[key] += value;

    if (C.data[key] == 0.0) {
      C.data.erase(key);
    }
  }
  return C;
}

void SparseMatrixHash::addInPlace(const SparseMatrixHash &B) {
  assert(n == B.n && m == B.m);

  for (auto [i, j, value]: B.items()) {
    std::pair key{i, j};
    data[key] += value;

    if (data[key] == 0.0) {
      data.erase(key);
    }
  }
}

SparseMatrixHash &SparseMatrixHash::operator+=(const SparseMatrixHash &B) {
  addInPlace(B);
  return *this;
}

SparseMatrixHash SparseMatrixHash::scalarMult(const double alpha) const {
  if (alpha == 0.0) {
    return SparseMatrixHash(n, m);
  }

  SparseMatrixHash C(n, m, transposed, {});

  for (const auto [key, value]: data) {
    C.data[key] = alpha * value;
  }

  return C;
}

void SparseMatrixHash::scalarMultInPlace(const double alpha) {
  if (alpha == 0.0) {
    data.clear();
    return;
  }

  for (auto &[_, value]: data) {
    value *= alpha;
  }
}

SparseMatrixHash &SparseMatrixHash::operator*=(const double alpha) {
  scalarMultInPlace(alpha);
  return *this;
}

SparseMatrixHash SparseMatrixHash::mult(const SparseMatrixHash &B) const {
  assert(m == B.n);

  SparseMatrixHash C(n, B.m);

  std::unordered_map<int, std::vector<std::pair<int, double> > > Brow;

  for (auto [i, j, value]: B.items()) {
    Brow[i].emplace_back(j, value);
  }

  for (auto [i, k, aValue]: this->items()) {
    if (!Brow.count(k)) continue;

    for (auto &[j, bValue]: Brow[k]) {
      const double old = C.get(i, j);
      C.set(i, j, old + aValue * bValue);
    }
  }

  return C;
}

std::ostream &operator<<(std::ostream &os, const SparseMatrixHash &M) {
  os << "SparseMatrixHash(" << M.n << "x" << M.m
      << ", nnz=" << M.data.size()
      << ", transposed=" << (M.transposed ? "true" : "false") << ")";
  return os;
}
