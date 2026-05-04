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
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/input-event.h>
#include <dali-ui-foundation/public-api/overlay-effect.h>
#include <dali-ui-foundation/public-api/state-effect.h>
#include <dali-ui-foundation/public-api/state-event.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

class OverlayEffectDataObject;

class OverlayEffectObject : public StateEffectObject
{
public:
  static constexpr uint32_t DEFAULT_OVERLAY_COLOR_RGB   = 0x000000u;
  static constexpr float    DEFAULT_OVERLAY_COLOR_ALPHA = 0.1f;

  OverlayEffectObject();
  OverlayEffectObject(const UiColor& overlayColor, const Vector4& cornerRadius, CornerRadiusPolicy cornerRadiusPolicy, bool useTargetCornerRadius);
  OverlayEffectObject(const OverlayEffectObject& other);

  void    SetOverlayColor(const UiColor& color);
  UiColor GetOverlayColor() const;

  void    SetCornerRadius(const Vector4& radius);
  Vector4 GetCornerRadius() const;

  void               SetCornerRadiusPolicy(CornerRadiusPolicy policy);
  CornerRadiusPolicy GetCornerRadiusPolicy() const;

  void SetUseTargetCornerRadius(bool useTargetCornerRadius);
  bool IsUsingTargetCornerRadius() const;

  void HandleStateChanged(OverlayEffectDataObject& data, View owner);
  void Cleanup(OverlayEffectDataObject& data);

protected:
  ~OverlayEffectObject() override;

  void OnAttached(TraitId id, View& view) override;
  void OnDetaching(TraitId id, View& view) override;
  void OnViewDestroying(ViewImpl* viewImpl) override;
  void OnViewStateChanged(View view, const StateEvent& event) override;

private:
  View ResolveTarget(View owner) const;
  View AcquireOverlay() const;
  void ReleaseOverlay(View overlay) const;
  void ApplyOverlayProperties(View overlay, View target) const;

private:
  UiColor            mOverlayColor;
  Vector4            mCornerRadius;
  CornerRadiusPolicy mCornerRadiusPolicy;
  bool               mUseTargetCornerRadius;
};

class OverlayEffectDataObject : public TraitObject
{
public:
  OverlayEffectDataObject();

  void AttachEffect(OverlayEffectObject* effect);
  void DetachEffect(bool cleanupOverlay = true);

  void SetActiveOverlay(View overlay);
  View GetActiveOverlay() const;
  bool HasActiveOverlay() const;
  void ClearActiveOverlay();
  bool IsPressed() const;
  bool IsFocused() const;
  void SetState(const ViewState& state);

protected:
  ~OverlayEffectDataObject() override;

private:
  OverlayEffectObject* mEffect;
  WeakHandle<View>     mActiveOverlay;
  bool                 mPressed;
  bool                 mFocused;
};

} // namespace Internal
} // namespace Ui
} // namespace Dali
