import createEigenModule from './eigen.js'; // produced by Emscripten

export async function loadEigen() {
  const Module = await createEigenModule();
  return {
    computeEigenRealSymmetric: async (A_flat, n) => {
      const bytes = 8 * n * n;
      const aPtr = Module._malloc(bytes);
      const arr = new Float64Array(Module.HEAPF64.buffer, aPtr, n*n);
      arr.set(A_flat);
      const outPtr = Module._malloc(8 * n);
      Module._compute_eigenvalues_real_symmetric(aPtr, n, outPtr);
      const out = Array.from(new Float64Array(Module.HEAPF64.buffer, outPtr, n));
      Module._free(aPtr);
      Module._free(outPtr);
      return out;
    },
    // add wrappers for other functions as needed
  };
}
