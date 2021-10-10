#include <benchmark/benchmark.h>

static void system_malloc(benchmark::State &state) {
  const int num_allocs = state.range(0);
  const int alloc_size = state.range(1);
  std::vector<void *> allocs(num_allocs);
  for (auto _ : state) {

    for (int i = 0; i < num_allocs; i++) {
      allocs[i] = malloc(alloc_size);
    }
    benchmark::ClobberMemory();

    state.PauseTiming();
    for (auto ptr : allocs) {
      free(ptr);
    }
    benchmark::ClobberMemory();
    state.ResumeTiming();
  }
  state.counters["PerAlloc"] = benchmark::Counter(
      num_allocs * state.iterations(),
      benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
}

BENCHMARK(system_malloc)->Args({100, 17})->Args({1000, 17})->Args({10000, 17});
BENCHMARK(system_malloc)->Args({100, 55})->Args({1000, 55})->Args({10000, 55});
BENCHMARK(system_malloc)
    ->Args({100, 259})
    ->Args({1000, 259})
    ->Args({10000, 259});

static void system_aligned_alloc(benchmark::State &state) {
  const int num_allocs = state.range(0);
  const int alloc_size = state.range(1);
  const int alignment = state.range(1);
  std::vector<void *> allocs(num_allocs);
  for (auto _ : state) {

    for (int i = 0; i < num_allocs; i++) {
      allocs[i] = aligned_alloc(alignment, alloc_size);
    }
    benchmark::ClobberMemory();
    state.PauseTiming();
    for (auto ptr : allocs) {
      free(ptr);
    }
    benchmark::ClobberMemory();
    state.ResumeTiming();
  }
  state.counters["PerAlloc"] = benchmark::Counter(
      num_allocs * state.iterations(),
      benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
}

BENCHMARK(system_aligned_alloc)
    ->Args({100, 17, 32})
    ->Args({1000, 17, 32})
    ->Args({10000, 17, 32});
BENCHMARK(system_aligned_alloc)
    ->Args({100, 55, 32})
    ->Args({1000, 55, 32})
    ->Args({10000, 55, 32});
BENCHMARK(system_aligned_alloc)
    ->Args({100, 259, 32})
    ->Args({1000, 259, 32})
    ->Args({10000, 259, 32});

static void system_free(benchmark::State &state) {
  const int num_allocs = state.range(0);
  const int alloc_size = state.range(1);
  std::vector<void *> allocs(num_allocs);
  for (auto _ : state) {
    state.PauseTiming();
    for (int i = 0; i < num_allocs; i++) {
      allocs[i] = malloc(alloc_size);
    }
    benchmark::ClobberMemory();
    state.ResumeTiming();

    for (auto ptr : allocs) {
      free(ptr);
    }
    benchmark::ClobberMemory();
  }
  state.counters["PerAlloc"] = benchmark::Counter(
      num_allocs * state.iterations(),
      benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
}

BENCHMARK(system_free)->Args({100, 17})->Args({1000, 17})->Args({10000, 17});
BENCHMARK(system_free)->Args({100, 55})->Args({1000, 55})->Args({10000, 55});
BENCHMARK(system_free)->Args({100, 259})->Args({1000, 259})->Args({10000, 259});