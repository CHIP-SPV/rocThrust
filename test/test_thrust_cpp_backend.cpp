// Reproducer for https://github.com/CHIP-SPV/chipStar/issues/1363
//
// Building any Thrust code with -DTHRUST_DEVICE_SYSTEM=THRUST_DEVICE_SYSTEM_CPP
// (the host-only fallback backend) must compile and run correctly. Before the
// fix this failed to compile with:
//   thrust/detail/type_traits.h: 'invoke_result_t' had no CPP-system branch
//   thrust/detail/type_traits/result_of_adaptable_function.h: no 'invoke_result_t'
//   thrust/system/detail/sequential/trivial_copy.h: undeclared 'NV_IS_HOST'
//
// This test is compiled with THRUST_DEVICE_SYSTEM=THRUST_DEVICE_SYSTEM_CPP
// (see test/CMakeLists.txt) and runs entirely on the host.

#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <thrust/reduce.h>
#include <thrust/sort.h>
#include <thrust/transform.h>
#include <thrust/transform_reduce.h>

#include <cassert>
#include <cstdio>

struct square
{
  __host__ __device__ int operator()(int x) const { return x * x; }
};

int main()
{
  const int n = 100;

  thrust::host_vector<int> h(n);
  for (int i = 0; i < n; ++i)
    h[i] = i + 1;

  // With the CPP device system, device_vector lives in host memory.
  thrust::device_vector<int> d = h;

  // reduce: 1 + 2 + ... + 100 = 5050
  const int sum = thrust::reduce(d.begin(), d.end(), 0);
  assert(sum == 5050);

  // transform: element-wise square
  thrust::device_vector<int> squared(n);
  thrust::transform(d.begin(), d.end(), squared.begin(), square());
  const thrust::host_vector<int> h_squared = squared;
  for (int i = 0; i < n; ++i)
    assert(h_squared[i] == (i + 1) * (i + 1));

  // transform_reduce: sum of squares = n(n+1)(2n+1)/6 = 338350
  const int sum_sq = thrust::transform_reduce(d.begin(), d.end(), square(), 0,
                                              thrust::plus<int>());
  assert(sum_sq == 338350);

  // sort a reversed copy
  thrust::device_vector<int> rev(n);
  for (int i = 0; i < n; ++i)
    rev[i] = n - i;
  thrust::sort(rev.begin(), rev.end());
  const thrust::host_vector<int> h_sorted = rev;
  for (int i = 0; i < n; ++i)
    assert(h_sorted[i] == i + 1);

  std::printf("PASS\n");
  return 0;
}
