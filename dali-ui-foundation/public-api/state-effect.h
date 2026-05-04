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

#include <dali/public-api/common/intrusive-ptr.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/trait-object.h>

namespace Dali
{
namespace Ui
{

class StateEvent;
class View;

namespace Internal
{
class ViewStateManager;
}

/**
 * @brief Base object for effects that react to ViewState changes.
 */
class DALI_UI_API StateEffectObject : public TraitObject
{
protected:
  StateEffectObject();
  ~StateEffectObject() override;

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

/**
 * @brief Shared state effect object, or nullptr to disable an effect.
 */
using StateEffect = IntrusivePtr<StateEffectObject>;

} // namespace Ui
} // namespace Dali
