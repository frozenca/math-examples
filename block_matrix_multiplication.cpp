#include <cassert>
#include <vector>
#include <random>
#include <chrono>
#include <iostream>

std::mt19937 gen(std::random_device{}());

template <typename T, size_t M, size_t N>
class Matrix {
public:
    Matrix() : rows {M}, cols {N} {
        elems.resize(M * N);
    }

    Matrix& operator+=(const Matrix& rhs);
    T& operator()(size_t i, size_t j);
    const T& operator() (size_t i, size_t j) const;

    size_t rows;
    size_t cols;
    std::vector<T> elems;
};

template <typename T, size_t M, size_t N>
Matrix<T, M, N>& Matrix<T, M, N>::operator+=(const Matrix<T, M, N>& rhs) {
    for (size_t i = 0; i < elems.size(); i++) {
        elems[i] += rhs.elems[i];
    }
    return *this;
}

template <typename T, size_t M, size_t N>
T& Matrix<T, M, N>::operator()(size_t i, size_t j) {
    // assert(i < M && j < N);
    return elems[i * N + j];
}

template <typename T, size_t M, size_t N>
const T& Matrix<T, M, N>::operator()(size_t i, size_t j) const {
    // assert(i < M && j < N);
    return elems[i * N + j];
}

template <typename T, size_t M, size_t N>
Matrix<T, M, N> operator+(const Matrix<T, M, N>& A, const Matrix<T, M, N>& B) {
    Matrix<T, M, N> C = A;
    C += B;
    return C;
}

template <typename T, size_t M, size_t N, size_t K>
Matrix<T, M, N> operator*(const Matrix<T, M, K>& A, const Matrix<T, K, N>& B) {
    Matrix<T, M, N> C;
    for (size_t i = 0; i < M; i++) {
        for (size_t j = 0; j < N; j++) {
            for (size_t k = 0; k < K; k++) {
                C(i, j) += A(i, k) * B(k, j);
            }
        }
    }
    return C;
}

int main() {
    std::uniform_real_distribution<> dis(-10000.0, 10000.0);

    constexpr size_t M = 100;
    constexpr size_t N = 100;
    constexpr size_t block = 10;

    Matrix<double, block * M, block * N> A, B;
    for (auto& elem : A.elems) {
        elem = dis(gen);
    }
    for (auto& elem : B.elems) {
        elem = dis(gen);
    }
    auto start = std::chrono::steady_clock::now();
    auto C = A * B;
    auto end = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time to multiply " << block * M << " x " << block * N
              << " matrices : " << diff.count() << "ms\n";

    Matrix<Matrix<double, block, block>, M, N> A2, B2;
    for (auto& A2block : A2.elems) {
        for (auto& A2elem : A2block.elems) {
            A2elem = dis(gen);
        }
    }
    for (auto& B2block : B2.elems) {
        for (auto& B2elem : B2block.elems) {
            B2elem = dis(gen);
        }
    }
    start = std::chrono::steady_clock::now();
    auto C2 = A2 * B2;
    end = std::chrono::steady_clock::now();
    diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time to multiply " << block * M << " x " << block * N
    << " matrices in " << block << " x " << block << ": " << diff.count() << "ms\n";

}
