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

#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/integration-api/reserved-trait-id.h>
#include <dali-ui-foundation/integration-api/state-effect-impl.h>
#include <dali-ui-foundation/integration-api/view-integ.h>
#include <dali-ui-test-suite-utils.h>
#include <test-gesture-generator.h>
#include <dali/integration-api/events/touch-event-integ.h>

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Integration;

namespace
{

class TestStateEffectImpl : public StateEffectImpl
{
public:
  int attachedCount{0};
  int detachingCount{0};
  int stateChangedCount{0};
  int interactiveAttachedCount{0};

  void OnAttached(TraitId /*id*/, View& /*view*/) override
  {
    ++attachedCount;
  }

  void OnDetaching(TraitId /*id*/, View& /*view*/) override
  {
    ++detachingCount;
  }

protected:
  void OnViewStateChanged(View /*view*/, const StateEvent& /*event*/) override
  {
    ++stateChangedCount;
  }

  void OnInteractiveAttached(View /*view*/) override
  {
    ++interactiveAttachedCount;
  }
};

class TestStateEffect : public StateEffect
{
public:
  static TestStateEffect New(TestStateEffectImpl*& impl)
  {
    impl = new TestStateEffectImpl();
    return TestStateEffect(impl);
  }

private:
  explicit TestStateEffect(TestStateEffectImpl* impl)
  : StateEffect(impl)
  {
  }
};

View CreateTestView(UiTestApplication& application)
{
  View view = View::New();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  view.SetPivot(Pivot::TOP_LEFT);
  view.SetParentOrigin(ParentOrigin::TOP_LEFT);

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  return view;
}

void ProcessTouch(UiTestApplication& application, PointState::Type state, uint32_t time = 100u)
{
  Dali::Integration::TouchEvent touchEvent;
  Dali::Integration::Point      point;
  point.SetState(state);
  point.SetScreenPosition(Vector2(50.0f, 50.0f));
  point.SetDeviceId(1);
  point.SetDeviceClass(Device::Class::TOUCH);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  touchEvent.points.push_back(point);
  touchEvent.time = time;
  application.ProcessEvent(touchEvent);
}

} // namespace

void utc_dali_state_effect_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_state_effect_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliStateEffectNoneP(void)
{
  StateEffect none = StateEffect::None();

  DALI_TEST_CHECK(none);
  DALI_TEST_CHECK(none.IsNone());
  DALI_TEST_CHECK(!StateEffect().IsNone());
  END_TEST;
}

int UtcDaliStateEffectAttachDetachP(void)
{
  UiTestApplication application;
  View              view = CreateTestView(application);

  TestStateEffectImpl* impl   = nullptr;
  StateEffect          effect = TestStateEffect::New(impl);

  view.SetStateEffect(effect);

  DALI_TEST_EQUALS(impl->attachedCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(impl->detachingCount, 0, TEST_LOCATION);
  DALI_TEST_CHECK(IntegrationView::GetTrait(GetImpl(view), ReservedTraitId::STATE_EFFECT));

  view.SetStateEffect(StateEffect::None());

  DALI_TEST_EQUALS(impl->detachingCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(IntegrationView::GetTrait(GetImpl(view), ReservedTraitId::STATE_EFFECT));
  END_TEST;
}

int UtcDaliStateEffectInteractiveAttachedP(void)
{
  UiTestApplication application;
  View              view = CreateTestView(application);

  TestStateEffectImpl* impl   = nullptr;
  StateEffect          effect = TestStateEffect::New(impl);

  view.SetStateEffect(effect);
  DALI_TEST_EQUALS(impl->interactiveAttachedCount, 0, TEST_LOCATION);

  view.AsInteractive();
  DALI_TEST_EQUALS(impl->interactiveAttachedCount, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStateEffectViewStateChangedP(void)
{
  UiTestApplication application;
  View              view = CreateTestView(application);

  TestStateEffectImpl* impl   = nullptr;
  StateEffect          effect = TestStateEffect::New(impl);

  view.SetStateEffect(effect);
  view.AsInteractive();

  ProcessTouch(application, PointState::DOWN);
  DALI_TEST_CHECK(impl->stateChangedCount > 0);
  END_TEST;
}
