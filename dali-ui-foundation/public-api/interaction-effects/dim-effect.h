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
class DimEffectImpl;
}

class DimEffect;

/**
 * @brief Mutable proxy for configuring a DimEffect inside DimEffect::Configure().
 *
 * MutableDimEffect is a non-owning, non-copyable proxy that forwards setter calls
 * to the underlying DimEffectImpl. It can only be obtained inside the lambda passed
 * to DimEffect::Configure() — there is intentionally no public constructor.
 *
 * Attempting to store a pointer or reference to a MutableDimEffect outside the
 * lambda scope will result in a runtime assertion on the next setter call, because
 * the proxy is invalidated immediately after the lambda returns.
 *
 * @code
 * auto effect = DimEffect::Default().Configure([](MutableDimEffect& m) {
 *   m.SetPressedOpacity(0.4f); // all other params stay at Default values
 * });
 * @endcode
 */
class DALI_UI_API MutableDimEffect
{
public:
  /**
   * @brief Sets the overlay opacity applied when the view is pressed.
   *
   * @param[in] opacity Value in [0, 1]; higher = darker dim
   * @return Reference to this for method chaining
   */
  MutableDimEffect& SetPressedOpacity(float opacity);

  /**
   * @brief Sets the overlay opacity applied when the view has keyboard focus.
   *
   * @param[in] opacity Value in [0, 1]; higher = darker dim
   * @return Reference to this for method chaining
   */
  MutableDimEffect& SetFocusedOpacity(float opacity);

  // --- Non-copyable, non-movable, non-addressable ---
  MutableDimEffect(const MutableDimEffect&)            = delete;
  MutableDimEffect(MutableDimEffect&&)                 = delete;
  MutableDimEffect& operator=(const MutableDimEffect&) = delete;
  MutableDimEffect& operator=(MutableDimEffect&&)      = delete;

  /// @cond internal
  void* operator&() = delete; ///< Prevents taking a pointer to this proxy
  /// @endcond

private:
  friend class DimEffect;

  explicit MutableDimEffect(DimEffect& copy);

  void Invalidate();

  DimEffect& mCopy;
  bool       mValid{true};
};

/**
 * @brief An immutable, predefined-based interaction effect that dims the view.
 *
 * DimEffect applies a semi-transparent overlay on the host view when the view
 * enters the PRESSED or FOCUSED state, providing visual feedback to the user.
 *
 * DimEffect instances can only be obtained from predefined factory methods
 * (Default(), Deep()) or by calling Configure() on an existing instance to
 * produce a variant with adjusted parameters. There is no public New() and
 * no setters — modification is only possible through Configure().
 *
 * @code
 * // Use a predefined effect as-is
 * view.SetInteractionEffect(DimEffect::Default());
 *
 * // Create a variant of a predefined effect
 * view.SetInteractionEffect(DimEffect::Deep().Configure([](MutableDimEffect& m) {
 *   m.SetFocusedOpacity(0.05f);
 * }));
 *
 * // Remove the effect
 * view.SetInteractionEffect(Trait{});
 * @endcode
 *
 * @see MutableDimEffect
 * @see View::SetInteractionEffect
 */
class DALI_UI_API DimEffect : public Trait
{
public: // Predefined factories
  /**
   * @brief Returns the default DimEffect (subtle press + focus feedback).
   *
   * pressedOpacity = 0.15, focusedOpacity = 0.08
   *
   * This is the value used by UiConfig when no explicit effect is set.
   *
   * @return A frozen DimEffect handle (shared, no heap allocation)
   */
  static DimEffect Default();

  /**
   * @brief Returns a stronger DimEffect for high-contrast scenarios.
   *
   * pressedOpacity = 0.30, focusedOpacity = 0.15
   *
   * @return A frozen DimEffect handle (shared, no heap allocation)
   */
  static DimEffect Deep();

public: // Configure (instance only — no static version)
  /**
   * @brief Creates a variant of this effect with adjusted parameters.
   *
   * Clones the current effect's parameters, passes a MutableDimEffect proxy
   * to the configure lambda, then returns a new frozen DimEffect.
   * This is the only way to create a DimEffect with custom parameters.
   *
   * The lambda must not store the MutableDimEffect reference beyond its scope;
   * the proxy is invalidated immediately after the lambda returns.
   *
   * @param[in] configure Lambda receiving a MutableDimEffect& for parameter adjustment
   * @return A new frozen DimEffect with the adjusted parameters
   *
   * @code
   * auto custom = DimEffect::Default().Configure([](MutableDimEffect& m) {
   *   m.SetPressedOpacity(0.4f);
   * });
   * @endcode
   */
  template<typename F>
  DimEffect Configure(F&& configure) const
  {
    DimEffect        copy = DoClone(); // non-template, defined in .cpp
    MutableDimEffect m(copy);
    configure(m);
    m.Invalidate();
    return copy;
  }

public: // Handle boilerplate
  /**
   * @brief Creates an uninitialized DimEffect handle.
   *
   * Only exists to satisfy handle conventions (e.g. returning from DownCast on failure).
   * An uninitialized handle is not a valid effect and cannot be passed to SetInteractionEffect.
   */
  DimEffect();

  /**
   * @brief Downcasts a Trait handle to a DimEffect handle.
   *
   * @param[in] handle Handle to downcast
   * @return A valid DimEffect handle if the handle points to a DimEffect; uninitialized otherwise
   */
  static DimEffect DownCast(BaseHandle handle);

  DimEffect(const DimEffect&)            = default;
  DimEffect(DimEffect&&)                 = default;
  DimEffect& operator=(const DimEffect&) = default;
  DimEffect& operator=(DimEffect&&)      = default;

  ~DimEffect() = default;

public: // Not intended for application developers
  /// @cond internal
  explicit DimEffect(Integration::DimEffectImpl* impl);
  /// @endcond

private:
  DimEffect                   DoClone() const; ///< Returns unfrozen copy — defined in .cpp where DimEffectImpl is complete
  Integration::DimEffectImpl& GetImpl() const;
};

} // namespace Ui

} // namespace Dali
