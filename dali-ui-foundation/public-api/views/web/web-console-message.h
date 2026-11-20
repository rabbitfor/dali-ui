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
#include <dali-ui-foundation/public-api/views/web/web-view-types.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/unique-ptr.h>
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
 * @brief An immutable snapshot of a web console message.
 * Copy the signal argument to keep it after the signal handler returns.
 * @SINCE_3_0.2
 */
class DALI_UI_API WebConsoleMessage
{
public:
  /// @brief Creates an empty message. @SINCE_3_0.2
  WebConsoleMessage();

  /// @brief Destroys the message. @SINCE_3_0.2
  ~WebConsoleMessage();

  /// @brief Copies the message data. @SINCE_3_0.2
  WebConsoleMessage(const WebConsoleMessage& message);

  /// @brief Moves the message data, leaving the source empty. @SINCE_3_0.2
  WebConsoleMessage(WebConsoleMessage&& message) noexcept;

  /// @brief Replaces the message with an independent copy. @SINCE_3_0.2
  WebConsoleMessage& operator=(const WebConsoleMessage& message);

  /// @brief Moves the message data, leaving the source empty. @SINCE_3_0.2
  WebConsoleMessage& operator=(WebConsoleMessage&& message) noexcept;

  /// @brief Returns the message text. @SINCE_3_0.2
  Dali::String GetText() const;

  /// @brief Returns the source identifier supplied by the engine. @SINCE_3_0.2
  Dali::String GetSource() const;

  /// @brief Returns the source line number, or 0 if unavailable. @SINCE_3_0.2
  uint32_t GetLine() const;

  /// @brief Returns the message level, or EMPTY if unavailable. @SINCE_3_0.2
  WebConsoleMessageLevel GetLevel() const;

private:
  friend class Integration::WebViewImpl;
  struct Impl;
  DALI_INTERNAL WebConsoleMessage(Dali::String text, Dali::String source, uint32_t line, WebConsoleMessageLevel level);

  Dali::UniquePtr<Impl> mImpl;
};

} // namespace Ui
} // namespace DALI_NAMESPACE
