cmake_minimum_required(VERSION 3.13)
project(EigenWasm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/extern/eigen) # or /eigen if you vendor headers

set(CMAKE_EXECUTABLE_SUFFIX ".js")

# Link flags for Emscripten
set(EMSCRIPTEN_LINK_FLAGS
  "-O3 -s WASM=1 -s MODULARIZE=1 -s EXPORT_NAME='createEigenModule' \
   -s EXPORTED_FUNCTIONS=[ \
     '_compute_eigenvalues_real_symmetric', \
     '_compute_eigenpairs_real_symmetric', \
     '_compute_eigenvalues_general', \
     '_compute_eigenpairs_general', \
     '_adjacency_to_normalized_laplacian_and_spectrum', \
     '_compute_top_k_eigenvalues_real_symmetric' \
   ] \
   -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap','allocate','getValue','setValue']"
)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${EMSCRIPTEN_LINK_FLAGS}")

add_executable(eigen_wasm src/wrapper.cpp)
