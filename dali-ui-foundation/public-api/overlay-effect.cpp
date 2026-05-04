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

// CLASS HEADER
#include <dali-ui-foundation/public-api/overlay-effect.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/interaction-effects/overlay-effect-impl.h>

namespace Dali
{
namespace Ui
{

namespace
{

OverlayEffect CreateDefaultOverlayEffect()
{
  return OverlayEffect::Config::New().Build();
}

OverlayEffect CreateRoundOverlayEffect()
{
  return OverlayEffect::Config::New()
    .SetCornerRadius(Vector4(0.5f, 0.5f, 0.5f, 0.5f))
    .SetCornerRadiusPolicyRelative()
    .Build();
}

} // namespace

const OverlayEffect& OverlayEffect::Default()
{
  static const OverlayEffect effect = CreateDefaultOverlayEffect();
  return effect;
}

const OverlayEffect& OverlayEffect::Round()
{
  static const OverlayEffect effect = CreateRoundOverlayEffect();
  return effect;
}

OverlayEffect OverlayEffect::DownCast(BaseHandle handle)
{
  return OverlayEffect(dynamic_cast<Internal::OverlayEffectImpl*>(handle.GetObjectPtr()));
}

UiColor OverlayEffect::GetOverlayColor() const
{
  return GetImpl(*this).GetOverlayColor();
}

Vector4 OverlayEffect::GetCornerRadius() const
{
  return GetImpl(*this).GetCornerRadius();
}

CornerRadiusPolicy OverlayEffect::GetCornerRadiusPolicy() const
{
  return GetImpl(*this).GetCornerRadiusPolicy();
}

OverlayEffect::Config OverlayEffect::Configure() const
{
  IntrusivePtr<Internal::OverlayEffectImpl> impl(new Internal::OverlayEffectImpl(GetImpl(*this)));
  return Config(impl.Get());
}

OverlayEffect::OverlayEffect(Internal::OverlayEffectImpl* impl)
: StateEffect(impl)
{
}

OverlayEffect::Config OverlayEffect::Config::New()
{
  IntrusivePtr<Internal::OverlayEffectImpl> impl(new Internal::OverlayEffectImpl());
  return Config(impl.Get());
}

OverlayEffect::Config::Config(Config&& rhs) noexcept = default;

OverlayEffect::Config& OverlayEffect::Config::operator=(Config&& rhs) noexcept = default;

OverlayEffect::Config::~Config() = default;

OverlayEffect::Config& OverlayEffect::Config::SetOverlayColor(const UiColor& color)
{
  mImpl->SetOverlayColor(color);
  return *this;
}

UiColor OverlayEffect::Config::GetOverlayColor() const
{
  return mImpl->GetOverlayColor();
}

OverlayEffect::Config& OverlayEffect::Config::SetCornerRadius(float radius)
{
  return SetCornerRadius(Vector4(radius, radius, radius, radius));
}

OverlayEffect::Config& OverlayEffect::Config::SetCornerRadius(float topLeft, float topRight, float bottomRight, float bottomLeft)
{
  return SetCornerRadius(Vector4(topLeft, topRight, bottomRight, bottomLeft));
}

OverlayEffect::Config& OverlayEffect::Config::SetCornerRadius(const Vector4& radius)
{
  mImpl->SetCornerRadius(radius);
  mImpl->SetUseTargetCornerRadius(false);
  return *this;
}

Vector4 OverlayEffect::Config::GetCornerRadius() const
{
  return mImpl->GetCornerRadius();
}

OverlayEffect::Config& OverlayEffect::Config::SetCornerRadiusPolicy(CornerRadiusPolicy policy)
{
  mImpl->SetCornerRadiusPolicy(policy);
  mImpl->SetUseTargetCornerRadius(false);
  return *this;
}

OverlayEffect::Config& OverlayEffect::Config::SetCornerRadiusPolicyRelative()
{
  return SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);
}

CornerRadiusPolicy OverlayEffect::Config::GetCornerRadiusPolicy() const
{
  return mImpl->GetCornerRadiusPolicy();
}

OverlayEffect OverlayEffect::Config::Build()
{
  OverlayEffect effect(mImpl.Get());
  mImpl.Reset();
  return effect;
}

OverlayEffect::Config::Config(Internal::OverlayEffectImpl* impl)
: mImpl(impl)
{
}

} // namespace Ui
} // namespace Dali
