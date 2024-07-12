#include <cstdio>
#include <cstdlib>
#include <memory>

#include "benchmark/benchmark.h"

void test_mod(const size_t len, int16_t* a, int16_t b, int16_t* mod) {
  for ( int i=0; i < len; i++) {
        int expected = a[i] % b;
        const char* result;
        if ( expected == mod[i]) {
            result = "OK";
        }
        else {
            result = "ERR";
        }
        printf("%d mod %d = %d\t%s\n", a[i], b, mod[i],result);
    }  
}
namespace mod {
    void run_simd_shr(
      const size_t size
    , const int16_t* __restrict__ a
    , const int16_t b
    ,       int16_t* __restrict__ mod
    );
    void run_simd_sub(
      const size_t size
    , const int16_t* __restrict__ a
    , const int16_t b
    ,       int16_t* __restrict__ mod
    );
    void run_simd_div(
      const size_t size
    , const int16_t* __restrict__ a
    , const int16_t b
    ,       int16_t* __restrict__ mod
    );
}

class Data {
  public:
    size_t size;
    int16_t  b;
    std::unique_ptr<int16_t> a;
    std::unique_ptr<int16_t> mod;

    Data(int align, int16_t start, size_t size,  int16_t b) {
      this->size = size;
      this->b = b;
      a   = std::unique_ptr<int16_t>( static_cast<int16_t*>(std::aligned_alloc( align, size * sizeof(int16_t) )) );
      mod = std::unique_ptr<int16_t>( static_cast<int16_t*>(std::aligned_alloc( align, size * sizeof(int16_t) )) );

      int16_t* _a = a.get();

      for (int i=0; i < size; i++) {
        _a[i] = start + i;
      }
    }

};

const int16_t B = 31;
const int16_t START = -128;
const size_t SIZE = 2 * std::abs(START);
const int ALIGN = 32;

class MyFixture : public benchmark::Fixture {
public:

  Data data;

  MyFixture() : data(ALIGN, START, SIZE, B) {}

  void SetUp(::benchmark::State& state) {
  }

  void TearDown(::benchmark::State& state) {
  }

};

BENCHMARK_DEFINE_F(MyFixture, divide)(benchmark::State& st) {
  for (auto _ : st) {
    mod::run_simd_div(data.size,data.a.get(),data.b,data.mod.get());
  }
}

BENCHMARK_DEFINE_F(MyFixture, substract)(benchmark::State& st) {
  for (auto _ : st) {
    mod::run_simd_sub(data.size,data.a.get(),data.b,data.mod.get());
  }
}


/* BarTest is NOT registered */
BENCHMARK_REGISTER_F(MyFixture, divide);
BENCHMARK_REGISTER_F(MyFixture, substract);

/* BarTest is now registered */
// Register the function as a benchmark
//BENCHMARK(BM_mod_substract)->Setup(DoSetup)->Teardown(DoTeardown);
//BENCHMARK(BM_mod_div)->Setup(DoSetup)->Teardown(DoTeardown);
// Run the benchmark
BENCHMARK_MAIN();

/*
int main() {
  Data data(ALIGN, START, SIZE, B);
  mod::run_simd_sub(data.size,data.a.get(),data.b,data.mod.get());
  test_mod(data.size,data.a.get(),data.b,data.mod.get());
}*/