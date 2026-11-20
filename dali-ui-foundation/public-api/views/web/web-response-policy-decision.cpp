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

#include <dali-ui-foundation/public-api/views/web/web-response-policy-decision.h>
#include <utility>

namespace DALI_NAMESPACE
{
namespace Ui
{

struct WebResponsePolicyDecision::Impl
{
  Dali::String           mUrl;
  int32_t                mStatusCode;
  Dali::Callback<bool()> mUse;
  Dali::Callback<bool()> mIgnore;
};

WebResponsePolicyDecision::~WebResponsePolicyDecision() = default;

Dali::String WebResponsePolicyDecision::GetUrl() const
{
  return mImpl->mUrl;
}

int32_t WebResponsePolicyDecision::GetStatusCode() const
{
  return mImpl->mStatusCode;
}

bool WebResponsePolicyDecision::AllowResponse()
{
  return mImpl->mUse.Invoke();
}

bool WebResponsePolicyDecision::CancelResponse()
{
  return mImpl->mIgnore.Invoke();
}

WebResponsePolicyDecision::WebResponsePolicyDecision(Dali::String url, int32_t statusCode, Dali::Callback<bool()> use, Dali::Callback<bool()> ignore)
: mImpl(new Impl{std::move(url), statusCode, std::move(use), std::move(ignore)})
{
}

} // namespace Ui
} // namespace DALI_NAMESPACE
