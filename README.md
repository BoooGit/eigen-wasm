# eigen-wasm — exported API (matrix spectra)

This repository produces `eigen.wasm` + `eigen.js` (Emscripten) exposing a small, focused API for matrix spectra.

## Available exported functions (C ABI)
All functions are `extern "C"` and use row-major order for matrices.

1. compute_eigenvalues_real_symmetric(const double* A, int n, double* out_evals)
   - Computes eigenvalues of symmetric real matrix A (size n×n).
   - Writes n doubles to out_evals (ascending order).

2. compute_eigenpairs_real_symmetric(const double* A, int n, double* out_evals, double* out_evecs)
   - Computes eigenvalues and eigenvectors.
   - out_evecs is n×n doubles (row-major): row i contains row i of eigenvector matrix.

3. compute_eigenvalues_general(const double* A, int n, double* out_real, double* out_imag)
   - For general real matrices; returns arrays of real and imaginary parts.

4. compute_eigenpairs_general(const double* A, int n, double* out_real, double* out_imag, double* out_vecs_real, double* out_vecs_imag)
   - Full eigenpairs for general matrices (real/imag parts for eigenvalues + eigenvectors).

5. adjacency_to_normalized_laplacian_and_spectrum(const uint8_t* adjacency, int n, double* out_evals)
   - Accepts adjacency matrix as bytes (0/1), returns spectrum of normalized Laplacian (n eigenvalues).

6. compute_top_k_eigenvalues_real_symmetric(const double* A, int n, int k, double* out_k_evals)
   - Returns the k largest eigenvalues (simple full-solver approach).
   - Returns k on success, negative on error.

## Memory model
- Caller is responsible for allocating/freeing memory in WASM heap (use Module._malloc / Module._free).
- Arrays are row-major and use IEEE 64-bit doubles for numeric arrays. Adjacency may be passed as uint8 bytes.

## Build
- `CMakeLists.txt` at repository root; built with emcmake/emmake in CI to produce `eigen.js` + `eigen.wasm`.

## Notes
- Only Eigen code reachable from exported functions is compiled into WASM — binary size remains small.
- To add new features: update `src/wrapper.cpp`, push changes; CI rebuilds new `eigen.wasm` + `eigen.js`.
