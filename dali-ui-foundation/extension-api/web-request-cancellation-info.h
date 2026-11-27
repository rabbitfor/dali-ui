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
#include <dali/public-api/object/property-map.h>
#include <cstddef>
#include <cstdint>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration
{
class WebProfileImpl;
}
namespace Extension
{
/**
 * @brief Immutable cancellation metadata, copied during interception.
 * Provides no response-writing capability.
 * @SINCE_3_0.3
 */
class DALI_UI_API WebRequestCancellationInfo
{
public:
  /// @brief Creates an empty value. @SINCE_3_0.3
  WebRequestCancellationInfo();
  /// @brief Destroys the value. @SINCE_3_0.3
  ~WebRequestCancellationInfo();
  /**
   * @brief Copies the value.
   * @SINCE_3_0.3
   * @param[in] other Source value
   */
  WebRequestCancellationInfo(const WebRequestCancellationInfo& other);
  /**
   * @brief Moves the value.
   * @SINCE_3_0.3
   * @param[in] other Source value
   */
  WebRequestCancellationInfo(WebRequestCancellationInfo&& other) noexcept;
  /**
   * @brief Copies the value.
   * @SINCE_3_0.3
   * @param[in] other Source value
   * @return This value
   */
  WebRequestCancellationInfo& operator=(const WebRequestCancellationInfo& other);
  /**
   * @brief Moves the value.
   * @SINCE_3_0.3
   * @param[in] other Source value
   * @return This value
   */
  WebRequestCancellationInfo& operator=(WebRequestCancellationInfo&& other) noexcept;

  /**
   * @brief Request identity independent of URL.
   * @SINCE_3_0.3
   * @return The saved request metadata
   */
  uint64_t GetRequestId() const;

  /**
   * @brief Cached originating Actor ID, or 0 if unavailable.
   * @SINCE_3_0.3
   * @return The saved request metadata
   */
  uint32_t GetWebViewId() const;

  /**
   * @brief Request URL snapshot.
   * @SINCE_3_0.3
   * @return The saved request metadata
   */
  Dali::String GetUrl() const;

  /**
   * @brief HTTP method snapshot.
   * @SINCE_3_0.3
   * @return The saved request metadata
   */
  Dali::String GetMethod() const;

  /**
   * @brief A copy of the request headers.
   * @SINCE_3_0.3
   * @return The saved request metadata
   */
  Dali::Property::Map GetHeaders() const;

private:
  friend class Integration::WebProfileImpl;
  struct Impl;
  explicit DALI_INTERNAL WebRequestCancellationInfo(Impl* implementation);
  Dali::UniquePtr<Impl>  mImpl;
};
} // namespace Extension
} // namespace Ui
} // namespace DALI_NAMESPACE
