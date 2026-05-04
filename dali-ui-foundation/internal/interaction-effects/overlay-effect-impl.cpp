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

OverlayEffectDataTrait GetOrCreateOverlayEffectDataTrait(View owner)
{
  OverlayEffectDataTrait data =
    IntegrationView::GetTrait<OverlayEffectDataTrait>(GetImpl(owner), Integration::ReservedTraitId::STATE_EFFECT_DATA);
  if(!data)
  {
    data = OverlayEffectDataTrait::New();
    IntegrationView::SetTrait(GetImpl(owner), Integration::ReservedTraitId::STATE_EFFECT_DATA, data);
  }
  return data;
}

OverlayEffectDataTrait GetOverlayEffectDataTrait(View owner)
{
  return owner ? IntegrationView::GetTrait<OverlayEffectDataTrait>(GetImpl(owner), Integration::ReservedTraitId::STATE_EFFECT_DATA)
               : OverlayEffectDataTrait();
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

OverlayEffectImpl::OverlayEffectImpl()
: OverlayEffectImpl(UiColor(DEFAULT_OVERLAY_COLOR_RGB, DEFAULT_OVERLAY_COLOR_ALPHA),
                    Vector4::ZERO,
                    CornerRadiusPolicy::ABSOLUTE,
                    true)
{
}

OverlayEffectImpl::OverlayEffectImpl(const UiColor& overlayColor, const Vector4& cornerRadius, CornerRadiusPolicy cornerRadiusPolicy, bool useTargetCornerRadius)
: Integration::StateEffectImpl(),
  mOverlayColor(overlayColor),
  mCornerRadius(cornerRadius),
  mCornerRadiusPolicy(cornerRadiusPolicy),
  mUseTargetCornerRadius(useTargetCornerRadius)
{
}

OverlayEffectImpl::OverlayEffectImpl(const OverlayEffectImpl& other)
: OverlayEffectImpl(other.mOverlayColor, other.mCornerRadius, other.mCornerRadiusPolicy, other.mUseTargetCornerRadius)
{
}

OverlayEffectImpl::~OverlayEffectImpl() = default;

void OverlayEffectImpl::SetOverlayColor(const UiColor& color)
{
  mOverlayColor = color;
}

UiColor OverlayEffectImpl::GetOverlayColor() const
{
  return mOverlayColor;
}

void OverlayEffectImpl::SetCornerRadius(const Vector4& radius)
{
  mCornerRadius = radius;
}

Vector4 OverlayEffectImpl::GetCornerRadius() const
{
  return mCornerRadius;
}

void OverlayEffectImpl::SetCornerRadiusPolicy(CornerRadiusPolicy policy)
{
  mCornerRadiusPolicy = policy;
}

CornerRadiusPolicy OverlayEffectImpl::GetCornerRadiusPolicy() const
{
  return mCornerRadiusPolicy;
}

void OverlayEffectImpl::SetUseTargetCornerRadius(bool useTargetCornerRadius)
{
  mUseTargetCornerRadius = useTargetCornerRadius;
}

bool OverlayEffectImpl::IsUsingTargetCornerRadius() const
{
  return mUseTargetCornerRadius;
}

void OverlayEffectImpl::HandleStateChanged(OverlayEffectDataTraitImpl& data, View owner)
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

void OverlayEffectImpl::Cleanup(OverlayEffectDataTraitImpl& data)
{
  View overlay = data.GetActiveOverlay();
  data.ClearActiveOverlay();
  if(overlay)
  {
    ReleaseOverlay(overlay);
  }
}

void OverlayEffectImpl::OnAttached(TraitId /*id*/, View& view)
{
  InteractiveTrait interaction =
    IntegrationView::GetTrait<InteractiveTrait>(GetImpl(view), Integration::ReservedTraitId::INTERACTION_TRAIT);
  if(!interaction)
  {
    return;
  }

  OverlayEffectDataTrait data = GetOrCreateOverlayEffectDataTrait(view);
  data.GetImpl().AttachEffect(this);
  data.GetImpl().SetState(view.GetState());
  HandleStateChanged(data.GetImpl(), view);
}

void OverlayEffectImpl::OnDetached(TraitId /*id*/, View& view)
{
  OverlayEffectDataTrait data = GetOverlayEffectDataTrait(view);
  if(data)
  {
    data.GetImpl().DetachEffect();
  }
}

void OverlayEffectImpl::OnViewDestroying(ViewImpl* viewImpl)
{
  OverlayEffectDataTrait data =
    viewImpl ? IntegrationView::GetTrait<OverlayEffectDataTrait>(*viewImpl, Integration::ReservedTraitId::STATE_EFFECT_DATA)
             : OverlayEffectDataTrait();
  if(data)
  {
    data.GetImpl().DetachEffect(false);
  }
}

void OverlayEffectImpl::OnViewStateChanged(View view, const StateEvent& event)
{
  OverlayEffectDataTrait data = GetOverlayEffectDataTrait(view);
  if(!data)
  {
    return;
  }

  if(!event.Changed(ViewState::PRESSED) && !event.Changed(ViewState::FOCUSED))
  {
    return;
  }

  data.GetImpl().SetState(event.GetCurrent());
  HandleStateChanged(data.GetImpl(), view);
}

View OverlayEffectImpl::ResolveTarget(View owner) const
{
  if(!owner)
  {
    return View();
  }

  View primaryTarget = owner.GetStateEffectPrimaryTarget();
  return primaryTarget ? primaryTarget : owner;
}

View OverlayEffectImpl::AcquireOverlay() const
{
  return GetOverlayViewPool().Acquire([]()
  {
    return View::New();
  });
}

void OverlayEffectImpl::ReleaseOverlay(View overlay) const
{
  GetOverlayViewPool().Release(overlay, [](View view)
  {
    ResetOverlayView(view);
  });
}

void OverlayEffectImpl::ApplyOverlayProperties(View overlay, View target) const
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

OverlayEffectDataTrait OverlayEffectDataTrait::New()
{
  IntrusivePtr<OverlayEffectDataTraitImpl> impl(new OverlayEffectDataTraitImpl());
  return OverlayEffectDataTrait(impl.Get());
}

OverlayEffectDataTrait OverlayEffectDataTrait::DownCast(BaseHandle handle)
{
  return OverlayEffectDataTrait(dynamic_cast<OverlayEffectDataTraitImpl*>(handle.GetObjectPtr()));
}

OverlayEffectDataTraitImpl& OverlayEffectDataTrait::GetImpl()
{
  return static_cast<OverlayEffectDataTraitImpl&>(GetBaseObject());
}

const OverlayEffectDataTraitImpl& OverlayEffectDataTrait::GetImpl() const
{
  return static_cast<const OverlayEffectDataTraitImpl&>(GetBaseObject());
}

OverlayEffectDataTrait::OverlayEffectDataTrait(OverlayEffectDataTraitImpl* impl)
: Trait(impl)
{
}

OverlayEffectDataTraitImpl::OverlayEffectDataTraitImpl()
: Integration::TraitImpl(),
  mEffect(nullptr),
  mActiveOverlay(),
  mPressed(false),
  mFocused(false)
{
}

OverlayEffectDataTraitImpl::~OverlayEffectDataTraitImpl()
{
  DetachEffect();
}

void OverlayEffectDataTraitImpl::OnViewDestroying(ViewImpl* /*viewImpl*/)
{
  DetachEffect(false);
}

void OverlayEffectDataTraitImpl::AttachEffect(OverlayEffectImpl* effect)
{
  DetachEffect();
  mEffect = effect;
}

void OverlayEffectDataTraitImpl::DetachEffect(bool cleanupOverlay)
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

void OverlayEffectDataTraitImpl::SetActiveOverlay(View overlay)
{
  mActiveOverlay = overlay;
}

View OverlayEffectDataTraitImpl::GetActiveOverlay() const
{
  return mActiveOverlay.GetHandle();
}

bool OverlayEffectDataTraitImpl::HasActiveOverlay() const
{
  return !!mActiveOverlay.GetHandle();
}

void OverlayEffectDataTraitImpl::ClearActiveOverlay()
{
  mActiveOverlay.Reset();
}

bool OverlayEffectDataTraitImpl::IsPressed() const
{
  return mPressed;
}

bool OverlayEffectDataTraitImpl::IsFocused() const
{
  return mFocused;
}

void OverlayEffectDataTraitImpl::SetState(const ViewState& state)
{
  mPressed = state.Contains(ViewState::PRESSED);
  mFocused = state.Contains(ViewState::FOCUSED);
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
