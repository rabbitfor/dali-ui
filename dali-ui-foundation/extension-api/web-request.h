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
 * @brief A retainable intercepted request with shared synchronized response capability.
 * Copies share one request. Metadata remains readable after cancellation.
 * Releasing the last copy during interception passes the request through.
 * Later abandonment submits an empty 503 response, or finishes an active stream.
 * No response operations access native state after invalidation.
 * @SINCE_3_0.3
 */
class DALI_UI_API WebRequest
{
public:
  /// @brief Creates an empty value. @SINCE_3_0.3
  WebRequest();
  /// @brief Destroys the value. @SINCE_3_0.3
  ~WebRequest();
  /**
   * @brief Copies the value.
   * @SINCE_3_0.3
   * @param[in] other Source value
   */
  WebRequest(const WebRequest& other);
  /**
   * @brief Moves the value.
   * @SINCE_3_0.3
   * @param[in] other Source value
   */
  WebRequest(WebRequest&& other) noexcept;
  /**
   * @brief Copies the value.
   * @SINCE_3_0.3
   * @param[in] other Source value
   * @return This value
   */
  WebRequest& operator=(const WebRequest& other);
  /**
   * @brief Moves the value.
   * @SINCE_3_0.3
   * @param[in] other Source value
   * @return This value
   */
  WebRequest& operator=(WebRequest&& other) noexcept;

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

  /**
   * @brief Passes the request to the engine's normal loader.
   * @SINCE_3_0.3
   * @return The backend result, or false outside the interception callback
   */
  bool Ignore();

  /**
   * @brief Sets the status before response submission or streaming.
   * @SINCE_3_0.3
   * @param[in] code HTTP status code (100 to 599)
   * @param[in] text HTTP reason phrase
   * @return True if accepted
   */
  bool SetResponseStatus(int32_t code, const Dali::String& text);

  /**
   * @brief Adds a response header while preparing.
   * @SINCE_3_0.3
   * @param[in] name Header name
   * @param[in] value Header value
   * @return True if accepted
   */
  bool AddResponseHeader(const Dali::String& name, const Dali::String& value);

  /**
   * @brief Validates all headers before forwarding individual entries.
   * @SINCE_3_0.3
   * @param[in] headers String-name/string-value map
   * @return True if every header was accepted; no rollback is promised on native failure
   */
  bool AddResponseHeaders(const Dali::Property::Map& headers);

  /**
   * @brief Submits a complete response and consumes response capability.
   * Worker calls are allowed. The engine copies the buffer before returning.
   * @SINCE_3_0.3
   * @param[in] headers Serialized HTTP headers with CRLF lines and final empty line
   * @param[in] body Binary body, or null for an empty body
   * @param[in] length Body length in bytes, within uint32_t range
   * @return Backend acceptance, not delivery acknowledgement; false does not permit retry
   */
  bool SetResponse(const Dali::String& headers, const uint8_t* body, size_t length);

  /**
   * @brief Submits the complete body after setting status and headers.
   * Worker calls are allowed. Submission consumes response capability.
   * @SINCE_3_0.3
   * @param[in] body Binary body, or null for an empty body
   * @param[in] length Body length in bytes, within uint32_t range
   * @return Backend acceptance, not delivery acknowledgement
   */
  bool SetResponseBody(const uint8_t* body, size_t length);

  /**
   * @brief Writes a chunk after the interception callback has returned.
   * The first chunk freezes status and headers. Worker calls are allowed.
   * The engine copies input before returning.
   * @SINCE_3_0.3
   * @param[in] body Chunk data; pass null with length 0 to finish
   * @param[in] length Chunk length in bytes, within uint32_t range
   * @return True if another write is possible while live. Native false ends writing,
   * including normal explicit finish. Invalid arguments return false without
   * changing the request. True does not confirm delivery.
   */
  bool WriteResponseChunk(const uint8_t* body, size_t length);

private:
  friend class Integration::WebProfileImpl;
  struct Impl;
  explicit DALI_INTERNAL WebRequest(Impl* implementation);
  Dali::UniquePtr<Impl>  mImpl;
};
} // namespace Extension
} // namespace Ui
} // namespace DALI_NAMESPACE
