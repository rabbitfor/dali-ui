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
#include <dali/public-api/animation/animation.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/trait.h>

namespace Dali
{

namespace Ui
{

namespace Internal
{

/**
 * @brief Per-view data trait for ScaleEffect.
 *
 * Stores the view's original scale (captured at attachment time) and the
 * currently running scale animation. Created by ScaleEffectImpl::OnAttached
 * and stored in the INTERACTION_EFFECT_DATA trait slot.
 */
class ScaleEffectData : public Trait
{
public:
  /**
   * @brief Creates a new ScaleEffectData with the given original scale values.
   */
  static ScaleEffectData New(float originalScaleX, float originalScaleY);

  /**
   * @brief Downcasts a Trait handle to ScaleEffectData.
   *
   * @return A valid ScaleEffectData if the handle wraps ScaleEffectDataImpl; uninitialized otherwise
   */
  static ScaleEffectData DownCast(Trait handle);

  /// @cond internal
  explicit ScaleEffectData(Integration::TraitImpl* impl)
  : Trait(impl)
  {
  }
  /// @endcond

  float     GetOriginalScaleX() const;
  float     GetOriginalScaleY() const;
  Animation GetAnimation() const;
  void      SetAnimation(Animation anim);
};

} // namespace Internal

} // namespace Ui

} // namespace Dali
