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

#include <dali-ui-foundation/public-api/views/web/web-console-message.h>
#include <utility>

namespace DALI_NAMESPACE
{
namespace Ui
{

struct WebConsoleMessage::Impl
{
  Dali::String           mText;
  Dali::String           mSource;
  uint32_t               mLine;
  WebConsoleMessageLevel mLevel;
};

WebConsoleMessage::WebConsoleMessage()  = default;
WebConsoleMessage::~WebConsoleMessage() = default;

WebConsoleMessage::WebConsoleMessage(const WebConsoleMessage& message)
: mImpl(message.mImpl ? new Impl(*message.mImpl) : nullptr)
{
}

WebConsoleMessage::WebConsoleMessage(WebConsoleMessage&& message) noexcept = default;

WebConsoleMessage& WebConsoleMessage::operator=(const WebConsoleMessage& message)
{
  if(this != &message)
  {
    mImpl.Reset(message.mImpl ? new Impl(*message.mImpl) : nullptr);
  }
  return *this;
}

WebConsoleMessage& WebConsoleMessage::operator=(WebConsoleMessage&& message) noexcept = default;

Dali::String WebConsoleMessage::GetText() const
{
  return mImpl ? mImpl->mText : Dali::String();
}

Dali::String WebConsoleMessage::GetSource() const
{
  return mImpl ? mImpl->mSource : Dali::String();
}

uint32_t WebConsoleMessage::GetLine() const
{
  return mImpl ? mImpl->mLine : 0;
}

WebConsoleMessageLevel WebConsoleMessage::GetLevel() const
{
  return mImpl ? mImpl->mLevel : WebConsoleMessageLevel::EMPTY;
}

WebConsoleMessage::WebConsoleMessage(Dali::String text, Dali::String source, uint32_t line, WebConsoleMessageLevel level)
: mImpl(new Impl{std::move(text), std::move(source), line, level})
{
}

} // namespace Ui
} // namespace DALI_NAMESPACE
