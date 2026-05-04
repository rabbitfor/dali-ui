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
#include <dali/public-api/math/vector4.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/state-effect.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali-ui-foundation/public-api/view-types.h>

namespace Dali
{
namespace Ui
{
namespace OverlayEffect
{

/**
 * @brief Gets the default overlay state effect.
 *
 * Uses UiColor(0x000000, 0.1f) and follows the target View corner radius.
 *
 * @return A shared default overlay effect
 */
DALI_UI_API StateEffect Default();

/**
 * @brief Gets the round overlay state effect.
 *
 * Uses UiColor(0x000000, 0.1f) and relative 0.5 corner radius.
 *
 * @return A shared round overlay effect
 */
DALI_UI_API StateEffect Round();

/**
 * @brief Mutable configuration used to create overlay state effects.
 */
class DALI_UI_API Config
{
public:
  /**
   * @brief Creates a configuration with default overlay effect values.
   */
  Config();

  /**
   * @brief Creates a configuration by copying another configuration.
   *
   * @param[in] rhs The configuration to copy
   */
  Config(const Config& rhs);

  /**
   * @brief Creates a configuration by moving another configuration.
   *
   * @param[in] rhs The configuration to move
   */
  Config(Config&& rhs) noexcept;

  /**
   * @brief Copies another configuration to this configuration.
   *
   * @param[in] rhs The configuration to copy
   * @return Reference to this configuration
   */
  Config& operator=(const Config& rhs);

  /**
   * @brief Moves another configuration to this configuration.
   *
   * @param[in] rhs The configuration to move
   * @return Reference to this configuration
   */
  Config& operator=(Config&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~Config();

  /**
   * @brief Creates a configuration with default overlay effect values.
   *
   * @return A new default configuration
   */
  static Config Default();

  /**
   * @brief Creates a configuration with round overlay effect values.
   *
   * @return A new round configuration
   */
  static Config Round();

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
   * @brief Creates an overlay state effect from this configuration.
   *
   * @return A new state effect
   */
  StateEffect Build() const;

private:
  class Impl;
  Impl* mImpl{nullptr};
};

} // namespace OverlayEffect
} // namespace Ui
} // namespace Dali
