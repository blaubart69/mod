#include "hwy/highway.h"

HWY_BEFORE_NAMESPACE();

namespace mod {
// This namespace name is unique per target, which allows code for multiple
// targets to co-exist in the same translation unit. Required when using dynamic
// dispatch, otherwise optional.

namespace HWY_NAMESPACE {

namespace hn = hwy::HWY_NAMESPACE;

template <class DI16, class VI16>
static void mod_div(
    const DI16 di16
    , const int16_t* HWY_RESTRICT a
    , const VI16     vb
    ,       int16_t* HWY_RESTRICT mod)
{
    const auto va = hn::Load(di16,a);

    VI16 result = hn::Mod(va,vb);

    hn::Store(result, di16, mod);
}

static void loop_mod(
      const size_t size
    , const int16_t* HWY_RESTRICT a
    , const int16_t b
    ,       int16_t* HWY_RESTRICT mod
    ) {
    const hn::ScalableTag<int16_t> di16;

    const auto vb = hn::Set(di16,b);

    for ( size_t i = 0; i < size; i += hn::Lanes(di16) )
    {
        mod_div(di16, a + i, vb, mod + i);
    }
}

} // namespace HWY_NAMESPACE {
} //} // namespace mod

HWY_AFTER_NAMESPACE();


namespace mod {
    void run_simd_div(
      const size_t size
    , const int16_t* HWY_RESTRICT a
    , const int16_t b
    ,       int16_t* HWY_RESTRICT mod
    ) {
        HWY_STATIC_DISPATCH(loop_mod)(size,a,b,mod);
    }
}
