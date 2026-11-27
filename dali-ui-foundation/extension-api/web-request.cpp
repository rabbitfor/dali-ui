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

#include <dali-ui-foundation/internal/web-request-data.h>
#include <utility>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Extension
{
WebRequest::WebRequest()  = default;
WebRequest::~WebRequest() = default;
WebRequest::WebRequest(const WebRequest& other)
: mImpl(other.mImpl ? new Impl(*other.mImpl) : nullptr)
{
}
WebRequest::WebRequest(WebRequest&& other) noexcept = default;

WebRequest& WebRequest::operator=(const WebRequest& other)
{
  if(this != &other) mImpl.Reset(other.mImpl ? new Impl(*other.mImpl) : nullptr);
  return *this;
}
WebRequest& WebRequest::operator=(WebRequest&& other) noexcept = default;

WebRequest::WebRequest(Impl* implementation)
: mImpl(implementation)
{
}

uint64_t WebRequest::GetRequestId() const
{
  return mImpl && mImpl->request ? mImpl->request->GetRequestId() : 0;
}

uint32_t WebRequest::GetWebViewId() const
{
  return mImpl && mImpl->request ? mImpl->request->GetWebViewId() : 0;
}

Dali::String WebRequest::GetUrl() const
{
  return mImpl && mImpl->request ? Dali::String(mImpl->request->GetUrl().c_str()) : Dali::String();
}

Dali::String WebRequest::GetMethod() const
{
  return mImpl && mImpl->request ? Dali::String(mImpl->request->GetMethod().c_str()) : Dali::String();
}

Dali::Property::Map WebRequest::GetHeaders() const
{
  return mImpl && mImpl->request ? mImpl->request->GetHeaders() : Dali::Property::Map();
}

bool WebRequest::Ignore()
{
  return mImpl && mImpl->request && mImpl->request->Ignore();
}

bool WebRequest::SetResponseStatus(int32_t code, const Dali::String& text)
{
  return mImpl && mImpl->request && mImpl->request->SetResponseStatus(code, text.CStr());
}

bool WebRequest::AddResponseHeader(const Dali::String& name, const Dali::String& value)
{
  return mImpl && mImpl->request && mImpl->request->AddResponseHeader(name.CStr(), value.CStr());
}

bool WebRequest::AddResponseHeaders(const Dali::Property::Map& headers)
{
  return mImpl && mImpl->request && mImpl->request->AddResponseHeaders(headers);
}

bool WebRequest::SetResponse(const Dali::String& headers, const uint8_t* body, size_t length)
{
  return mImpl && mImpl->request && mImpl->request->SetResponse(headers.CStr(), body, length);
}

bool WebRequest::SetResponseBody(const uint8_t* body, size_t length)
{
  return mImpl && mImpl->request && mImpl->request->SetResponseBody(body, length);
}

bool WebRequest::WriteResponseChunk(const uint8_t* body, size_t length)
{
  return mImpl && mImpl->request && mImpl->request->WriteResponseChunk(body, length);
}
} // namespace Extension
} // namespace Ui
} // namespace DALI_NAMESPACE
