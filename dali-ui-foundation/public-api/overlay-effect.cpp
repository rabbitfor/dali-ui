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

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/interaction-effects/overlay-effect-impl.h>

#define DALI_ASSERT_VALID_OVERLAY_EFFECT_CONFIG(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from OverlayEffect::Config")

namespace Dali
{
namespace Ui
{
namespace OverlayEffect
{
namespace
{

StateEffect CreateDefaultOverlayEffect()
{
  return Config::Default().Build();
}

StateEffect CreateRoundOverlayEffect()
{
  return Config::Round().Build();
}

} // namespace

class Config::Impl
{
public:
  Impl()
  : mOverlayColor(Internal::OverlayEffectObject::DEFAULT_OVERLAY_COLOR_RGB, Internal::OverlayEffectObject::DEFAULT_OVERLAY_COLOR_ALPHA),
    mCornerRadius(Vector4::ZERO),
    mCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE),
    mUseTargetCornerRadius(true)
  {
  }

  Impl(const Impl& rhs) = default;

  UiColor            mOverlayColor;
  Vector4            mCornerRadius;
  CornerRadiusPolicy mCornerRadiusPolicy;
  bool               mUseTargetCornerRadius;
};

StateEffect Default()
{
  static const StateEffect effect = CreateDefaultOverlayEffect();
  return effect;
}

StateEffect Round()
{
  static const StateEffect effect = CreateRoundOverlayEffect();
  return effect;
}

Config::Config()
: mImpl(new Impl())
{
}

Config::Config(const Config& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_OVERLAY_EFFECT_CONFIG(rhs.mImpl);
  mImpl = new Impl(*rhs.mImpl);
}

Config::Config(Config&& rhs) noexcept
: mImpl(rhs.mImpl)
{
  rhs.mImpl = nullptr;
}

Config& Config::operator=(const Config& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_OVERLAY_EFFECT_CONFIG(rhs.mImpl);
    Impl* newImpl = new Impl(*rhs.mImpl);
    delete mImpl;
    mImpl = newImpl;
  }
  return *this;
}

Config& Config::operator=(Config&& rhs) noexcept
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl     = rhs.mImpl;
    rhs.mImpl = nullptr;
  }
  return *this;
}

Config::~Config()
{
  delete mImpl;
}

Config Config::Default()
{
  return Config();
}

Config Config::Round()
{
  return Config()
    .SetCornerRadius(Vector4(0.5f, 0.5f, 0.5f, 0.5f))
    .SetCornerRadiusPolicyRelative();
}

Config& Config::SetOverlayColor(const UiColor& color)
{
  DALI_ASSERT_VALID_OVERLAY_EFFECT_CONFIG(mImpl);
  mImpl->mOverlayColor = color;
  return *this;
}

UiColor Config::GetOverlayColor() const
{
  DALI_ASSERT_VALID_OVERLAY_EFFECT_CONFIG(mImpl);
  return mImpl->mOverlayColor;
}

Config& Config::SetCornerRadius(float radius)
{
  return SetCornerRadius(Vector4(radius, radius, radius, radius));
}

Config& Config::SetCornerRadius(float topLeft, float topRight, float bottomRight, float bottomLeft)
{
  return SetCornerRadius(Vector4(topLeft, topRight, bottomRight, bottomLeft));
}

Config& Config::SetCornerRadius(const Vector4& radius)
{
  DALI_ASSERT_VALID_OVERLAY_EFFECT_CONFIG(mImpl);
  mImpl->mCornerRadius          = radius;
  mImpl->mUseTargetCornerRadius = false;
  return *this;
}

Vector4 Config::GetCornerRadius() const
{
  DALI_ASSERT_VALID_OVERLAY_EFFECT_CONFIG(mImpl);
  return mImpl->mCornerRadius;
}

Config& Config::SetCornerRadiusPolicy(CornerRadiusPolicy policy)
{
  DALI_ASSERT_VALID_OVERLAY_EFFECT_CONFIG(mImpl);
  mImpl->mCornerRadiusPolicy    = policy;
  mImpl->mUseTargetCornerRadius = false;
  return *this;
}

Config& Config::SetCornerRadiusPolicyRelative()
{
  return SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);
}

CornerRadiusPolicy Config::GetCornerRadiusPolicy() const
{
  DALI_ASSERT_VALID_OVERLAY_EFFECT_CONFIG(mImpl);
  return mImpl->mCornerRadiusPolicy;
}

StateEffect Config::Build() const
{
  DALI_ASSERT_VALID_OVERLAY_EFFECT_CONFIG(mImpl);
  return StateEffect(new Internal::OverlayEffectObject(mImpl->mOverlayColor, mImpl->mCornerRadius, mImpl->mCornerRadiusPolicy, mImpl->mUseTargetCornerRadius));
}

} // namespace OverlayEffect
} // namespace Ui
} // namespace Dali

#undef DALI_ASSERT_VALID_OVERLAY_EFFECT_CONFIG
