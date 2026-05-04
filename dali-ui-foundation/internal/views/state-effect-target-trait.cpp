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
#include <dali-ui-foundation/internal/views/state-effect-target-trait.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

StateEffectTargetTrait StateEffectTargetTrait::New()
{
  IntrusivePtr<StateEffectTargetTraitImpl> impl(new StateEffectTargetTraitImpl());
  return StateEffectTargetTrait(impl.Get());
}

StateEffectTargetTrait StateEffectTargetTrait::DownCast(BaseHandle handle)
{
  return StateEffectTargetTrait(dynamic_cast<StateEffectTargetTraitImpl*>(handle.GetObjectPtr()));
}

StateEffectTargetTraitImpl& StateEffectTargetTrait::GetImpl()
{
  return static_cast<StateEffectTargetTraitImpl&>(GetBaseObject());
}

const StateEffectTargetTraitImpl& StateEffectTargetTrait::GetImpl() const
{
  return static_cast<const StateEffectTargetTraitImpl&>(GetBaseObject());
}

StateEffectTargetTrait::StateEffectTargetTrait(StateEffectTargetTraitImpl* impl)
: BaseHandle(impl)
{
}

StateEffectTargetTraitImpl::StateEffectTargetTraitImpl()
: TraitObject(),
  mPrimaryTargetId(INVALID_TARGET_ID),
  mSecondaryTargetId(INVALID_TARGET_ID)
{
}

StateEffectTargetTraitImpl::~StateEffectTargetTraitImpl() = default;

void StateEffectTargetTraitImpl::SetPrimaryTargetId(int32_t targetId)
{
  mPrimaryTargetId = targetId;
}

int32_t StateEffectTargetTraitImpl::GetPrimaryTargetId() const
{
  return mPrimaryTargetId;
}

void StateEffectTargetTraitImpl::SetSecondaryTargetId(int32_t targetId)
{
  mSecondaryTargetId = targetId;
}

int32_t StateEffectTargetTraitImpl::GetSecondaryTargetId() const
{
  return mSecondaryTargetId;
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
