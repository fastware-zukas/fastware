#include <benchmark/benchmark.h>

static void memroy_start(benchmark::State &state) {
  for (auto _ : state) {
  }
}

BENCHMARK(memroy_start);

BENCHMARK_MAIN();