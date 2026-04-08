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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/trait.h>

namespace Dali
{

namespace Ui
{

// Forward declarations
namespace Integration
{
class ScaleEffectImpl;
}

class ScaleEffect;

/**
 * @brief Mutable proxy for configuring a ScaleEffect inside ScaleEffect::Configure().
 *
 * MutableScaleEffect is a non-owning, non-copyable proxy that forwards setter calls
 * to the underlying ScaleEffectImpl. It can only be obtained inside the lambda passed
 * to ScaleEffect::Configure() — there is intentionally no public constructor.
 *
 * Attempting to store a pointer or reference to a MutableScaleEffect outside the
 * lambda scope will result in a runtime assertion on the next setter call, because
 * the proxy is invalidated immediately after the lambda returns.
 *
 * @code
 * auto effect = ScaleEffect::Default().Configure([](MutableScaleEffect& m) {
 *   m.SetFocusedScale(1.3f);
 * });
 * @endcode
 */
class DALI_UI_API MutableScaleEffect
{
public:
  /**
   * @brief Sets the scale multiplier applied when the view has keyboard focus.
   *
   * @param[in] scale Multiplier applied to the view's original scale (e.g. 1.18 = 18% larger)
   * @return Reference to this for method chaining
   */
  MutableScaleEffect& SetFocusedScale(float scale);

  /**
   * @brief Sets the scale multiplier applied when the view is pressed.
   *
   * Press state takes priority over focus regardless of focus state.
   *
   * @param[in] scale Multiplier applied to the view's original scale (e.g. 0.9 = 10% smaller)
   * @return Reference to this for method chaining
   */
  MutableScaleEffect& SetPressedScale(float scale);

  // --- Non-copyable, non-movable, non-addressable ---
  MutableScaleEffect(const MutableScaleEffect&)            = delete;
  MutableScaleEffect(MutableScaleEffect&&)                 = delete;
  MutableScaleEffect& operator=(const MutableScaleEffect&) = delete;
  MutableScaleEffect& operator=(MutableScaleEffect&&)      = delete;

  /// @cond internal
  void* operator&() = delete; ///< Prevents taking a pointer to this proxy
  /// @endcond

private:
  friend class ScaleEffect;

  explicit MutableScaleEffect(ScaleEffect& copy);

  void Invalidate();

  ScaleEffect& mCopy;
  bool         mValid{true};
};

/**
 * @brief An immutable, predefined-based interaction effect that scales the view.
 *
 * ScaleEffect applies a scale animation on the host view when the view
 * enters the PRESSED or FOCUSED state, providing visual feedback to the user.
 *
 * State priority (PRESSED > FOCUSED > NORMAL):
 * - PRESSED: originalScale × pressedScale (default 0.9, regardless of focus state)
 * - FOCUSED: originalScale × focusedScale (default 1.18)
 * - NORMAL:  originalScale (restored)
 *
 * ScaleEffect instances can only be obtained from predefined factory methods
 * (Default()) or by calling Configure() on an existing instance to produce a
 * variant with adjusted parameters.
 *
 * @code
 * // Use the predefined effect as-is
 * view.SetInteractionEffect(ScaleEffect::Default());
 *
 * // Create a variant with custom focused scale
 * view.SetInteractionEffect(ScaleEffect::Default().Configure([](MutableScaleEffect& m) {
 *   m.SetFocusedScale(1.3f);
 * }));
 *
 * // Remove the effect
 * view.SetInteractionEffect(Trait{});
 * @endcode
 *
 * @see MutableScaleEffect
 * @see View::SetInteractionEffect
 */
class DALI_UI_API ScaleEffect : public Trait
{
public: // Predefined factories
  /**
   * @brief Returns the default ScaleEffect.
   *
   * focusedScale = 1.18, pressedScale = 0.9
   *
   * This is the value used by UiConfig when no explicit effect is set.
   *
   * @return A ScaleEffect handle (shared, no heap allocation per call)
   */
  static ScaleEffect Default();

public: // Configure (instance only — no static version)
  /**
   * @brief Creates a variant of this effect with adjusted parameters.
   *
   * Clones the current effect's parameters, passes a MutableScaleEffect proxy
   * to the configure lambda, then returns a new ScaleEffect.
   * This is the only way to create a ScaleEffect with custom parameters.
   *
   * The lambda must not store the MutableScaleEffect reference beyond its scope;
   * the proxy is invalidated immediately after the lambda returns.
   *
   * @param[in] configure Lambda receiving a MutableScaleEffect& for parameter adjustment
   * @return A new ScaleEffect with the adjusted parameters
   *
   * @code
   * auto custom = ScaleEffect::Default().Configure([](MutableScaleEffect& m) {
   *   m.SetPressedScale(0.85f);
   * });
   * @endcode
   */
  template<typename F>
  ScaleEffect Configure(F&& configure) const
  {
    ScaleEffect        copy = DoClone();
    MutableScaleEffect m(copy);
    configure(m);
    m.Invalidate();
    return copy;
  }

public: // Handle boilerplate
  /**
   * @brief Creates an uninitialized ScaleEffect handle.
   */
  ScaleEffect();

  /**
   * @brief Downcasts a Trait handle to a ScaleEffect handle.
   *
   * @param[in] handle Handle to downcast
   * @return A valid ScaleEffect handle if the handle points to a ScaleEffect; uninitialized otherwise
   */
  static ScaleEffect DownCast(BaseHandle handle);

  ScaleEffect(const ScaleEffect&)            = default;
  ScaleEffect(ScaleEffect&&)                 = default;
  ScaleEffect& operator=(const ScaleEffect&) = default;
  ScaleEffect& operator=(ScaleEffect&&)      = default;

  ~ScaleEffect() = default;

public: // Not intended for application developers
  /// @cond internal
  explicit ScaleEffect(Integration::ScaleEffectImpl* impl);
  /// @endcond

private:
  friend class MutableScaleEffect;

  ScaleEffect                   DoClone() const;
  Integration::ScaleEffectImpl& GetImpl() const;
};

} // namespace Ui

} // namespace Dali
