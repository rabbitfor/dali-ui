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

#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/signals/callback.h>
#include <cstdint>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration DALI_INTERNAL
{
class WebViewImpl;
} //namespace Integration DALI_INTERNAL

/**
 * @brief Inspects a response and decides how the engine handles it.
 * The object is valid only during the synchronous response policy callback.
 * If the application makes no decision, the engine applies its default policy.
 * The effect of multiple decision calls is backend dependent.
 * @SINCE_3_0.3
 */
class DALI_UI_API WebResponsePolicyDecision
{
public:
  /// @brief Destroys the wrapper. @SINCE_3_0.3
  ~WebResponsePolicyDecision();

  WebResponsePolicyDecision(const WebResponsePolicyDecision&)            = delete;
  WebResponsePolicyDecision(WebResponsePolicyDecision&&)                 = delete;
  WebResponsePolicyDecision& operator=(const WebResponsePolicyDecision&) = delete;
  WebResponsePolicyDecision& operator=(WebResponsePolicyDecision&&)      = delete;

  /**
   * @brief Returns the response URL.
   * @SINCE_3_0.3
   * @return The response URL, or an empty string if unavailable
   */
  Dali::String GetUrl() const;

  /**
   * @brief Returns the HTTP status code, or -1 if unavailable.
   * @SINCE_3_0.3
   * @return The HTTP response status code, or -1 if unavailable
   */
  int32_t GetStatusCode() const;

  /**
   * @brief Requests use of the response and returns the backend result.
   * @SINCE_3_0.3
   * @return True if the backend accepts the decision request, false otherwise
   */
  bool Use();

  /**
   * @brief Requests rejection of the response and returns the backend result.
   * @SINCE_3_0.3
   * @return True if the backend accepts the decision request, false otherwise
   */
  bool Ignore();

private:
  friend class Integration::WebViewImpl;
  struct Impl;
  DALI_INTERNAL WebResponsePolicyDecision(Dali::String url, int32_t statusCode, Dali::Callback<bool()> use, Dali::Callback<bool()> ignore);

  Dali::UniquePtr<Impl> mImpl;
};

} // namespace Ui
} // namespace DALI_NAMESPACE
