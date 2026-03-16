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
#include <dali-ui-foundation/integration-api/selectable-trait-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/clickable-trait.h>
#include <dali-ui-foundation/public-api/input-event.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali::Ui::Integration
{

SelectableTraitImpl::SelectableTraitImpl()
: TraitImpl(),
  mSelectionChangedSignal(),
  mSelected(false),
  mToggleByClickEnabled(false),
  mAttached(false)
{
}

SelectableTraitImpl::~SelectableTraitImpl()
{
}

Signal<void(View, bool)>& SelectableTraitImpl::SelectionChangedSignal()
{
  return mSelectionChangedSignal;
}

bool SelectableTraitImpl::IsSelected() const
{
  return mSelected;
}

void SelectableTraitImpl::SetSelected(bool selected)
{
  if(mSelected == selected)
  {
    return;
  }

  View owner = mOwner.GetHandle();
  if(!owner)
  {
    // Not attached yet, just store the state
    mSelected = selected;
    return;
  }

  if(!OnSelectionChanging(owner, selected))
  {
    return;
  }

  mSelected = selected;
  mSelectionChangedSignal.Emit(owner, mSelected);
}

bool SelectableTraitImpl::IsToggleByClickEnabled() const
{
  return mToggleByClickEnabled;
}

void SelectableTraitImpl::EnableToggleByClick(bool enabled)
{
  if(mToggleByClickEnabled == enabled)
  {
    return;
  }

  mToggleByClickEnabled = enabled;

  if(mAttached)
  {
    if(enabled)
    {
      EnsureClickableAndConnect();
    }
    else
    {
      DisconnectClickable();
    }
  }
}

View SelectableTraitImpl::GetOwner() const
{
  return mOwner.GetHandle();
}

void SelectableTraitImpl::OnBeforeAttached(TraitId id, View& view)
{
  DALI_ASSERT_ALWAYS(!(mOwner.GetHandle()) && "The trait can not be attached multiple target views");
  mOwner = view;
}

void SelectableTraitImpl::OnAttached(TraitId id, View& view)
{
  mAttached = true;

  if(mToggleByClickEnabled)
  {
    EnsureClickableAndConnect();
  }
}

void SelectableTraitImpl::OnDetached(TraitId id, View& view)
{
  DisconnectClickable();
  mAttached = false;
  mOwner.Reset();
}

void SelectableTraitImpl::OnViewDestroying(ViewImpl* viewImpl)
{
}

bool SelectableTraitImpl::OnSelectionChanging(View view, bool newSelected)
{
  return true;
}

void SelectableTraitImpl::EnsureClickableAndConnect()
{
  View owner = mOwner.GetHandle();
  if(!owner)
  {
    return;
  }

  // Get or create ClickableTrait on the owner view
  ClickableTrait clickable = owner.GetOrAttachClickableTrait();
  if(clickable)
  {
    clickable.ClickedSignal().Connect(this, &SelectableTraitImpl::OnClickedForToggle);
  }
}

void SelectableTraitImpl::DisconnectClickable()
{
  View owner = mOwner.GetHandle();
  if(!owner)
  {
    return;
  }

  ClickableTrait clickable = owner.GetClickableTrait();
  if(clickable)
  {
    clickable.ClickedSignal().Disconnect(this, &SelectableTraitImpl::OnClickedForToggle);
  }
}

bool SelectableTraitImpl::OnClickedForToggle(View view, const InputEvent& event)
{
  SetSelected(!mSelected);

  // Always return false so that application-level ClickedSignal handlers
  // are still invoked after the selection toggle.
  return false;
}

} // namespace Dali::Ui::Integration
