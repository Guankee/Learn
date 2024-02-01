#include <chrono>
#include <iostream>
#include <thread>

#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#define N (50)
__global__ void add(int* a, int* b, int* c) {
  int tid = threadIdx.x + blockDim.x * blockIdx.x;
  while (tid < N) {
    clock_t start_time = clock();
    while ((clock() - start_time) < 100) {
    }
    c[tid] = a[tid] + b[tid];

    tid += gridDim.x + blockDim.x;
  }
}
int main() {
  auto st = std::chrono::system_clock::now();
  int a[N], b[N], c[N];
  int *dev_a, *dev_b, *dev_c;
  cudaMalloc((void**)&dev_a, N * sizeof(int));
  cudaMalloc((void**)&dev_b, N * sizeof(int));
  cudaMalloc((void**)&dev_c, N * sizeof(int));
  for (int i = 0; i < N; ++i) {
    a[i] = -i;
    b[i] = i * i;
  }
  cudaMemcpy(dev_a, a, N * sizeof(int), cudaMemcpyHostToDevice);
  cudaMemcpy(dev_b, b, N * sizeof(int), cudaMemcpyHostToDevice);

  add<<<128, 128>>>(dev_a, dev_b, dev_c);
  cudaMemcpy(c, dev_c, N * sizeof(int), cudaMemcpyDeviceToHost);
  for (int i = 0; i < N; i++) {
    std::cout << c[i] << " ";
  }
  std::cout << std::endl;
  cudaFree(dev_a);
  cudaFree(dev_b);
  cudaFree(dev_c);
  auto end = std::chrono::system_clock::now();
  std::cout
      << std::chrono::duration_cast<std::chrono::milliseconds>(end - st).count()
      << "ms" << std::endl;
  return 0;
}