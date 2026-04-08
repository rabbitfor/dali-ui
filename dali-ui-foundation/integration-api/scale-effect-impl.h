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
#include <dali/public-api/signals/connection-tracker.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/trait-impl.h>
#include <dali-ui-foundation/public-api/interaction-effects/scale-effect.h>

namespace Dali
{

namespace Ui
{

namespace Integration
{

/**
 * @brief Implementation of ScaleEffect.
 *
 * Applies a scale animation on the host view in response to UiState changes.
 * This impl is shared across all views that use the same ScaleEffect handle.
 * Per-view data (original scale, running animation) is stored in a separate
 * ScaleEffectDataImpl trait attached to each view.
 *
 * State priority: PRESSED > FOCUSED > NORMAL
 * - PRESSED: originalScale × pressedScale (default 0.9)
 * - FOCUSED: originalScale × focusedScale (default 1.18)
 * - NORMAL:  originalScale × 1.0
 */
class DALI_UI_API ScaleEffectImpl : public TraitImpl, public ConnectionTracker
{
public:
  /**
   * @brief Plain-data parameters for a ScaleEffect.
   */
  struct Params
  {
    float focusedScale{1.18f}; ///< Scale multiplier when FOCUSED
    float pressedScale{0.9f};  ///< Scale multiplier when PRESSED (takes priority over focused)
  };

  /**
   * @brief Creates a new ScaleEffectImpl with the given parameters.
   */
  explicit ScaleEffectImpl(const Params& params);

  // --- Setters (only callable while unfrozen) ---

  /**
   * @brief Sets the scale multiplier applied when the view has keyboard focus.
   */
  void SetFocusedScale(float scale);

  /**
   * @brief Sets the scale multiplier applied when the view is pressed.
   */
  void SetPressedScale(float scale);

  // --- Getters ---

  float GetFocusedScale() const;
  float GetPressedScale() const;

  /**
   * @brief Creates a deep copy of this impl.
   */
  ScaleEffectImpl* Clone() const;

protected:
  /**
   * @brief Called when the host view's UiState changes.
   *
   * Computes the target scale and runs an animation on the view.
   */
  virtual void OnStateChanged(View view, const StateEvent& event);

  ~ScaleEffectImpl() override;

  // --- TraitImpl lifecycle ---
  void OnBeforeAttached(TraitId id, View& view) override;
  void OnAttached(TraitId id, View& view) override;
  void OnDetached(TraitId id, View& view) override;
  void OnViewDestroying(ViewImpl* viewImpl) override;

private:
  void OnViewStateChangedInternal(View view, const StateEvent& event);

private:
  Params mParams;
};

} // namespace Integration

// GetImpl helpers

inline Integration::ScaleEffectImpl& GetImpl(ScaleEffect& obj)
{
  return static_cast<Integration::ScaleEffectImpl&>(obj.GetBaseObject());
}

inline const Integration::ScaleEffectImpl& GetImpl(const ScaleEffect& obj)
{
  return static_cast<const Integration::ScaleEffectImpl&>(obj.GetBaseObject());
}

} // namespace Ui

} // namespace Dali
