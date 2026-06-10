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

#include "tv-label-ext.h"

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr int MARQUEE_GAP   = 80;
constexpr int MARQUEE_SPEED = 120;

struct OptionData
{
  explicit OptionData(int optionNum)
  : optionNum(optionNum)
  {
  }

  int optionNum;
};

AttachmentId GetOptionDataId();
OptionData* GetOptionData(Label& label);
void SetOptionData(Label& label, int optionNum);
void ResetOption(Label& label);
void ApplyEllipsis(Label& label);
void ApplyTextFit(Label& label, float desiredFontSize, float minimumFontSize, float stepSize);
void ApplyMarquee(Label& label);
void OnFocusChanged(View view, bool focused);

} // namespace

void TVLabelExt::OverflowOption(Label& label, int optionNum)
{
  ResetOption(label);

  switch(optionNum)
  {
    case 1:
    {
      ApplyEllipsis(label);
      break;
    }
    case 2:
    {
      ApplyTextFit(label, label.GetFontSize(), 16.0f, 4.0f);
      break;
    }
    case 3:
    {
      ApplyEllipsis(label);
      label.SetRequestedWidth(WRAP_CONTENT);
      break;
    }
    case 4:
    {
      ApplyEllipsis(label);
      label.SetFocusable(true);
      label.SetTouchFocusable(true);
      label.FocusChangedSignal().Connect(&OnFocusChanged);
      break;
    }
    case 5:
    {
      ApplyMarquee(label);
      break;
    }
    default:
    {
      ApplyEllipsis(label);
      break;
    }
  }

  SetOptionData(label, optionNum);
}

namespace
{

AttachmentId GetOptionDataId()
{
  static AttachmentId id = AttachmentId::Alloc();
  return id;
}

OptionData* GetOptionData(Label& label)
{
  return label.GetAttachment<OptionData>(GetOptionDataId());
}

void SetOptionData(Label& label, int optionNum)
{
  OptionData* optionData = GetOptionData(label);
  if(optionData)
  {
    optionData->optionNum = optionNum;
    return;
  }

  label.SetAttachment(GetOptionDataId(), Dali::MakeUnique<OptionData>(optionNum));
}

void ResetOption(Label& label)
{
  OptionData* optionData = GetOptionData(label);
  if(!optionData)
  {
    return;
  }

  switch(optionData->optionNum)
  {
    case 2:
    {
      label.ClearTextFit();
      break;
    }
    case 4:
    {
      label.FocusChangedSignal().Disconnect(&OnFocusChanged);
      label.StopMarquee();
      label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::MANUAL);
      break;
    }
    case 5:
    {
      label.StopMarquee();
      label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::MANUAL);
      break;
    }
    default:
    {
      break;
    }
  }
}

void ApplyEllipsis(Label& label)
{
  label.SetOverflowMode(Text::OverflowMode::ELLIPSIS);
}

void ApplyTextFit(Label& label, float desiredFontSize, float minimumFontSize, float stepSize)
{
  ApplyEllipsis(label);
  label.SetFontSize(desiredFontSize);
  label.SetTextFit(Text::FitRange(minimumFontSize, desiredFontSize, stepSize));
}

void ApplyMarquee(Label& label)
{
  label.SetOverflowMode(Text::OverflowMode::ELLIPSIS);
  label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::ON_OVERFLOW);
  label.SetMarqueeOrientation(Text::MarqueeOrientation::HORIZONTAL);
  label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
  label.SetMarqueeSpeed(MARQUEE_SPEED);
  label.SetMarqueeGap(MARQUEE_GAP);
}

void OnFocusChanged(View view, bool focused)
{
  Label label = Label::DownCast(view);
  if(!label)
  {
    return;
  }

  if(focused)
  {
    ApplyMarquee(label);
  }
  else
  {
    label.StopMarquee();
    label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::MANUAL);
    ApplyEllipsis(label);
  }
}

} // namespace
