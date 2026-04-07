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

// CLASS HEADER
#include <dali-ui-foundation/integration-api/dim-effect-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/state-event.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{

namespace Ui
{

namespace Integration
{

DimEffectImpl::DimEffectImpl(const Params& params)
: mParams(params)
{
}

DimEffectImpl::~DimEffectImpl()
{
}

void DimEffectImpl::SetPressedOpacity(float opacity)
{
  mParams.pressedOpacity = opacity;
}

void DimEffectImpl::SetFocusedOpacity(float opacity)
{
  mParams.focusedOpacity = opacity;
}

float DimEffectImpl::GetPressedOpacity() const
{
  return mParams.pressedOpacity;
}

float DimEffectImpl::GetFocusedOpacity() const
{
  return mParams.focusedOpacity;
}

DimEffectImpl* DimEffectImpl::Clone() const
{
  return new DimEffectImpl(mParams);
}

void DimEffectImpl::OnBeforeAttached(TraitId /*id*/, View& /*view*/)
{
}

void DimEffectImpl::OnAttached(TraitId /*id*/, View& view)
{
  mOwner = view;
  view.StateChangedSignal().Connect(this, &DimEffectImpl::OnViewStateChangedInternal);
}

void DimEffectImpl::OnDetached(TraitId /*id*/, View& view)
{
  view.StateChangedSignal().Disconnect(this, &DimEffectImpl::OnViewStateChangedInternal);
  mOwner.Reset();

  // TODO: revert any visual changes applied to the view
}

void DimEffectImpl::OnViewDestroying(ViewImpl* /*viewImpl*/)
{
  mOwner.Reset();
}

void DimEffectImpl::OnStateChanged(View /*view*/, const StateEvent& /*event*/)
{
  // TODO: apply dim overlay based on event.GetCurrent()
  // Priority: PRESSED > FOCUSED > NORMAL
}

void DimEffectImpl::OnViewStateChangedInternal(View view, const StateEvent& event)
{
  OnStateChanged(view, event);
}

} // namespace Integration

} // namespace Ui

} // namespace Dali
