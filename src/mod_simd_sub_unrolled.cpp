#include "hwy/highway.h"

HWY_BEFORE_NAMESPACE();

namespace mod {
// This namespace name is unique per target, which allows code for multiple
// targets to co-exist in the same translation unit. Required when using dynamic
// dispatch, otherwise optional.

namespace HWY_NAMESPACE {

namespace hn = hwy::HWY_NAMESPACE;

const hn::ScalableTag<int16_t> di16;
using VI16 = hn::Vec<decltype(di16)>;


static void mod_sub_v3(
      const int16_t* HWY_RESTRICT a
    ,       VI16     vb
    ,       int16_t* HWY_RESTRICT mod)
{
    auto va = hn::Load(di16,a);

    auto negative_mask = hn::IsNegative(va);
    va = hn::IfThenElse(negative_mask, hn::Neg(va), va);

    vb = hn::ShiftLeft<3>( vb );

    auto mask = hn::Ge(va,vb);
    va = hn::MaskedSubOr(va, mask, va, vb);
    vb = hn::ShiftRight<1>(vb);
    mask = hn::Ge(va,vb);
    va = hn::MaskedSubOr(va, mask, va, vb);
    vb = hn::ShiftRight<1>(vb);
    mask = hn::Ge(va,vb);
    va = hn::MaskedSubOr(va, mask, va, vb);
    vb = hn::ShiftRight<1>(vb);
    mask = hn::Ge(va,vb);
    va = hn::MaskedSubOr(va, mask, va, vb);
    
    va = hn::IfThenElse(negative_mask, hn::Neg(va), va);

    hn::Store(va, di16, mod);
}

static void loop_mod(
      const size_t size
    , const int16_t* HWY_RESTRICT a
    , const int16_t b
    ,       int16_t* HWY_RESTRICT mod
    ) {

    const auto vb =  hn::Set(di16,b);
    const int l = hn::Lanes(di16);

    for ( size_t i = 0; i < size; i += hn::Lanes(di16) * 4 )
    {
        mod_sub_v3(a + i + (0 * l), vb, mod + i + (0 * l)); 
        mod_sub_v3(a + i + (1 * l), vb, mod + i + (1 * l)); 
        mod_sub_v3(a + i + (2 * l), vb, mod + i + (2 * l)); 
        mod_sub_v3(a + i + (3 * l), vb, mod + i + (3 * l));
    }
}


} // namespace HWY_NAMESPACE {
} //} // namespace mod

HWY_AFTER_NAMESPACE();


namespace mod {
    void run_simd_sub_unrolled(
      const size_t size
    , const int16_t* HWY_RESTRICT a
    , const int16_t b
    ,       int16_t* HWY_RESTRICT mod
    ) {
        HWY_STATIC_DISPATCH(loop_mod)(size,a,b,mod);
    }
}
