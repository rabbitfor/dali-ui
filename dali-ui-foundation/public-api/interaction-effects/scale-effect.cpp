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
#include <dali-ui-foundation/public-api/interaction-effects/scale-effect.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/scale-effect-impl.h>

namespace Dali
{

namespace Ui
{

// =============================================================================
// MutableScaleEffect
// =============================================================================

MutableScaleEffect::MutableScaleEffect(ScaleEffect& copy)
: mCopy(copy)
{
}

void MutableScaleEffect::Invalidate()
{
  mValid = false;
}

MutableScaleEffect& MutableScaleEffect::SetFocusedScale(float scale)
{
  DALI_ASSERT_ALWAYS(mValid && "MutableScaleEffect must not be used outside Configure() scope");
  mCopy.GetImpl().SetFocusedScale(scale);
  return *this;
}

MutableScaleEffect& MutableScaleEffect::SetPressedScale(float scale)
{
  DALI_ASSERT_ALWAYS(mValid && "MutableScaleEffect must not be used outside Configure() scope");
  mCopy.GetImpl().SetPressedScale(scale);
  return *this;
}

// =============================================================================
// ScaleEffect
// =============================================================================

ScaleEffect::ScaleEffect()
{
}

ScaleEffect::ScaleEffect(Integration::ScaleEffectImpl* impl)
: Trait(impl)
{
}

ScaleEffect ScaleEffect::DownCast(BaseHandle handle)
{
  return ScaleEffect(dynamic_cast<Integration::ScaleEffectImpl*>(handle.GetObjectPtr()));
}

ScaleEffect ScaleEffect::Default()
{
  static ScaleEffect instance = []()
  {
    Integration::ScaleEffectImpl* impl = new Integration::ScaleEffectImpl(Integration::ScaleEffectImpl::Params{});
    return ScaleEffect(impl);
  }();
  return instance;
}

ScaleEffect ScaleEffect::DoClone() const
{
  return ScaleEffect(GetImpl().Clone());
}

Integration::ScaleEffectImpl& ScaleEffect::GetImpl() const
{
  return static_cast<Integration::ScaleEffectImpl&>(const_cast<Dali::BaseObject&>(GetBaseObject()));
}

} // namespace Ui

} // namespace Dali
