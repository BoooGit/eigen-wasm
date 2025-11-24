#include <emscripten/emscripten.h>
#include <Eigen/Dense>
#include <vector>
#include <cstdint>
#include <cmath>

using Eigen::MatrixXd;
using Eigen::Map;
using Eigen::RowMajor;
using Eigen::VectorXd;

extern "C" {

// 1) symmetric eigenvalues
EMSCRIPTEN_KEEPALIVE
void compute_eigenvalues_real_symmetric(const double* A, int n, double* out_evals) {
    Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, RowMajor>> M(A, n, n);
    Eigen::SelfAdjointEigenSolver<MatrixXd> es(M);
    if (es.info() != Eigen::Success) {
        for (int i=0;i<n;i++) out_evals[i] = NAN;
        return;
    }
    auto vals = es.eigenvalues();
    for (int i=0;i<n;i++) out_evals[i] = vals(i);
}

// 2) symmetric eigenpairs
EMSCRIPTEN_KEEPALIVE
void compute_eigenpairs_real_symmetric(const double* A, int n, double* out_evals, double* out_evecs) {
    Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, RowMajor>> M(A, n, n);
    Eigen::SelfAdjointEigenSolver<MatrixXd> es(M);
    if (es.info() != Eigen::Success) {
        for (int i=0;i<n;i++) out_evals[i] = NAN;
        return;
    }
    auto vals = es.eigenvalues();
    auto vecs = es.eigenvectors(); // columns are eigenvectors
    for (int i=0;i<n;i++) out_evals[i] = vals(i);
    // write eigenvectors into out_evecs row-major as rows contiguous
    for (int r=0;r<n;r++) {
        for (int c=0;c<n;c++) {
            out_evecs[r*n + c] = vecs(r,c); // vecs is column-major, but indexing (r,c) works
        }
    }
}

// 3) general eigenvalues (real + imag)
EMSCRIPTEN_KEEPALIVE
void compute_eigenvalues_general(const double* A, int n, double* out_real, double* out_imag) {
    Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, RowMajor>> M(A, n, n);
    Eigen::EigenSolver<MatrixXd> es(M, /* computeEigenvectors = */ false);
    if (es.info() != Eigen::Success) {
        for (int i=0;i<n;i++) { out_real[i]=NAN; out_imag[i]=NAN; }
        return;
    }
    auto vals = es.eigenvalues();
    for (int i=0;i<n;i++) {
        out_real[i] = vals(i).real();
        out_imag[i] = vals(i).imag();
    }
}

// 4) general eigenpairs (real + imag)
EMSCRIPTEN_KEEPALIVE
void compute_eigenpairs_general(const double* A, int n, double* out_real, double* out_imag, double* out_evecs_real, double* out_evecs_imag) {
    Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, RowMajor>> M(A, n, n);
    Eigen::EigenSolver<MatrixXd> es(M, /* computeEigenvectors = */ true);
    if (es.info() != Eigen::Success) {
        for (int i=0;i<n;i++) { out_real[i]=NAN; out_imag[i]=NAN; }
        return;
    }
    auto vals = es.eigenvalues();
	auto vecs = es.eigenvectors(); // columns are eigenvectors
    for (int i=0;i<n;i++) {
        out_real[i] = vals(i).real();
        out_imag[i] = vals(i).imag();
    // write eigenvectors into out_evecs row-major as rows contiguous
    for (int r=0;r<n;r++) {
        for (int c=0;c<n;c++) {
            out_evecs_real[r*n + c] = vecs(r,c).real(); // vecs is column-major, but indexing (r,c) works
			out_evecs_imag[r*n + c] = vecs(r,c).imag(); // vecs is column-major, but indexing (r,c) works
        }		
}

// 5) adjacency -> normalized laplacian -> spectrum
EMSCRIPTEN_KEEPALIVE
void adjacency_to_normalized_laplacian_and_spectrum(const uint8_t* adj, int n, double* out_evals) {
    MatrixXd A = MatrixXd::Zero(n,n);
    for (int i=0;i<n;i++) for (int j=0;j<n;j++) A(i,j) = adj[i*n + j] ? 1.0 : 0.0;
    VectorXd deg = A.rowwise().sum();
    MatrixXd DinvSqrt = MatrixXd::Zero(n,n);
    for (int i=0;i<n;i++) {
        if (deg(i) > 0) DinvSqrt(i,i) = 1.0 / std::sqrt(deg(i));
    }
    MatrixXd L = MatrixXd::Identity(n,n) - DinvSqrt * A * DinvSqrt;
    Eigen::SelfAdjointEigenSolver<MatrixXd> es(L);
    if (es.info() != Eigen::Success) {
        for (int i=0;i<n;i++) out_evals[i] = NAN;
        return;
    }
    auto vals = es.eigenvalues();
    for (int i=0;i<n;i++) out_evals[i] = vals(i);
}

// 6) top-k (simple)
EMSCRIPTEN_KEEPALIVE
int compute_top_k_eigenvalues_real_symmetric(const double* A, int n, int k, double* out_k_evals) {
    if (k <= 0 || k > n) return -1;
    Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, RowMajor>> M(A, n, n);
    Eigen::SelfAdjointEigenSolver<MatrixXd> es(M);
    if (es.info() != Eigen::Success) return -2;
    auto vals = es.eigenvalues();
    // largest k are last k entries of ascending-sorted vals
    for (int i=0;i<k;i++) out_k_evals[i] = vals(n - k + i);
    return k;
}

} // extern "C"
