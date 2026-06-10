/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
 */

#include "tv-config.h"

#include "tv-ext.h"

#include <dali/public-api/actors/actor.h>

namespace
{

void ApplyTVViewInitializer(Dali::Ui::View view)
{
  view.SetProperty(Dali::Actor::Property::PIVOT, Dali::Pivot::TOP_LEFT);
  view.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, Dali::ParentOrigin::TOP_LEFT);
  view.SetProperty(Dali::Actor::Property::POSITION_USES_PIVOT, false);
  TVExt::AttachFluxViewData(view);
}

} // unnamed namespace

TVConfig TVConfig::New()
{
  TVConfig config(Dali::Ui::UiConfig::New());
  config.SetViewInitializer(ApplyTVViewInitializer);
  return config;
}
