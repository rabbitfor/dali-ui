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
#include <dali-ui-foundation/extension-api/web-request-cancellation-info.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-async-request.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Extension
{
struct WebRequestCancellationInfo::Impl
{
  uint64_t            id;
  uint32_t            viewId;
  Dali::String        url;
  Dali::String        method;
  Dali::Property::Map headers;
};
} //namespace Extension
} //namespace Ui
} //namespace DALI_NAMESPACE
