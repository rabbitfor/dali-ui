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

#include <stdlib.h>
#include <iostream>

#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/integration-api/selectable-trait-impl.h>
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-test-suite-utils.h>
#include <test-gesture-generator.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/touch-event-integ.h>

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Integration;

namespace
{

// ============================================================================
// Signal callback helpers
// ============================================================================

struct SelectionChangedSignalData
{
  SelectionChangedSignalData()
  : called(false),
    selected(false)
  {
  }

  void Reset()
  {
    called   = false;
    selected = false;
    view     = View();
  }

  bool called;
  bool selected;
  View view;
};

struct SelectionChangedSignalFunctor
{
  SelectionChangedSignalFunctor(SelectionChangedSignalData& data)
  : signalData(data)
  {
  }

  void operator()(View view, bool selected)
  {
    signalData.called   = true;
    signalData.selected = selected;
    signalData.view     = view;
  }

  SelectionChangedSignalData& signalData;
};

/**
 * @brief Helper to create a View, add it to the scene, attach selectable, and render.
 */
View CreateSelectableView(TestApplication& application, float width = 100.0f, float height = 100.0f)
{
  View view = View::New();
  view.SetProperty(Actor::Property::SIZE, Vector2(width, height));
  view.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  application.GetScene().Add(view);
  view.AsSelectable();

  application.SendNotification();
  application.Render();

  return view;
}

} // namespace

void utc_dali_selectabletrait_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_selectabletrait_cleanup(void)
{
  test_return_value = TET_PASS;
}

// ============================================================================
// Construction / Handle tests
// ============================================================================

int UtcDaliSelectableTraitNewP(void)
{
  TestApplication    application;
  SelectableTrait selectable = SelectableTrait::New();
  DALI_TEST_CHECK(selectable);
  END_TEST;
}

int UtcDaliSelectableTraitCopyConstructorP(void)
{
  TestApplication    application;
  SelectableTrait selectable = SelectableTrait::New();
  SelectableTrait copy(selectable);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(copy == selectable);
  END_TEST;
}

int UtcDaliSelectableTraitDownCastP(void)
{
  TestApplication    application;
  SelectableTrait selectable = SelectableTrait::New();
  BaseHandle         handle(selectable);
  SelectableTrait downcast = SelectableTrait::DownCast(handle);
  DALI_TEST_CHECK(downcast);
  END_TEST;
}

int UtcDaliSelectableTraitDownCastN(void)
{
  TestApplication    application;
  BaseHandle         handle;
  SelectableTrait downcast = SelectableTrait::DownCast(handle);
  DALI_TEST_CHECK(!downcast);
  END_TEST;
}

// ============================================================================
// AsSelectable (View integration)
// ============================================================================

int UtcDaliViewAsSelectableP(void)
{
  TestApplication application;
  View view = View::New();

  View& result = view.AsSelectable();
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

  SelectableTrait selectable = view.GetSelectableTrait();
  DALI_TEST_CHECK(selectable);
  END_TEST;
}

int UtcDaliViewAsSelectableWithConfigureP(void)
{
  TestApplication application;
  bool configureCalled = false;

  View view = View::New();
  view.AsSelectable([&configureCalled](SelectableTrait& trait) {
    configureCalled = true;
    trait.EnableToggleByClick();
  });

  DALI_TEST_CHECK(configureCalled);

  SelectableTrait selectable = view.GetSelectableTrait();
  DALI_TEST_CHECK(selectable);
  DALI_TEST_CHECK(selectable.IsToggleByClickEnabled());
  END_TEST;
}

int UtcDaliViewAsSelectableIdempotentP(void)
{
  TestApplication application;
  View view = View::New();

  view.AsSelectable();
  SelectableTrait first = view.GetSelectableTrait();

  view.AsSelectable();
  SelectableTrait second = view.GetSelectableTrait();

  DALI_TEST_CHECK(first == second);
  END_TEST;
}

