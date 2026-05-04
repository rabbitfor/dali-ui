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
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/math/vector4.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/state-effect.h>
#include <dali-ui-foundation/public-api/trait.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali-ui-foundation/public-api/view-types.h>

namespace Dali
{
namespace Ui
{

namespace Internal
{
class OverlayEffectImpl;
}

/**
 * @brief State effect that adds a dim overlay child while a View is pressed or focused.
 */
class DALI_UI_API OverlayEffect : public StateEffect
{
public:
  class Config;

  /**
   * @brief Default overlay effect.
   *
   * Uses UiColor(0x000000, 0.1f) and follows the target View corner radius.
   *
   * @return A shared default overlay effect
   */
  static const OverlayEffect& Default();

  /**
   * @brief Round overlay effect.
   *
   * Uses UiColor(0x000000, 0.1f) and relative 0.5 corner radius.
   *
   * @return A shared round overlay effect
   */
  static const OverlayEffect& Round();

  /**
   * @brief Creates an uninitialized OverlayEffect handle.
   */
  OverlayEffect() = default;

  /**
   * @brief Downcasts a handle to OverlayEffect.
   *
   * @param[in] handle The handle to downcast
   * @return An initialized OverlayEffect on success, otherwise empty
   */
  static OverlayEffect DownCast(BaseHandle handle);

  /**
   * @brief Gets the overlay child background color.
   *
   * @return The overlay color
   */
  UiColor GetOverlayColor() const;

  /**
   * @brief Gets the configured corner radius.
   *
   * If this effect follows the target corner radius, this returns the stored
   * fallback radius value.
   *
   * @return The configured corner radius
   */
  Vector4 GetCornerRadius() const;

  /**
   * @brief Gets the configured corner radius policy.
   *
   * If this effect follows the target corner radius, this returns the stored
   * fallback policy value.
   *
   * @return The configured corner radius policy
   */
  CornerRadiusPolicy GetCornerRadiusPolicy() const;

  /**
   * @brief Creates a mutable configuration copied from this effect.
   *
   * @return A configuration object
   */
  Config Configure() const;

private:
  explicit OverlayEffect(Internal::OverlayEffectImpl* impl);

  friend class Config;
};

/**
 * @brief Mutable configuration used to create immutable OverlayEffect handles.
 */
class DALI_UI_API OverlayEffect::Config
{
public:
  /**
   * @brief Creates a new configuration with default overlay effect values.
   *
   * @return A new configuration
   */
  static Config New();

  Config(Config&& rhs) noexcept;
  Config& operator=(Config&& rhs) noexcept;

  Config(const Config&)            = delete;
  Config& operator=(const Config&) = delete;

  ~Config();

  /**
   * @brief Sets the overlay child background color.
   *
   * @param[in] color The overlay color
   * @return Reference to this configuration
   */
  Config& SetOverlayColor(const UiColor& color);

  /**
   * @brief Gets the overlay child background color.
   *
   * @return The overlay color
   */
  UiColor GetOverlayColor() const;

  /**
   * @brief Sets a uniform explicit corner radius.
   *
   * @param[in] radius The radius for all corners
   * @return Reference to this configuration
   */
  Config& SetCornerRadius(float radius);

  /**
   * @brief Sets explicit corner radii.
   *
   * @param[in] topLeft The top-left radius
   * @param[in] topRight The top-right radius
   * @param[in] bottomRight The bottom-right radius
   * @param[in] bottomLeft The bottom-left radius
   * @return Reference to this configuration
   */
  Config& SetCornerRadius(float topLeft, float topRight, float bottomRight, float bottomLeft);

  /**
   * @brief Sets explicit corner radii.
   *
   * @param[in] radius The corner radii
   * @return Reference to this configuration
   */
  Config& SetCornerRadius(const Vector4& radius);

  /**
   * @brief Gets the configured corner radius.
   *
   * @return The configured corner radius
   */
  Vector4 GetCornerRadius() const;

  /**
   * @brief Sets an explicit corner radius policy.
   *
   * @param[in] policy The corner radius policy
   * @return Reference to this configuration
   */
  Config& SetCornerRadiusPolicy(CornerRadiusPolicy policy);

  /**
   * @brief Sets the corner radius policy to relative.
   *
   * @return Reference to this configuration
   */
  Config& SetCornerRadiusPolicyRelative();

  /**
   * @brief Gets the configured corner radius policy.
   *
   * @return The configured corner radius policy
   */
  CornerRadiusPolicy GetCornerRadiusPolicy() const;

  /**
   * @brief Creates an immutable OverlayEffect from this configuration.
   *
   * This consumes the configuration and transfers its implementation without
   * cloning.
   *
   * @return A new OverlayEffect
   */
  OverlayEffect Build();

private:
  explicit Config(Internal::OverlayEffectImpl* impl);

  friend class OverlayEffect;

private:
  IntrusivePtr<Internal::OverlayEffectImpl> mImpl;
};

} // namespace Ui
} // namespace Dali
