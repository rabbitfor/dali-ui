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
#include <dali-ui-foundation/public-api/interactive-trait.h>
#include <dali-ui-foundation/public-api/view-impl.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

namespace
{

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

IntrusivePtr<TraitObject> ToTraitObject(BaseHandle handle)
{
  if(!handle)
  {
    return nullptr;
  }

  auto* traitObject = dynamic_cast<TraitObject*>(handle.GetObjectPtr());
  DALI_ASSERT_ALWAYS(traitObject && "Handle used as a View trait must wrap a TraitObject");
  return traitObject ? IntrusivePtr<TraitObject>(traitObject) : nullptr;
}

template<typename HandleType>
HandleType GetTraitHandle(const ViewImpl& viewImpl, TraitId id)
{
  IntrusivePtr<TraitObject> object     = IntegrationView::GetTrait(viewImpl, id);
  auto*                     baseObject = dynamic_cast<BaseObject*>(object.Get());
  return baseObject ? HandleType::DownCast(BaseHandle(baseObject)) : HandleType();
}

OverlayEffectDataTrait GetOverlayEffectDataTraitFromView(View owner)
{
  if(!owner)
  {
    return OverlayEffectDataTrait();
  }

  IntrusivePtr<TraitObject> object     = IntegrationView::GetTrait(GetImpl(owner), Integration::ReservedTraitId::STATE_EFFECT_DATA);
  auto*                     baseObject = dynamic_cast<BaseObject*>(object.Get());
  return baseObject ? OverlayEffectDataTrait::DownCast(BaseHandle(baseObject)) : OverlayEffectDataTrait();
}

OverlayEffectDataTrait GetOrCreateOverlayEffectDataTrait(View owner)
{
  OverlayEffectDataTrait data = GetOverlayEffectDataTraitFromView(owner);
  if(!data)
  {
    data = OverlayEffectDataTrait::New();
    IntegrationView::SetTrait(GetImpl(owner), Integration::ReservedTraitId::STATE_EFFECT_DATA, ToTraitObject(data));
  }
  return data;
}

OverlayEffectDataTrait GetOverlayEffectDataTrait(View owner)
{
  return GetOverlayEffectDataTraitFromView(owner);
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

  ColorVisual overlay = data.GetActiveOverlay();
  if(!overlay)
  {
    View target = ResolveTarget(owner);
    if(!target)
    {
      return;
    }

    // If overlay ColorVisual creation becomes a measured hot path, consider
    // applying an object pool at this creation point.
    overlay = ColorVisual::New();
    ApplyOverlayProperties(overlay, target);
    target.AddVisual(overlay, Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);
    data.SetActiveOverlay(overlay);
  }

  overlay.SetColor(GetOverlayColorWithActiveCount(mOverlayColor, activeCount));
}

void OverlayEffectImpl::Cleanup(OverlayEffectDataTraitImpl& data)
{
  ColorVisual overlay = data.GetActiveOverlay();
  data.ClearActiveOverlay();
  if(overlay)
  {
    overlay.Detach();
  }
}

void OverlayEffectImpl::OnAttached(TraitId /*id*/, View& view)
{
  InteractiveTrait interaction = GetTraitHandle<InteractiveTrait>(GetImpl(view), Integration::ReservedTraitId::INTERACTION_TRAIT);
  if(!interaction)
  {
    return;
  }

  OverlayEffectDataTrait data = GetOrCreateOverlayEffectDataTrait(view);
  data.GetImpl().AttachEffect(this);
  data.GetImpl().SetState(view.GetState());
  HandleStateChanged(data.GetImpl(), view);
}

void OverlayEffectImpl::OnDetaching(TraitId /*id*/, View& view)
{
  OverlayEffectDataTrait data = GetOverlayEffectDataTrait(view);
  if(data)
  {
    data.GetImpl().DetachEffect();
  }
}

void OverlayEffectImpl::OnViewDestroying(ViewImpl* viewImpl)
{
  OverlayEffectDataTrait data = viewImpl ? GetTraitHandle<OverlayEffectDataTrait>(*viewImpl, Integration::ReservedTraitId::STATE_EFFECT_DATA)
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

void OverlayEffectImpl::ApplyOverlayProperties(ColorVisual overlay, View target) const
{
  overlay.SetColor(mOverlayColor);
  overlay.SetWidth(1.0f);
  overlay.SetHeight(1.0f);

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
: BaseHandle(impl)
{
}

OverlayEffectDataTraitImpl::OverlayEffectDataTraitImpl()
: TraitObject(),
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

void OverlayEffectDataTraitImpl::SetActiveOverlay(ColorVisual overlay)
{
  mActiveOverlay = overlay;
}

ColorVisual OverlayEffectDataTraitImpl::GetActiveOverlay() const
{
  return mActiveOverlay;
}

bool OverlayEffectDataTraitImpl::HasActiveOverlay() const
{
  return !!mActiveOverlay;
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
