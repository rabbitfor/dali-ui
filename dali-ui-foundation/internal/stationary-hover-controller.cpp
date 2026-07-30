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
#include <dali-ui-foundation/internal/stationary-hover-controller.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/events/wheel-event.h>
#include <dali/public-api/object/weak-handle.h>
#include <algorithm>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{
constexpr uint32_t HOVER_REEVALUATION_INTERVAL_MILLISECONDS = 48u;
constexpr uint32_t WHEEL_FALLBACK_SAMPLE_COUNT              = 10u;
} //namespace

class StationaryHoverController::WindowTracker : public ConnectionTracker
{
public:
  explicit WindowTracker(Dali::Integration::SceneHolder sceneHolder)
  : mSceneHolder(sceneHolder)
  {
    Layer rootLayer = sceneHolder.GetRootLayer();
    rootLayer.InterceptWheelEventSignal().Connect(this, &WindowTracker::OnWheelEvent);
  }

  bool Matches(Dali::Integration::SceneHolder sceneHolder) const
  {
    return mSceneHolder.GetHandle() == sceneHolder;
  }

  bool IsAlive() const
  {
    return static_cast<bool>(mSceneHolder.GetHandle());
  }

  void BeginScroll(Actor actor)
  {
    PruneScrollSources();
    auto source = std::find_if(
      mScrollSources.begin(),
      mScrollSources.end(),
      [actor](const WeakHandle<Actor>& candidate)
    { return candidate.GetHandle() == actor; });
    if(source == mScrollSources.end())
    {
      mScrollSources.emplace_back(actor);
    }

    // The explicit lifecycle is authoritative for standard dali-ui scrollers.
    // Cancel the wheel fallback that may have been armed by the same input.
    mWheelFallbackSamplesRemaining = 0u;
    StartTimer();
  }

  bool EndScroll(Actor actor)
  {
    auto source = std::find_if(
      mScrollSources.begin(),
      mScrollSources.end(),
      [actor](const WeakHandle<Actor>& candidate)
    { return candidate.GetHandle() == actor; });
    if(source == mScrollSources.end())
    {
      return false;
    }

    mScrollSources.erase(source);
    RequestReevaluation();
    if(mScrollSources.empty() && mWheelFallbackSamplesRemaining == 0u && mTimer)
    {
      mTimer.Stop();
    }
    return true;
  }

private:
  bool OnWheelEvent(Actor, WheelEvent)
  {
    PruneScrollSources();
    if(!mScrollSources.empty())
    {
      return false;
    }

    // Custom wheel-driven scrollers do not expose a lifecycle to dali-ui.
    // Sample a bounded trailing window so they get the legacy FeedHover-style
    // behavior without keeping a permanent timer alive.
    mWheelFallbackSamplesRemaining = WHEEL_FALLBACK_SAMPLE_COUNT;
    StartTimer();
    return false;
  }

  bool OnTimerTick()
  {
    if(!IsAlive())
    {
      return false;
    }

    PruneScrollSources();
    if(!mScrollSources.empty())
    {
      RequestReevaluation();
      return true;
    }

    if(mWheelFallbackSamplesRemaining > 0u)
    {
      --mWheelFallbackSamplesRemaining;
      RequestReevaluation();
      return mWheelFallbackSamplesRemaining > 0u;
    }

    return false;
  }

  void StartTimer()
  {
    if(!mTimer)
    {
      mTimer = Timer::New(HOVER_REEVALUATION_INTERVAL_MILLISECONDS);
      mTimer.TickSignal().Connect(this, &WindowTracker::OnTimerTick);
    }
    mTimer.Start();
  }

  void RequestReevaluation()
  {
    Dali::Integration::SceneHolder sceneHolder = mSceneHolder.GetHandle();
    if(sceneHolder)
    {
      sceneHolder.RequestHoverReevaluation();
    }
  }

  void PruneScrollSources()
  {
    Dali::Integration::SceneHolder trackedSceneHolder = mSceneHolder.GetHandle();
    mScrollSources.erase(
      std::remove_if(
        mScrollSources.begin(),
        mScrollSources.end(),
        [trackedSceneHolder](const WeakHandle<Actor>& source)
    {
      Actor actor = source.GetHandle();
      return !actor || Dali::Integration::SceneHolder::Get(actor) != trackedSceneHolder;
    }),
      mScrollSources.end());
  }

private:
  WeakHandle<Dali::Integration::SceneHolder> mSceneHolder;
  std::vector<WeakHandle<Actor>>             mScrollSources;
  Timer                                      mTimer;
  uint32_t                                   mWheelFallbackSamplesRemaining{0u};
};

StationaryHoverController& StationaryHoverController::Get()
{
  static StationaryHoverController controller;
  return controller;
}

StationaryHoverController::~StationaryHoverController() = default;

void StationaryHoverController::SetEnabled(bool enabled)
{
  DisconnectAll();
  mWindowTrackers.clear();
  mEnabled = enabled && Adaptor::IsAvailable();
  if(!mEnabled)
  {
    return;
  }

  Adaptor& adaptor = Adaptor::Get();
  for(Dali::Integration::SceneHolder sceneHolder : adaptor.GetSceneHolders())
  {
    RegisterSceneHolder(sceneHolder);
  }
  adaptor.WindowCreatedSignal().Connect(this, &StationaryHoverController::OnSceneHolderCreated);
}

void StationaryHoverController::BeginScroll(Actor actor)
{
  if(!mEnabled || !actor)
  {
    return;
  }

  Dali::Integration::SceneHolder sceneHolder = Dali::Integration::SceneHolder::Get(actor);
  if(!sceneHolder)
  {
    return;
  }

  WindowTracker* tracker = FindTracker(sceneHolder);
  if(!tracker)
  {
    RegisterSceneHolder(sceneHolder);
    tracker = FindTracker(sceneHolder);
  }

  if(tracker)
  {
    tracker->BeginScroll(actor);
  }
}

void StationaryHoverController::EndScroll(Actor actor)
{
  if(!mEnabled || !actor)
  {
    return;
  }

  for(const auto& tracker : mWindowTrackers)
  {
    if(tracker->EndScroll(actor))
    {
      return;
    }
  }
}

void StationaryHoverController::RegisterSceneHolder(Dali::Integration::SceneHolder sceneHolder)
{
  if(!sceneHolder)
  {
    return;
  }

  mWindowTrackers.erase(
    std::remove_if(
      mWindowTrackers.begin(),
      mWindowTrackers.end(),
      [](const std::unique_ptr<WindowTracker>& tracker)
  { return !tracker->IsAlive(); }),
    mWindowTrackers.end());

  if(!FindTracker(sceneHolder))
  {
    mWindowTrackers.emplace_back(new WindowTracker(sceneHolder));
  }
}

StationaryHoverController::WindowTracker* StationaryHoverController::FindTracker(Dali::Integration::SceneHolder sceneHolder)
{
  auto tracker = std::find_if(
    mWindowTrackers.begin(),
    mWindowTrackers.end(),
    [sceneHolder](const std::unique_ptr<WindowTracker>& candidate)
  { return candidate->Matches(sceneHolder); });
  return tracker == mWindowTrackers.end() ? nullptr : tracker->get();
}

void StationaryHoverController::OnSceneHolderCreated(Dali::Integration::SceneHolder sceneHolder)
{
  RegisterSceneHolder(sceneHolder);
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