int UtcDaliViewGetSelectableTraitWithoutAttachN(void)
{
  TestApplication application;
  View view = View::New();

  SelectableTrait selectable = view.GetSelectableTrait();
  DALI_TEST_CHECK(!selectable);
  END_TEST;
}

int UtcDaliViewGetOrAttachSelectableTraitP(void)
{
  TestApplication application;
  View view = View::New();

  SelectableTrait selectable = view.GetOrAttachSelectableTrait();
  DALI_TEST_CHECK(selectable);

  // Second call returns the same trait
  SelectableTrait again = view.GetOrAttachSelectableTrait();
  DALI_TEST_CHECK(selectable == again);
  END_TEST;
}

// ============================================================================
// Clickable + Selectable coexistence
// ============================================================================

int UtcDaliViewClickableAndSelectableCoexistP(void)
{
  TestApplication application;
  View view = View::New();

  view.AsClickable();
  view.AsSelectable();

  ClickableTrait  clickable  = view.GetClickableTrait();
  SelectableTrait selectable = view.GetSelectableTrait();

  DALI_TEST_CHECK(clickable);
  DALI_TEST_CHECK(selectable);
  END_TEST;
}

int UtcDaliViewSelectableBeforeClickableP(void)
{
  TestApplication application;
  View view = View::New();

  // Selectable first, then clickable
  view.AsSelectable();
  view.AsClickable();

  ClickableTrait  clickable  = view.GetClickableTrait();
  SelectableTrait selectable = view.GetSelectableTrait();

  DALI_TEST_CHECK(clickable);
  DALI_TEST_CHECK(selectable);
  END_TEST;
}

// ============================================================================
// State API tests
// ============================================================================

int UtcDaliSelectableTraitIsSelectedDefaultP(void)
{
  TestApplication    application;
  SelectableTrait selectable = SelectableTrait::New();
  DALI_TEST_CHECK(!selectable.IsSelected());
  END_TEST;
}

int UtcDaliSelectableTraitSetSelectedP(void)
{
  TestApplication application;
  View view = CreateSelectableView(application);
  SelectableTrait selectable = view.GetSelectableTrait();

  selectable.SetSelected(true);
  DALI_TEST_CHECK(selectable.IsSelected());

  selectable.SetSelected(false);
  DALI_TEST_CHECK(!selectable.IsSelected());
  END_TEST;
}

int UtcDaliSelectableTraitSetSelectedNoChangeP(void)
{
  TestApplication application;
  View view = CreateSelectableView(application);
  SelectableTrait selectable = view.GetSelectableTrait();

  SelectionChangedSignalData    data;
  SelectionChangedSignalFunctor functor(data);
  selectable.SelectionChangedSignal().Connect(&application, functor);

  // Set to same value (already false)
  selectable.SetSelected(false);
  DALI_TEST_CHECK(!data.called); // Signal should NOT fire when no actual change
  END_TEST;
}

int UtcDaliSelectableTraitIsToggleByClickEnabledDefaultP(void)
{
  TestApplication    application;
  SelectableTrait selectable = SelectableTrait::New();
  DALI_TEST_CHECK(!selectable.IsToggleByClickEnabled());
  END_TEST;
}

int UtcDaliSelectableTraitEnableToggleByClickP(void)
{
  TestApplication    application;
  SelectableTrait selectable = SelectableTrait::New();

  selectable.EnableToggleByClick();
  DALI_TEST_CHECK(selectable.IsToggleByClickEnabled());

  selectable.EnableToggleByClick(false);
  DALI_TEST_CHECK(!selectable.IsToggleByClickEnabled());
  END_TEST;
}

// ============================================================================
// SelectionChangedSignal
// ============================================================================

int UtcDaliSelectableTraitSelectionChangedSignalP(void)
{
  TestApplication application;
  View view = CreateSelectableView(application);
  SelectableTrait selectable = view.GetSelectableTrait();

  SelectionChangedSignalData    data;
  SelectionChangedSignalFunctor functor(data);
  selectable.SelectionChangedSignal().Connect(&application, functor);

  selectable.SetSelected(true);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(data.selected);
  DALI_TEST_CHECK(data.view == view);

  data.Reset();

  selectable.SetSelected(false);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(!data.selected);
  END_TEST;
}

