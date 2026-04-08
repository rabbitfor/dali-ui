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
#include <dali-ui-foundation/integration-api/scale-effect-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/object/property.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/trait-id.h>
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/internal/interaction-effects/scale-effect-data.h>
#include <dali-ui-foundation/public-api/state-event.h>
#include <dali-ui-foundation/public-api/ui-state.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{

namespace Ui
{

namespace Integration
{

namespace
{

constexpr float SCALE_ANIMATION_DURATION = 0.15f;

Internal::ScaleEffectData GetData(View& view)
{
  Trait dataTrait = GetImpl(view).GetTrait(ReservedTraitId::INTERACTION_EFFECT_DATA);
  return Internal::ScaleEffectData::DownCast(dataTrait);
}

} // unnamed namespace

ScaleEffectImpl::ScaleEffectImpl(const Params& params)
: mParams(params)
{
}

ScaleEffectImpl::~ScaleEffectImpl()
{
}

void ScaleEffectImpl::SetFocusedScale(float scale)
{
  mParams.focusedScale = scale;
}

void ScaleEffectImpl::SetPressedScale(float scale)
{
  mParams.pressedScale = scale;
}

float ScaleEffectImpl::GetFocusedScale() const
{
  return mParams.focusedScale;
}

float ScaleEffectImpl::GetPressedScale() const
{
  return mParams.pressedScale;
}

ScaleEffectImpl* ScaleEffectImpl::Clone() const
{
  return new ScaleEffectImpl(mParams);
}

void ScaleEffectImpl::OnBeforeAttached(TraitId /*id*/, View& /*view*/)
{
}

void ScaleEffectImpl::OnAttached(TraitId /*id*/, View& view)
{
  float originalScaleX = view.GetScaleX();
  float originalScaleY = view.GetScaleY();

  Internal::ScaleEffectData dataTrait = Internal::ScaleEffectData::New(originalScaleX, originalScaleY);
  GetImpl(view).SetTrait(ReservedTraitId::INTERACTION_EFFECT_DATA, dataTrait);

  view.StateChangedSignal().Connect(this, &ScaleEffectImpl::OnViewStateChangedInternal);
}

void ScaleEffectImpl::OnDetached(TraitId /*id*/, View& view)
{
  view.StateChangedSignal().Disconnect(this, &ScaleEffectImpl::OnViewStateChangedInternal);

  Internal::ScaleEffectData data = GetData(view);
  if(data)
  {
    Animation anim = data.GetAnimation();
    if(anim)
    {
      anim.Stop();
    }
    view.SetScale(data.GetOriginalScaleX(), data.GetOriginalScaleY());
  }

  GetImpl(view).RemoveTrait(ReservedTraitId::INTERACTION_EFFECT_DATA);
}

void ScaleEffectImpl::OnViewDestroying(ViewImpl* /*viewImpl*/)
{
  // Per-view data trait handles its own cleanup via OnViewDestroying
}

void ScaleEffectImpl::OnStateChanged(View view, const StateEvent& event)
{
  Internal::ScaleEffectData data = GetData(view);
  if(!data)
  {
    return;
  }

  bool pressed = event.GetCurrent().Contains(UiState::PRESSED);
  bool focused = event.GetCurrent().Contains(UiState::FOCUSED);

  float multiplier = pressed   ? mParams.pressedScale
                     : focused ? mParams.focusedScale
                               : 1.0f;

  float targetX = data.GetOriginalScaleX() * multiplier;
  float targetY = data.GetOriginalScaleY() * multiplier;

  // Cancel any running animation
  Animation prev = data.GetAnimation();
  if(prev)
  {
    prev.Stop();
  }

  // Animate to target scale
  Animation anim = Animation::New(SCALE_ANIMATION_DURATION);
  anim.AnimateTo(Property(view, Actor::Property::SCALE_X), targetX, AlphaFunction::EASE_OUT);
  anim.AnimateTo(Property(view, Actor::Property::SCALE_Y), targetY, AlphaFunction::EASE_OUT);
  anim.Play();
  data.SetAnimation(anim);
}

void ScaleEffectImpl::OnViewStateChangedInternal(View view, const StateEvent& event)
{
  OnStateChanged(view, event);
}

} // namespace Integration

} // namespace Ui

} // namespace Dali
