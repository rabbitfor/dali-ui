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

#include "manual-test-case.h"

#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float CARD_W        = 128.0f;
constexpr float CARD_H        = 128.0f;
constexpr float GAP           = 18.0f;
constexpr float PAGE_PADDING  = 24.0f;
constexpr float TEXT_INSET_H  = 24.0f;
constexpr float TEXT_INSET_V  = 18.0f;
constexpr float TARGET_INSET  = 18.0f;
constexpr float LABEL_FONT    = 20.0f;
constexpr float SUBLABEL_FONT = 18.0f;

struct CardOptions
{
  bool touchFocusable{true};
};
} // namespace

/**
 * @brief Manual check for pressed/focused OverlayEffect and primary-target routing.
 */
class TcOverlayEffect : public ManualTest::TestCase
{
public:
  Dali::String GetName() const override
  {
    return "OverlayEffect";
  }

  Dali::String GetDescription() const override
  {
    return "Compare touch-focus ON/OFF cards and primary-target overlay behavior";
  }

  void OnEnter(View contentArea) override
  {
    contentArea.Add(StackLayout::New()
      .SetSpacing(GAP)
      .SetPadding(Extents(PAGE_PADDING, PAGE_PADDING, PAGE_PADDING, PAGE_PADDING))
      .SetRequestedWidth(MATCH_PARENT)
      .Children({
        CreateCard("TOUCH FOCUS ON", UiColor(0x2B6CB0), OverlayEffect::Config::Default().SetOverlayColor(UiColor(0x000000, 0.4f)).Build()),
        CreateCard("TOUCH FOCUS OFF", UiColor(0x2F855A), OverlayEffect::Config::Default().SetOverlayColor(UiColor(0x000000, 0.4f)).Build(), {false}),
        CreatePrimaryTargetCard(),
        CreateCard("ROUND", UiColor(0x805AD5), OverlayEffect::Round()),
      }));
  }

private:
  View CreateTextContent(const Dali::String& text, float fontSize = LABEL_FONT)
  {
    View content = View::New();
    content.SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetLayoutMode(LayoutMode::STANDALONE)
      .SetPadding(Extents(TEXT_INSET_H, TEXT_INSET_H, TEXT_INSET_V, TEXT_INSET_V))
      .Children({
        Label::New(text)
          .SetFontSize(fontSize)
          .SetTextColor(UiColor(0xFFFFFF))
          .SetMultiLine(true)
          .SetRequestedWidth(MATCH_PARENT)
          .SetRequestedHeight(MATCH_PARENT)
      });

    return content;
  }

  InteractiveView CreateCard(const Dali::String& text, const UiColor& color, StateEffect effect, CardOptions options = {})
  {
    return InteractiveView::New()
      .SetBackgroundColor(color)
      .SetRequestedWidth(CARD_W)
      .SetRequestedHeight(CARD_H)
      .SetTouchFocusable(options.touchFocusable)
      .SetStateEffect(effect)
      .Children({
        CreateTextContent(text)
      });
  }

  InteractiveView CreatePrimaryTargetCard()
  {
    Label target;
    return InteractiveView::New()
      .SetRequestedWidth(CARD_W)
      .SetRequestedHeight(CARD_H)
      .SetBackgroundColor(UiColor(0xAA5568))
      .SetTouchFocusable(false)
      .SetStateEffect(OverlayEffect::Config::Default().SetOverlayColor(UiColor(0x000000, 0.4f)).Build())
      .SetPadding(Extents(TARGET_INSET, TARGET_INSET, TARGET_INSET, TARGET_INSET))
      .Children({
        Label::New("PRIMARY TARGET")
          .SetBackgroundColor(UiColor(0xDD6B20))
          .SetRequestedWidth(MATCH_PARENT)
          .SetRequestedHeight(MATCH_PARENT)
          .SetFontSize(SUBLABEL_FONT)
          .SetTextColor(UiColor(0xFFFFFF))
          .SetMultiLine(true)
          .SetRequestedWidth(MATCH_PARENT)
          .SetRequestedHeight(MATCH_PARENT)
          .As(target)
      })
      .SetStateEffectPrimaryTarget(target);
  }
};

REGISTER_MANUAL_TEST(TcOverlayEffect)