int UtcDaliViewAsSelectableWithSignalLambdaP(void)
{
  TestApplication application;

  bool signalCalled   = false;
  bool signalSelected = false;

  View view = View::New();
  view.AsSelectable(static_cast<Dali::ConnectionTrackerInterface*>(&application),
                    [&signalCalled, &signalSelected](View v, bool selected) {
                      signalCalled   = true;
                      signalSelected = selected;
                    });

  SelectableTrait selectable = view.GetSelectableTrait();
  DALI_TEST_CHECK(selectable);

  // Manually add to scene for proper lifecycle
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  selectable.SetSelected(true);

  DALI_TEST_CHECK(signalCalled);
  DALI_TEST_CHECK(signalSelected);
  END_TEST;
}

// ============================================================================
// Toggle-by-click interaction
// ============================================================================

int UtcDaliSelectableTraitToggleByClickP(void)
{
  TestApplication application;
  View view = View::New();
  view.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  view.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  application.GetScene().Add(view);

  view.AsSelectable([](SelectableTrait& trait) {
    trait.EnableToggleByClick();
  });

  application.SendNotification();
  application.Render();

  SelectableTrait selectable = view.GetSelectableTrait();

  // ClickableTrait should have been auto-created by EnableToggleByClick
  ClickableTrait clickable = view.GetClickableTrait();
  DALI_TEST_CHECK(clickable);

  SelectionChangedSignalData    data;
  SelectionChangedSignalFunctor functor(data);
  selectable.SelectionChangedSignal().Connect(&application, functor);

  DALI_TEST_CHECK(!selectable.IsSelected());

  // First tap: select
  TestGenerateTap(application, 50.0f, 50.0f, 100);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(data.selected);
  DALI_TEST_CHECK(selectable.IsSelected());

  data.Reset();

  // Second tap: deselect
  TestGenerateTap(application, 50.0f, 50.0f, 300);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(!data.selected);
  DALI_TEST_CHECK(!selectable.IsSelected());
  END_TEST;
}

int UtcDaliSelectableTraitToggleByClickAutoCreatesClickableP(void)
{
  TestApplication application;
  View view = View::New();
  view.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  view.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  application.GetScene().Add(view);

  // No clickable trait exists yet
  DALI_TEST_CHECK(!view.GetClickableTrait());

  // AsSelectable with toggle-by-click should auto-create clickable
  view.AsSelectable([](SelectableTrait& trait) {
    trait.EnableToggleByClick();
  });

  application.SendNotification();
  application.Render();

  // ClickableTrait should now exist
  DALI_TEST_CHECK(view.GetClickableTrait());
  END_TEST;
}

int UtcDaliSelectableTraitToggleByClickWithExistingClickableP(void)
{
  TestApplication application;
  View view = View::New();
  view.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  view.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  application.GetScene().Add(view);

  // Attach clickable first
  view.AsClickable();
  ClickableTrait existingClickable = view.GetClickableTrait();

  // Then attach selectable with toggle
  view.AsSelectable([](SelectableTrait& trait) {
    trait.EnableToggleByClick();
  });

  application.SendNotification();
  application.Render();

  // Should reuse the existing clickable, not create a new one
  ClickableTrait clickableAfter = view.GetClickableTrait();
  DALI_TEST_CHECK(existingClickable == clickableAfter);

  // Toggle should still work
  SelectableTrait selectable = view.GetSelectableTrait();
  TestGenerateTap(application, 50.0f, 50.0f, 100);
  DALI_TEST_CHECK(selectable.IsSelected());
  END_TEST;
}

