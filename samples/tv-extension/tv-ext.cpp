/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
 */

#include "tv-ext.h"

#include <dali/public-api/common/unique-ptr.h>

namespace TVExt
{

namespace
{

const Dali::Ui::AttachmentId FLUX_VIEW_DATA_ID = Dali::Ui::AttachmentId::Alloc();

FluxViewData* GetFluxViewData(Dali::Ui::View view)
{
  return view ? view.GetAttachment<FluxViewData>(FLUX_VIEW_DATA_ID) : nullptr;
}

FluxLabelData* GetFluxLabelData(Dali::Ui::Label label)
{
  return dynamic_cast<FluxLabelData*>(GetFluxViewData(label));
}

FluxImageViewData* GetFluxImageViewData(Dali::Ui::ImageView imageView)
{
  return dynamic_cast<FluxImageViewData*>(GetFluxViewData(imageView));
}

} // unnamed namespace

Dali::Ui::AttachmentId GetFluxViewDataAttachmentId()
{
  return FLUX_VIEW_DATA_ID;
}

void AttachFluxViewData(Dali::Ui::View view)
{
  if(!view || GetFluxViewData(view))
  {
    return;
  }

  Dali::Ui::ImageView imageView = Dali::Ui::ImageView::DownCast(view);
  if(imageView)
  {
    Dali::UniquePtr<FluxViewData> data = Dali::MakeUnique<FluxImageViewData>(imageView);
    view.SetAttachment(FLUX_VIEW_DATA_ID, Dali::Move(data));
    return;
  }

  Dali::Ui::Label label = Dali::Ui::Label::DownCast(view);
  if(label)
  {
    Dali::UniquePtr<FluxViewData> data = Dali::MakeUnique<FluxLabelData>(label);
    view.SetAttachment(FLUX_VIEW_DATA_ID, Dali::Move(data));
    return;
  }

  view.SetAttachment(FLUX_VIEW_DATA_ID, Dali::MakeUnique<FluxViewData>(view));
}

void SetFluxPolicy(Dali::Ui::View view, FluxPolicy policy)
{
  FluxViewData* data = GetFluxViewData(view);
  if(data)
  {
    data->SetFluxPolicy(policy);
  }
}

FluxPolicy GetFluxPolicy(Dali::Ui::View view)
{
  FluxViewData* data = GetFluxViewData(view);
  return data ? data->GetFluxPolicy() : FluxPolicy::NONE;
}

FluxViewData::FluxPolicyAppliedSignalType& FluxPolicyAppliedSignal(Dali::Ui::View view)
{
  FluxViewData* data = GetFluxViewData(view);
  DALI_ASSERT_ALWAYS(data && "FluxPolicyAppliedSignal requires FluxViewData attachment");
  return data->FluxPolicyAppliedSignal();
}

void SetOverflowOption(Dali::Ui::Label label, int option)
{
  FluxLabelData* data = GetFluxLabelData(label);
  if(data)
  {
    data->SetOverflowOption(option);
  }
}

int GetOverflowOption(Dali::Ui::Label label)
{
  FluxLabelData* data = GetFluxLabelData(label);
  return data ? data->GetOverflowOption() : 0;
}

void SetAsyncImageResize(Dali::Ui::ImageView imageView, bool asyncImageResize)
{
  FluxImageViewData* data = GetFluxImageViewData(imageView);
  if(data)
  {
    data->SetAsyncImageResize(asyncImageResize);
  }
}

bool GetAsyncImageResize(Dali::Ui::ImageView imageView)
{
  FluxImageViewData* data = GetFluxImageViewData(imageView);
  return data ? data->IsImageFittingResize() : false;
}

FluxImageViewData::AsyncImageResizeFinishedSignalType& AsyncImageResizeFinishedSignal(Dali::Ui::ImageView imageView)
{
  FluxImageViewData* data = GetFluxImageViewData(imageView);
  DALI_ASSERT_ALWAYS(data && "AsyncImageResizeFinishedSignal requires FluxImageViewData attachment");
  return data->AsyncImageResizeFinishedSignal();
}

} // namespace TVExt
