#include <benchmark/benchmark.h>

#include "include/memory.h"

static void system_malloc(benchmark::State &state) {
  const int num_allocs = state.range(0);
  const int alloc_size = state.range(1);
  std::vector<void *> allocs(num_allocs);
  for (auto _ : state) {

    for (int i = 0; i < num_allocs; i++) {
      allocs[i] = malloc(alloc_size);
    }

    state.PauseTiming();
    for (auto ptr : allocs) {
      free(ptr);
    }
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

    state.PauseTiming();
    for (auto ptr : allocs) {
      free(ptr);
    }
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

static void memory_stack_allocator(benchmark::State &state) {
  const int num_allocs = state.range(0);
  const int alloc_size = state.range(1);
  const int alignment = state.range(1);
  using namespace fastware::memory;

  stack_alloc_create_info_t create_info{
      nullptr,
      num_allocs *
          align(alloc_size, static_cast<alignment_t::value>(alignment)),
      static_cast<alignment_t::value>(alignment)};
  allocator_t *alloc = create(&create_info);

  for (auto _ : state) {

    for (int i = 0; i < num_allocs; i++) {
      auto blk = allocate(alloc, alloc_size);
      benchmark::DoNotOptimize(blk);
    }

    state.PauseTiming();
    deallocate_all(alloc);
    state.ResumeTiming();
  }

  destroy(alloc);

  state.counters["PerAlloc"] = benchmark::Counter(
      num_allocs * state.iterations(),
      benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
}

BENCHMARK(memory_stack_allocator)
    ->Args({100, 17, 32})
    ->Args({1000, 17, 32})
    ->Args({10000, 17, 32});
BENCHMARK(memory_stack_allocator)
    ->Args({100, 55, 32})
    ->Args({1000, 55, 32})
    ->Args({10000, 55, 32});
BENCHMARK(memory_stack_allocator)
    ->Args({100, 259, 32})
    ->Args({1000, 259, 32})
    ->Args({10000, 259, 32});

BENCHMARK_MAIN();