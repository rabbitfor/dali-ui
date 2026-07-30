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
#include <dali/public-api/signals/connection-tracker.h>
#include <memory>
#include <vector>

namespace Dali
{
class Actor;

namespace Integration
{
class SceneHolder;
}

namespace Ui
{
namespace Internal
{

/**
 * @brief Applies the dali-ui stationary-hover policy to each SceneHolder.
 *
 * Each SceneHolder owns one periodic sampler. Standard scroll containers keep
 * it active for their full scroll lifecycle, while wheel input starts a
 * bounded fallback for custom wheel-driven scrolling.
 */
class StationaryHoverController : public ConnectionTracker
{
public:
  static StationaryHoverController& Get();

  /**
   * @brief Enables or disables observation of SceneHolders.
   */
  void SetEnabled(bool enabled);

  /**
   * @brief Starts periodic reevaluation for the scroll containing actor.
   */
  void BeginScroll(Actor actor);

  /**
   * @brief Stops periodic reevaluation and requests a final reevaluation.
   */
  void EndScroll(Actor actor);

private:
  class WindowTracker;

  StationaryHoverController() = default;
  ~StationaryHoverController() override;

  StationaryHoverController(const StationaryHoverController&)            = delete;
  StationaryHoverController(StationaryHoverController&&)                 = delete;
  StationaryHoverController& operator=(const StationaryHoverController&) = delete;
  StationaryHoverController& operator=(StationaryHoverController&&)      = delete;

  void           RegisterSceneHolder(Dali::Integration::SceneHolder sceneHolder);
  WindowTracker* FindTracker(Dali::Integration::SceneHolder sceneHolder);
  void           OnSceneHolderCreated(Dali::Integration::SceneHolder sceneHolder);

private:
  std::vector<std::unique_ptr<WindowTracker>> mWindowTrackers;
  bool                                        mEnabled{false};
};

} // namespace Internal
} // namespace Ui
} // namespace Dali