int UtcDaliSelectableTraitToggleByClickDoesNotConsumeClickP(void)
{
  TestApplication application;
  View view = View::New();
  view.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  view.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  application.GetScene().Add(view);

  view.AsClickable();
  view.AsSelectable([](SelectableTrait& trait) {
    trait.EnableToggleByClick();
  });

  application.SendNotification();
  application.Render();

  // Connect a separate clicked handler
  bool appClickedCalled = false;
  view.GetClickableTrait().ClickedSignal().Connect(
    &application,
    [&appClickedCalled](View v, const InputEvent& e) -> bool {
      appClickedCalled = true;
      return false;
    });

  TestGenerateTap(application, 50.0f, 50.0f, 100);

  // Both should fire: selectable toggle AND app clicked handler
  DALI_TEST_CHECK(view.GetSelectableTrait().IsSelected());
  DALI_TEST_CHECK(appClickedCalled);
  END_TEST;
}

// ============================================================================
// Toggle-by-click via key event
// ============================================================================

int UtcDaliSelectableTraitToggleByKeyP(void)
{
  TestApplication application;
  View view = View::New();
  view.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  view.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  application.GetScene().Add(view);

  view.AsSelectable([](SelectableTrait& trait) {
    trait.EnableToggleByClick();
  });
  view.GetClickableTrait().SetKeyClickPolicy(KeyClickPolicy::ON_RELEASE);

  application.SendNotification();
  application.Render();

  Dali::Toolkit::KeyboardFocusManager::Get().SetCurrentFocusActor(view);
  application.SendNotification();
  application.Render();

  SelectableTrait selectable = view.GetSelectableTrait();
  DALI_TEST_CHECK(!selectable.IsSelected());

  // Key press + release → click → toggle
  Dali::Integration::KeyEvent keyDown(
    "Return", "", "", 0, 0, 100, Dali::Integration::KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(keyDown);

  Dali::Integration::KeyEvent keyUp(
    "Return", "", "", 0, 0, 120, Dali::Integration::KeyEvent::UP, "", "", Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(keyUp);

  DALI_TEST_CHECK(selectable.IsSelected());
  END_TEST;
}

// ============================================================================
// Enable/disable toggle-by-click after attachment
// ============================================================================

int UtcDaliSelectableTraitEnableToggleByClickAfterAttachP(void)
{
  TestApplication application;
  View view = View::New();
  view.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  view.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  application.GetScene().Add(view);

  // Attach selectable WITHOUT toggle-by-click
  view.AsSelectable();

  application.SendNotification();
  application.Render();

  SelectableTrait selectable = view.GetSelectableTrait();

  // Tap should NOT toggle
  TestGenerateTap(application, 50.0f, 50.0f, 100);
  DALI_TEST_CHECK(!selectable.IsSelected());

  // Now enable toggle-by-click
  selectable.EnableToggleByClick();

  // Tap should now toggle
  TestGenerateTap(application, 50.0f, 50.0f, 300);
  DALI_TEST_CHECK(selectable.IsSelected());
  END_TEST;
}

int UtcDaliSelectableTraitDisableToggleByClickP(void)
{
  TestApplication application;
  View view = View::New();
  view.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  view.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  application.GetScene().Add(view);

  view.AsSelectable([](SelectableTrait& trait) {
    trait.EnableToggleByClick();
  });

  application.SendNotification();
  application.Render();

  SelectableTrait selectable = view.GetSelectableTrait();

  // First tap works
  TestGenerateTap(application, 50.0f, 50.0f, 100);
  DALI_TEST_CHECK(selectable.IsSelected());

  // Disable toggle
  selectable.EnableToggleByClick(false);

  // Tap should no longer toggle
  TestGenerateTap(application, 50.0f, 50.0f, 300);
  DALI_TEST_CHECK(selectable.IsSelected()); // Still true, not toggled back
  END_TEST;
}

// ============================================================================
// Chaining API
// ============================================================================

int UtcDaliViewAsClickableAsSelectableChainingP(void)
{
  TestApplication application;
  View view = View::New();

  View& result = view.AsClickable().AsSelectable();
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

  DALI_TEST_CHECK(view.GetClickableTrait());
  DALI_TEST_CHECK(view.GetSelectableTrait());
  END_TEST;
}
