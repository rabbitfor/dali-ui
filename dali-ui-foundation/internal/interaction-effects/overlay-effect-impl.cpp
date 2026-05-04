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
#include <dali-ui-foundation/internal/interaction-effects/overlay-effect-impl.h>

// EXTERNAL INCLUDES
#include <cmath>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/reserved-trait-id.h>
#include <dali-ui-foundation/integration-api/view-integ.h>
#include <dali-ui-foundation/internal/common/object-pool.h>
#include <dali-ui-foundation/public-api/interactive-trait.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/view-impl.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

namespace
{

ObjectPool<View>& GetOverlayViewPool()
{
  static thread_local ObjectPool<View> pool;
  return pool;
}

UiColor GetOverlayColorWithActiveCount(const UiColor& color, uint32_t activeCount)
{
  if(activeCount <= 1u)
  {
    return color;
  }

  Vector4 rgba = color.GetRgba();
  rgba.a       = 1.0f - std::pow(1.0f - rgba.a, static_cast<float>(activeCount));
  return UiColor(rgba);
}

OverlayEffectDataObject* GetOverlayEffectDataObject(View owner)
{
  IntrusivePtr<TraitObject> object = owner ? IntegrationView::GetTrait(GetImpl(owner), Integration::ReservedTraitId::STATE_EFFECT_DATA) : nullptr;
  return dynamic_cast<OverlayEffectDataObject*>(object.Get());
}

OverlayEffectDataObject& GetOrCreateOverlayEffectDataObject(View owner)
{
  OverlayEffectDataObject* data = GetOverlayEffectDataObject(owner);
  if(!data)
  {
    IntrusivePtr<OverlayEffectDataObject> newData(new OverlayEffectDataObject());
    IntegrationView::SetTrait(GetImpl(owner), Integration::ReservedTraitId::STATE_EFFECT_DATA, newData);
    data = newData.Get();
  }
  return *data;
}

void ResetOverlayView(View overlay)
{
  if(!overlay)
  {
    return;
  }

  if(overlay.GetParent())
  {
    overlay.GetParent().Remove(overlay);
  }

  overlay.ClearBackground();
  overlay.SetRequestedWidth(WRAP_CONTENT);
  overlay.SetRequestedHeight(WRAP_CONTENT);
  overlay.SetLayoutMode(LayoutMode::DEFAULT);
  overlay.SetCornerRadius(Vector4::ZERO);
  overlay.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
}

} // namespace

OverlayEffectObject::OverlayEffectObject()
: OverlayEffectObject(UiColor(DEFAULT_OVERLAY_COLOR_RGB, DEFAULT_OVERLAY_COLOR_ALPHA),
                      Vector4::ZERO,
                      CornerRadiusPolicy::ABSOLUTE,
                      true)
{
}

OverlayEffectObject::OverlayEffectObject(const UiColor& overlayColor, const Vector4& cornerRadius, CornerRadiusPolicy cornerRadiusPolicy, bool useTargetCornerRadius)
: StateEffectObject(),
  mOverlayColor(overlayColor),
  mCornerRadius(cornerRadius),
  mCornerRadiusPolicy(cornerRadiusPolicy),
  mUseTargetCornerRadius(useTargetCornerRadius)
{
}

OverlayEffectObject::OverlayEffectObject(const OverlayEffectObject& other)
: OverlayEffectObject(other.mOverlayColor, other.mCornerRadius, other.mCornerRadiusPolicy, other.mUseTargetCornerRadius)
{
}

OverlayEffectObject::~OverlayEffectObject() = default;

void OverlayEffectObject::SetOverlayColor(const UiColor& color)
{
  mOverlayColor = color;
}

UiColor OverlayEffectObject::GetOverlayColor() const
{
  return mOverlayColor;
}

void OverlayEffectObject::SetCornerRadius(const Vector4& radius)
{
  mCornerRadius = radius;
}

Vector4 OverlayEffectObject::GetCornerRadius() const
{
  return mCornerRadius;
}

void OverlayEffectObject::SetCornerRadiusPolicy(CornerRadiusPolicy policy)
{
  mCornerRadiusPolicy = policy;
}

CornerRadiusPolicy OverlayEffectObject::GetCornerRadiusPolicy() const
{
  return mCornerRadiusPolicy;
}

void OverlayEffectObject::SetUseTargetCornerRadius(bool useTargetCornerRadius)
{
  mUseTargetCornerRadius = useTargetCornerRadius;
}

bool OverlayEffectObject::IsUsingTargetCornerRadius() const
{
  return mUseTargetCornerRadius;
}

