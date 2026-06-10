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

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Integration;

namespace
{

void TVViewInitializer(View view)
{
  UiConfig::ApplyDefaultViewInitializer(view);

  view.SetBackgroundColor(UiColor(0x00FFFF));

  Label label = Label::DownCast(view);
  if(label)
  {
    label.SetTextColor(UiColor(0x0000FF));
  }
}

} // unnamed namespace

class TVConfigImpl : public UiConfigImpl
{
};

TVConfig::TVConfig(UiConfigImpl* impl)
: UiConfig(impl)
{
}

TVConfig TVConfig::New()
{
  IntrusivePtr<TVConfigImpl> impl = new TVConfigImpl();
  TVConfig                   handle(impl.Get());
  return handle;
}

void TVConfig::SetInitializer()
{
  SetViewInitializer(TVViewInitializer);
}
