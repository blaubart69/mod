#include "hwy/highway.h"

HWY_BEFORE_NAMESPACE();

namespace mod {
// This namespace name is unique per target, which allows code for multiple
// targets to co-exist in the same translation unit. Required when using dynamic
// dispatch, otherwise optional.

namespace HWY_NAMESPACE {

namespace hn = hwy::HWY_NAMESPACE;

template <class DI16, class VI16>
static void mod_sub_v3(
    const DI16 di16
    , const int16_t* HWY_RESTRICT a
    , const VI16     vb
    ,       int16_t* HWY_RESTRICT mod)
{
    auto va = hn::Load(di16,a);

    auto negative_mask = hn::IsNegative(va);
    va = hn::IfThenElse(negative_mask, hn::Neg(va), va);

    for (;;) {
            auto mask = hn::Ge(va,vb);
            va = hn::MaskedSubOr(va, mask, va, vb);
            if ( hn::AllFalse(di16,mask) ) {
                break;
            }
        }

    va = hn::IfThenElse(negative_mask, hn::Neg(va), va);

    hn::Store(va, di16, mod);
}

static void loop_mod(
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
        mod_sub_v3(di16, a + i, vb, mod + i);
    }
}

} // namespace HWY_NAMESPACE {
} //} // namespace mod

HWY_AFTER_NAMESPACE();


namespace mod {
    void run_mod_simd_sub(
      const size_t size
    , const int16_t* HWY_RESTRICT a
    , const int16_t b
    ,       int16_t* HWY_RESTRICT mod
    ) {
        HWY_STATIC_DISPATCH(loop_mod)(size,a,b,mod);
    }
}
