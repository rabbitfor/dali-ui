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
#include <dali-ui-foundation/internal/interaction-effects/scale-effect-data.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/trait-impl.h>

namespace Dali
{

namespace Ui
{

namespace Internal
{

// =============================================================================
// ScaleEffectDataImpl (private to this TU)
// =============================================================================

class ScaleEffectDataImpl : public Integration::TraitImpl
{
public:
  ScaleEffectDataImpl(float originalScaleX, float originalScaleY)
  : mOriginalScaleX(originalScaleX),
    mOriginalScaleY(originalScaleY)
  {
  }

  ~ScaleEffectDataImpl() override
  {
    StopAnimation();
  }

  void StopAnimation()
  {
    if(mAnimation)
    {
      mAnimation.Stop();
      mAnimation.Reset();
    }
  }

  float     mOriginalScaleX;
  float     mOriginalScaleY;
  Animation mAnimation;

protected:
  void OnBeforeAttached(Integration::TraitId /*id*/, View& /*view*/) override
  {
  }

  void OnAttached(Integration::TraitId /*id*/, View& /*view*/) override
  {
  }

  void OnDetached(Integration::TraitId /*id*/, View& /*view*/) override
  {
    StopAnimation();
  }

  void OnViewDestroying(Integration::ViewImpl* /*viewImpl*/) override
  {
    StopAnimation();
  }
};

// =============================================================================
// ScaleEffectData handle methods
// =============================================================================

static ScaleEffectDataImpl& GetImpl(const ScaleEffectData& data)
{
  return static_cast<ScaleEffectDataImpl&>(const_cast<Dali::BaseObject&>(data.GetBaseObject()));
}

ScaleEffectData ScaleEffectData::New(float originalScaleX, float originalScaleY)
{
  return ScaleEffectData(new ScaleEffectDataImpl(originalScaleX, originalScaleY));
}

ScaleEffectData ScaleEffectData::DownCast(Trait handle)
{
  return ScaleEffectData(dynamic_cast<ScaleEffectDataImpl*>(handle.GetObjectPtr()));
}

float ScaleEffectData::GetOriginalScaleX() const
{
  return GetImpl(*this).mOriginalScaleX;
}

float ScaleEffectData::GetOriginalScaleY() const
{
  return GetImpl(*this).mOriginalScaleY;
}

Animation ScaleEffectData::GetAnimation() const
{
  return GetImpl(*this).mAnimation;
}

void ScaleEffectData::SetAnimation(Animation anim)
{
  GetImpl(*this).mAnimation = anim;
}

} // namespace Internal

} // namespace Ui

} // namespace Dali
