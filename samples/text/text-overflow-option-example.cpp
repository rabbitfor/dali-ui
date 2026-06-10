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

#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float STACK_SPACING = 12.0f;
constexpr float STACK_PADDING = 24.0f;
constexpr float LABEL_WIDTH   = 360.0f;
constexpr float LABEL_HEIGHT  = 64.0f;
constexpr float LABEL_PADDING = 12.0f;
constexpr float TITLE_SIZE    = 22.0f;
constexpr float CAPTION_SIZE  = 14.0f;
constexpr float LABEL_SIZE    = 26.0f;

const char* const LONG_TEXT = "TV overflow options keep long single-line labels readable in a constrained layout.";

Label CreateCaption(const char* text)
{
  Label label = Label::New(text);
  label.SetFontSize(CAPTION_SIZE);
  label.SetTextColor(UiColor(0x333333));
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(WRAP_CONTENT);
  return label;
}

Label CreateOptionLabel(int optionNum)
{
  Label label = Label::New(LONG_TEXT);
  label.SetFontSize(LABEL_SIZE);
  label.SetTextColor(UiColor(0xFFFFFF));
  label.SetBackgroundColor(UiColor(0x202632));
  label.SetRequestedWidth(LABEL_WIDTH);
  label.SetRequestedHeight(LABEL_HEIGHT);
  label.SetPadding(Extents(LABEL_PADDING, LABEL_PADDING, 0.0f, 0.0f));
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  label.SetMultiLine(false);
  label.SetAsyncRendering(true);
  label.SetFocusable(true);
  label.SetTouchFocusable(true);
  label.With(TVLabelExt::OverflowOption, optionNum);
  return label;
}

} // namespace

class TextOverflowOptionController : public ConnectionTracker
{
public:
  explicit TextOverflowOptionController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextOverflowOptionController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xFFFFFF));

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetSpacing(STACK_SPACING);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING));

    Label title = Label::New("Overflow Option Extension");
    title.SetFontSize(TITLE_SIZE);
    title.SetTextColor(UiColor(0x111111));
    title.SetRequestedWidth(MATCH_PARENT);
    title.SetRequestedHeight(WRAP_CONTENT);

    root.AddChildren({
      title,
      CreateCaption("label.With(TVLabelExt::OverflowOption, 1) - ellipsis"),
      CreateOptionLabel(1),
      CreateCaption("label.With(TVLabelExt::OverflowOption, 2) - text fit, then ellipsis"),
      CreateOptionLabel(2),
      CreateCaption("label.With(TVLabelExt::OverflowOption, 3) - wrap-content width"),
      CreateOptionLabel(3),
      CreateCaption("label.With(TVLabelExt::OverflowOption, 4) - marquee while focused"),
      CreateOptionLabel(4),
      CreateCaption("label.With(TVLabelExt::OverflowOption, 5) - auto marquee on overflow"),
      CreateOptionLabel(5),
    });

    window.Add(root);
    window.KeyEventSignal().Connect(this, &TextOverflowOptionController::OnKeyEvent);
  }

  void OnKeyEvent(Window window, KeyEvent event)
  {
    (void)window;
    if(event.GetState() == KeyEvent::UP && (event.GetKeyName() == "Escape" || event.GetKeyName() == "Back"))
    {
      mApplication.Quit();
    }
  }

private:
  Application& mApplication;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application                  application = Application::New(&argc, &argv);
  TextOverflowOptionController controller(application);
  application.MainLoop();
  return 0;
}
