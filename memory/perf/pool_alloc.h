#include <benchmark/benchmark.h>

#include "include/memory.h"

static void memory_pool_allocator_alloc(benchmark::State &state) {
  const int num_allocs = state.range(0);
  const int alloc_size = state.range(1);
  const int alignment = state.range(1);
  using namespace fastware::memory;

  pool_alloc_create_info_t create_info{
      nullptr, align(alloc_size, static_cast<alignment_t::value>(alignment)),
      static_cast<alignment_t::value>(alignment),
      static_cast<uint64_t>(num_allocs)};
  allocator_t *alloc = create(&create_info);

  for (auto _ : state) {

    for (int i = 0; i < num_allocs; i++) {
      auto blk = allocate(alloc, alloc_size);
      benchmark::DoNotOptimize(blk);
    }
    benchmark::ClobberMemory();

    state.PauseTiming();
    deallocate_all(alloc);
    benchmark::ClobberMemory();
    state.ResumeTiming();
  }

  destroy(alloc);

  state.counters["PerAlloc"] = benchmark::Counter(
      num_allocs * state.iterations(),
      benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
}

BENCHMARK(memory_pool_allocator_alloc)
    ->Args({100, 17, 32})
    ->Args({1000, 17, 32})
    ->Args({10000, 17, 32});
BENCHMARK(memory_pool_allocator_alloc)
    ->Args({100, 55, 32})
    ->Args({1000, 55, 32})
    ->Args({10000, 55, 32});
BENCHMARK(memory_pool_allocator_alloc)
    ->Args({100, 259, 32})
    ->Args({1000, 259, 32})
    ->Args({10000, 259, 32});

static void memory_pool_allocator_dealloc(benchmark::State &state) {
  const int num_allocs = state.range(0);
  const int alloc_size = state.range(1);
  const int alignment = state.range(1);
  using namespace fastware::memory;

  pool_alloc_create_info_t create_info{
      nullptr, align(alloc_size, static_cast<alignment_t::value>(alignment)),
      static_cast<alignment_t::value>(alignment),
      static_cast<uint64_t>(num_allocs)};
  allocator_t *alloc = create(&create_info);

  std::vector<memblk> allocs(num_allocs);

  for (auto _ : state) {
    state.PauseTiming();
    for (int i = 0; i < num_allocs; i++) {
      allocs[i] = allocate(alloc, alloc_size);
    }
    benchmark::ClobberMemory();
    state.ResumeTiming();

    for (auto blk : allocs) {
      deallocate(alloc, blk);
    }
    benchmark::ClobberMemory();
  }

  destroy(alloc);

  state.counters["PerAlloc"] = benchmark::Counter(
      num_allocs * state.iterations(),
      benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
}

BENCHMARK(memory_pool_allocator_dealloc)
    ->Args({100, 17, 32})
    ->Args({1000, 17, 32})
    ->Args({10000, 17, 32});
BENCHMARK(memory_pool_allocator_dealloc)
    ->Args({100, 55, 32})
    ->Args({1000, 55, 32})
    ->Args({10000, 55, 32});
BENCHMARK(memory_pool_allocator_dealloc)
    ->Args({100, 259, 32})
    ->Args({1000, 259, 32})
    ->Args({10000, 259, 32});