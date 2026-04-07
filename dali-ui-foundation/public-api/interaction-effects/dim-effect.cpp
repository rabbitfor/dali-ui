/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// CLASS HEADER
#include <dali-ui-foundation/public-api/interaction-effects/dim-effect.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/dim-effect-impl.h>

namespace Dali
{

namespace Ui
{

// =============================================================================
// MutableDimEffect
// =============================================================================

MutableDimEffect::MutableDimEffect(DimEffect& copy)
: mCopy(copy)
{
}

void MutableDimEffect::Invalidate()
{
  mValid = false;
}

MutableDimEffect& MutableDimEffect::SetPressedOpacity(float opacity)
{
  DALI_ASSERT_ALWAYS(mValid && "MutableDimEffect must not be used outside Configure() scope");
  mCopy.GetImpl().SetPressedOpacity(opacity);
  return *this;
}

MutableDimEffect& MutableDimEffect::SetFocusedOpacity(float opacity)
{
  DALI_ASSERT_ALWAYS(mValid && "MutableDimEffect must not be used outside Configure() scope");
  mCopy.GetImpl().SetFocusedOpacity(opacity);
  return *this;
}

// =============================================================================
// DimEffect
// =============================================================================

DimEffect::DimEffect()
{
}

DimEffect::DimEffect(Integration::DimEffectImpl* impl)
: Trait(impl)
{
}

DimEffect DimEffect::DownCast(BaseHandle handle)
{
  return DimEffect(dynamic_cast<Integration::DimEffectImpl*>(handle.GetObjectPtr()));
}

DimEffect DimEffect::Default()
{
  static DimEffect instance = []()
  {
    Integration::DimEffectImpl* impl = new Integration::DimEffectImpl(Integration::DimEffectImpl::Params{});
    return DimEffect(impl);
  }();
  return instance;
}

DimEffect DimEffect::Deep()
{
  static DimEffect instance = []()
  {
    Integration::DimEffectImpl::Params p;
    p.pressedOpacity                 = 0.30f;
    p.focusedOpacity                 = 0.15f;
    Integration::DimEffectImpl* impl = new Integration::DimEffectImpl(p);
    return DimEffect(impl);
  }();
  return instance;
}

DimEffect DimEffect::DoClone() const
{
  return DimEffect(GetImpl().Clone());
}

Integration::DimEffectImpl& DimEffect::GetImpl() const
{
  return static_cast<Integration::DimEffectImpl&>(GetBaseObject());
}

} // namespace Ui

} // namespace Dali
