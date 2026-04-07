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
#include <dali/public-api/signals/connection-tracker.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/trait-impl.h>
#include <dali-ui-foundation/public-api/interaction-effects/dim-effect.h>

namespace Dali
{

namespace Ui
{

namespace Integration
{

/**
 * @brief Implementation of DimEffect.
 *
 * Applies a semi-transparent overlay on the host view in response to
 * UiState changes (PRESSED, FOCUSED). The exact visual mechanism is
 * determined by OnStateChanged(), which subclasses may override.
 *
 * Instances are created frozen (via DimEffect predefined factories) or
 * unfrozen (during DimEffect::Configure()). Once Freeze() is called —
 * which ViewImpl does on SetInteractionEffect() — setter calls assert.
 */
class DALI_UI_API DimEffectImpl : public TraitImpl, public ConnectionTracker
{
public:
  /**
   * @brief Plain-data parameters for a DimEffect.
   *
   * MutableDimEffect forwards its setters directly to these fields via DimEffectImpl.
   */
  struct Params
  {
    float pressedOpacity{0.15f}; ///< Additional opacity dimming when PRESSED
    float focusedOpacity{0.08f}; ///< Additional opacity dimming when FOCUSED
  };

  /**
   * @brief Creates a new DimEffectImpl with the given parameters.
   *
   * @param[in] params The initial parameters
   */
  explicit DimEffectImpl(const Params& params);

  // --- Setters (only callable while unfrozen) ---

  /**
   * @brief Sets the opacity applied when the view is pressed.
   *
   * @pre Must not be frozen.
   * @param[in] opacity Value in [0, 1]; higher = darker overlay
   */
  void SetPressedOpacity(float opacity);

  /**
   * @brief Sets the opacity applied when the view has focus.
   *
   * @pre Must not be frozen.
   * @param[in] opacity Value in [0, 1]; higher = darker overlay
   */
  void SetFocusedOpacity(float opacity);

  // --- Getters ---

  /**
   * @brief Returns the pressed opacity parameter.
   */
  float GetPressedOpacity() const;

  /**
   * @brief Returns the focused opacity parameter.
   */
  float GetFocusedOpacity() const;

  /**
   * @brief Creates an unfrozen deep copy of this impl.
   *
   * @return A new, unfrozen DimEffectImpl with the same parameters
   */
  DimEffectImpl* Clone() const;

protected:
  /**
   * @brief Called when the host view's UiState changes.
   *
   * Subclasses override this to provide the actual visual response.
   * The default implementation is a no-op (TBD).
   *
   * @param[in] view  The host view
   * @param[in] event The state transition event
   */
  virtual void OnStateChanged(View view, const StateEvent& event);

  ~DimEffectImpl() override;

  // --- TraitImpl lifecycle ---
  void OnBeforeAttached(TraitId id, View& view) override;
  void OnAttached(TraitId id, View& view) override; ///< Connects to StateChangedSignal
  void OnDetached(TraitId id, View& view) override; ///< Disconnects from StateChangedSignal
  void OnViewDestroying(ViewImpl* viewImpl) override;

private:
  void OnViewStateChangedInternal(View view, const StateEvent& event);

private:
  WeakHandle<View> mOwner;
  Params           mParams;
};

} // namespace Integration

// GetImpl helpers

inline Integration::DimEffectImpl& GetImpl(DimEffect& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Integration::DimEffectImpl&>(handle);
}

inline const Integration::DimEffectImpl& GetImpl(const DimEffect& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Integration::DimEffectImpl&>(handle);
}

} // namespace Ui

} // namespace Dali