void OverlayEffectObject::HandleStateChanged(OverlayEffectDataObject& data, View owner)
{
  uint32_t activeCount = 0u;
  if(data.IsPressed())
  {
    ++activeCount;
  }
  if(data.IsFocused())
  {
    ++activeCount;
  }

  if(activeCount == 0u)
  {
    Cleanup(data);
    return;
  }

  View overlay = data.GetActiveOverlay();
  if(!overlay)
  {
    View target = ResolveTarget(owner);
    if(!target)
    {
      return;
    }

    overlay = AcquireOverlay();
    ApplyOverlayProperties(overlay, target);
    target.Add(overlay);
    overlay.LowerToBottom();
    data.SetActiveOverlay(overlay);
  }

  overlay.SetBackgroundColor(GetOverlayColorWithActiveCount(mOverlayColor, activeCount));
}

void OverlayEffectObject::Cleanup(OverlayEffectDataObject& data)
{
  View overlay = data.GetActiveOverlay();
  data.ClearActiveOverlay();
  if(overlay)
  {
    ReleaseOverlay(overlay);
  }
}

void OverlayEffectObject::OnAttached(TraitId /*id*/, View& view)
{
  OverlayEffectDataObject& data = GetOrCreateOverlayEffectDataObject(view);
  data.AttachEffect(this);
  data.SetState(view.GetState());
  HandleStateChanged(data, view);
}

void OverlayEffectObject::OnDetaching(TraitId /*id*/, View& view)
{
  OverlayEffectDataObject* data = GetOverlayEffectDataObject(view);
  if(data)
  {
    data->DetachEffect();
  }
}

void OverlayEffectObject::OnViewDestroying(ViewImpl* viewImpl)
{
  IntrusivePtr<TraitObject> object = viewImpl ? IntegrationView::GetTrait(*viewImpl, Integration::ReservedTraitId::STATE_EFFECT_DATA) : nullptr;
  auto*                     data   = dynamic_cast<OverlayEffectDataObject*>(object.Get());
  if(data)
  {
    data->DetachEffect(false);
  }
}

void OverlayEffectObject::OnViewStateChanged(View view, const StateEvent& event)
{
  OverlayEffectDataObject* data = GetOverlayEffectDataObject(view);
  if(!data)
  {
    return;
  }

  if(!event.Changed(ViewState::PRESSED) && !event.Changed(ViewState::FOCUSED))
  {
    return;
  }

  data->SetState(event.GetCurrent());
  HandleStateChanged(*data, view);
}

View OverlayEffectObject::ResolveTarget(View owner) const
{
  if(!owner)
  {
    return View();
  }

  View primaryTarget = owner.GetStateEffectPrimaryTarget();
  return primaryTarget ? primaryTarget : owner;
}

View OverlayEffectObject::AcquireOverlay() const
{
  return GetOverlayViewPool().Acquire([]()
  {
    return View::New();
  });
}

void OverlayEffectObject::ReleaseOverlay(View overlay) const
{
  GetOverlayViewPool().Release(overlay, [](View view)
  {
    ResetOverlayView(view);
  });
}

void OverlayEffectObject::ApplyOverlayProperties(View overlay, View target) const
{
  overlay.SetBackgroundColor(mOverlayColor)
    .SetRequestedWidth(MATCH_PARENT)
    .SetRequestedHeight(MATCH_PARENT)
    .SetLayoutMode(LayoutMode::STANDALONE);

  if(mUseTargetCornerRadius)
  {
    overlay.SetCornerRadius(target.GetCornerRadius());
    overlay.SetCornerRadiusPolicy(target.GetCornerRadiusPolicy());
  }
  else
  {
    overlay.SetCornerRadius(mCornerRadius);
    overlay.SetCornerRadiusPolicy(mCornerRadiusPolicy);
  }
}

OverlayEffectDataObject::OverlayEffectDataObject()
: TraitObject(),
  mEffect(nullptr),
  mActiveOverlay(),
  mPressed(false),
  mFocused(false)
{
}

OverlayEffectDataObject::~OverlayEffectDataObject()
{
  DetachEffect();
}

void OverlayEffectDataObject::AttachEffect(OverlayEffectObject* effect)
{
  DetachEffect();
  mEffect = effect;
}

void OverlayEffectDataObject::DetachEffect(bool cleanupOverlay)
{
  if(mEffect && cleanupOverlay)
  {
    mEffect->Cleanup(*this);
  }
  else
  {
    ClearActiveOverlay();
  }
  mEffect  = nullptr;
  mPressed = false;
  mFocused = false;
}

void OverlayEffectDataObject::SetActiveOverlay(View overlay)
{
  mActiveOverlay = overlay;
}

View OverlayEffectDataObject::GetActiveOverlay() const
{
  return mActiveOverlay.GetHandle();
}

bool OverlayEffectDataObject::HasActiveOverlay() const
{
  return !!mActiveOverlay.GetHandle();
}

void OverlayEffectDataObject::ClearActiveOverlay()
{
  mActiveOverlay.Reset();
}

bool OverlayEffectDataObject::IsPressed() const
{
  return mPressed;
}

bool OverlayEffectDataObject::IsFocused() const
{
  return mFocused;
}

void OverlayEffectDataObject::SetState(const ViewState& state)
{
  mPressed = state.Contains(ViewState::PRESSED);
  mFocused = state.Contains(ViewState::FOCUSED);
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
