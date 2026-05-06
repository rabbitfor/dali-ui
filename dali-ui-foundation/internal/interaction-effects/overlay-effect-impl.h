#pragma once

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

// EXTERNAL INCLUDES
#include <dali/public-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/state-effect-impl.h>
#include <dali-ui-foundation/public-api/input-event.h>
#include <dali-ui-foundation/public-api/overlay-effect.h>
#include <dali-ui-foundation/public-api/state-event.h>
#include <dali-ui-foundation/public-api/trait-object.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

class OverlayEffectDataTraitImpl;

class OverlayEffectImpl : public Integration::StateEffectImpl
{
public:
  static constexpr uint32_t DEFAULT_OVERLAY_COLOR_RGB   = 0x000000u;
  static constexpr float    DEFAULT_OVERLAY_COLOR_ALPHA = 0.1f;

  OverlayEffectImpl();
  OverlayEffectImpl(const UiColor& overlayColor, const Vector4& cornerRadius, CornerRadiusPolicy cornerRadiusPolicy, bool useTargetCornerRadius);
  OverlayEffectImpl(const OverlayEffectImpl& other);

  void    SetOverlayColor(const UiColor& color);
  UiColor GetOverlayColor() const;

  void    SetCornerRadius(const Vector4& radius);
  Vector4 GetCornerRadius() const;

  void               SetCornerRadiusPolicy(CornerRadiusPolicy policy);
  CornerRadiusPolicy GetCornerRadiusPolicy() const;

  void SetUseTargetCornerRadius(bool useTargetCornerRadius);
  bool IsUsingTargetCornerRadius() const;

  void HandleStateChanged(OverlayEffectDataTraitImpl& data, View owner);
  void Cleanup(OverlayEffectDataTraitImpl& data);

protected:
  ~OverlayEffectImpl() override;

  void OnAttached(TraitId id, View& view) override;
  void OnDetaching(TraitId id, View& view) override;
  void OnViewDestroying(ViewImpl* viewImpl) override;
  void OnViewStateChanged(View view, const StateEvent& event) override;

private:
  View ResolveTarget(View owner) const;
  void ApplyOverlayProperties(View overlay, View target) const;

private:
  UiColor            mOverlayColor;
  Vector4            mCornerRadius;
  CornerRadiusPolicy mCornerRadiusPolicy;
  bool               mUseTargetCornerRadius;
};

class OverlayEffectDataTrait : public BaseHandle
{
public:
  OverlayEffectDataTrait() = default;

  static OverlayEffectDataTrait New();
  static OverlayEffectDataTrait DownCast(BaseHandle handle);

  OverlayEffectDataTraitImpl&       GetImpl();
  const OverlayEffectDataTraitImpl& GetImpl() const;

private:
  explicit OverlayEffectDataTrait(OverlayEffectDataTraitImpl* impl);
};

class OverlayEffectDataTraitImpl : public TraitObject
{
public:
  OverlayEffectDataTraitImpl();

  void AttachEffect(OverlayEffectImpl* effect);
  void DetachEffect(bool cleanupOverlay = true);

  void SetActiveOverlay(View overlay);
  View GetActiveOverlay() const;
  bool HasActiveOverlay() const;
  void ClearActiveOverlay();
  bool IsPressed() const;
  bool IsFocused() const;
  void SetState(const ViewState& state);

protected:
  ~OverlayEffectDataTraitImpl() override;

  void OnViewDestroying(ViewImpl* viewImpl) override;

private:
  OverlayEffectImpl* mEffect;
  WeakHandle<View>   mActiveOverlay;
  bool               mPressed;
  bool               mFocused;
};

} // namespace Internal

inline DALI_UI_API Internal::OverlayEffectImpl& GetImpl(OverlayEffect& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Internal::OverlayEffectImpl&>(handle);
}

inline DALI_UI_API const Internal::OverlayEffectImpl& GetImpl(const OverlayEffect& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Internal::OverlayEffectImpl&>(handle);
}

} // namespace Ui
} // namespace Dali
