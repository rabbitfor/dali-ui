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
#include <dali-ui-foundation/integration-api/trait-impl.h>
#include <dali-ui-foundation/public-api/state-effect.h>
#include <dali-ui-foundation/public-api/state-event.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{
namespace Ui
{

namespace Internal
{
class ViewStateManager;
}

namespace Integration
{

/**
 * @brief Base implementation for effects that react to ViewState changes.
 */
class DALI_UI_API StateEffectImpl : public TraitImpl
{
protected:
  StateEffectImpl();
  ~StateEffectImpl() override;

  /**
   * @brief Called when the attached View state changes.
   *
   * @param[in] view The View whose state changed
   * @param[in] event The state transition event
   */
  virtual void OnViewStateChanged(View view, const StateEvent& event);

private:
  friend class Internal::ViewStateManager;
};

} // namespace Integration

inline DALI_UI_API Integration::StateEffectImpl& GetImpl(StateEffect& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Integration::StateEffectImpl&>(handle);
}

inline DALI_UI_API const Integration::StateEffectImpl& GetImpl(const StateEffect& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Integration::StateEffectImpl&>(handle);
}

} // namespace Ui
} // namespace Dali
