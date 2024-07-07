#include "hwy/highway.h"

HWY_BEFORE_NAMESPACE();

namespace mod {
// This namespace name is unique per target, which allows code for multiple
// targets to co-exist in the same translation unit. Required when using dynamic
// dispatch, otherwise optional.

namespace HWY_NAMESPACE {

//
// 2024-07-03 Mike
//
// mod( a, b ) = a -  [ a / b ]          * b
//            ~= a - (( a * k ) / 2^16 ) * b
//               with k = 2^16 / b
//

namespace hn = hwy::HWY_NAMESPACE;

template <class DI16, class DI32, class VI16>
static void one_mod(
      const DI16 di16
    , const DI32 di32
    , const int16_t k
    , const int16_t* HWY_RESTRICT a
    , const VI16  vb
    ,       int16_t* HWY_RESTRICT mod)
{
    //
    // k   = 1 + ( (2^16) / b )
    // mod = a - ( (a * k) / 2^16 ) * b
    //
    
    // 1. a * k
    const auto vk = hn::Set(di16, k);
    const auto va = hn::Load(di16,a);
    const auto a_mull_k_even = hn::MulEven( va, vk );
    const auto a_mull_k_odd  = hn::MulOdd ( va, vk );

    // 2. div by 65536
    const auto shift_even = hn::ShiftRight<16>(a_mull_k_even);                    // divide by shifting 16 to the right
    const auto shift_odd  = hn::And( a_mull_k_odd, hn::Set(di32, 0xFFFF0000) );   // divide by setting the lower bits to zero

    auto tmp_mul_div = hn::Add(
          hn::BitCast(di16, shift_even)
        , hn::BitCast(di16, shift_odd) );

    // 3. tmp * b
    const auto tmp_mull_b = hn::Mul(tmp_mul_div, vb);

    // 4. a - tmp
    const auto result = hn::Sub(va, tmp_mull_b);

    hn::Store(result, di16, mod);
}

void loop_mod(
      const size_t size
    , const int16_t* HWY_RESTRICT a
    , const int16_t b
    ,       int16_t* HWY_RESTRICT mod
    ) {
    const hn::ScalableTag<int16_t> di16;
    const hn::ScalableTag<int32_t> di32;

    const int16_t k = 1 + ( (1 << 16) / b );
    const auto vb = hn::Set(di16,b);

    for ( size_t i = 0; i < size; i += hn::Lanes(di16) )
    {
        one_mod(di16,di32, k, a + i, vb, mod + i);
    }
}

} // namespace HWY_NAMESPACE {
} //} // namespace mod

HWY_AFTER_NAMESPACE();

namespace mod {
    void run_mod_simd_shr(
      const size_t size
    , const int16_t* HWY_RESTRICT a
    , const int16_t b
    ,       int16_t* HWY_RESTRICT mod
    ) {
        HWY_STATIC_DISPATCH(loop_mod)(size,a,b,mod);
    }
}
