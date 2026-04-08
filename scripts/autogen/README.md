# Chaining Method Code Generator

Fluent method chaining을 위한 코드 자동 생성 도구입니다.

## 목적

DALi UI의 View, Label 등 public-api 클래스는 fluent chaining 패턴을 지원합니다:

```cpp
Label::New()
  .SetText("Hello")
  .SetFontSize(24)
  .SetBackgroundColor(UiColor::RED)
  .SetOpacity(0.8f);  // View에서 상속받은 메소드
```

derived class(Label)에서 부모(View)의 체이닝 메소드를 사용하려면
리턴 타입을 `Label&`로 오버로딩해야 합니다.
이 스크립트가 해당 오버로딩 코드를 자동 생성합니다.

## 생성 파일

스크립트는 `@CHAIN_CLASS`에 parent가 지정된 클래스에 대해 두 가지 파일을 생성합니다:

| 파일 | 용도 |
|---|---|
| `classname.autogen.h` | 실제 inline 함수 코드. 라이브러리 내부에서 사용. 디버거에서 브레이크포인트/스텝인 가능 |
| `classname.macro.autogen.h` | `#define` 매크로. 라이브러리 외부의 derived class에서 사용 |

parent가 없는 base class (View, UiConfig 등)는 `macro.autogen.h`만 생성합니다.

## 태그

헤더 파일에 다음 태그를 사용하여 체이닝 메소드를 지정합니다:

| 태그 | 용도 |
|---|---|
| `// @CHAIN_CLASS(Cls[, Parent])` | 클래스 등록. parent 지정 시 autogen 자동 생성 |
| `// @CHAIN` | 바로 다음 메소드를 체이닝 목록에 수집 |
| `// @CHAIN_SELF` | body를 복사하고 클래스명을 ChildClass로 치환 (`With()` 등) |

### 사용 예

```cpp
// view.h — base class
class View : public CustomActor
{
  // @CHAIN_CLASS(View)

  // @CHAIN
  View& SetOpacity(float opacity);

  // @CHAIN
  View& SetVisibility(bool visibility);
};

// label.h — derived class
class Label : public View
{
  // @CHAIN_CLASS(Label, View)

  // @CHAIN
  Label& SetText(const Dali::String& text);
};
```

## 생성 결과

위 코드에 대해 스크립트를 실행하면 다음 파일이 생성됩니다:

* **view.macro.autogen.h** — `DALI_UI_CHAIN_VIEW_METHODS` 매크로 제공 (view.h에 포함)

* **label.macro.autogen.h** — `DALI_UI_CHAIN_LABEL_METHODS` 매크로 제공 (label.h에 포함)

* **label.autogen.h** — View의 메소드를 Label& 리턴으로 오버로딩한 코드 (label.h에 포함)

앱 개발자는 생성된 매크로를 사용하여 자신의 클래스에서 체이닝 메소드를 오버로드 할 수 있습니다:

```cpp
#include <dali-ui-foundation/public-api/label.h>

class MyLabel : public Label
{
public:
  DALI_UI_CHAIN_LABEL_METHODS(MyLabel)
};

// View + Label의 모든 체이닝 메소드가 MyLabel& 리턴으로 사용 가능
MyLabel::New()
  .SetText("Hello")      // Label 메소드
  .SetOpacity(0.5f);      // View 메소드
```

<br/>

## 확장 라이브러리에서 사용

스크립트와 CMake 모듈을 빌드에 통합하면 확장 라이브러리에서도
동일한 태그 기반 코드 생성을 사용할 수 있습니다.

## CMake 통합

```cmake
# autogen.cmake를 include
include(/path/to/scripts/autogen/autogen.cmake)

# 타겟과 헤더 디렉토리를 등록
dali_ui_autogen_chaining_macro(my-library ${CMAKE_SOURCE_DIR}/public-api)
```

이렇게 하면 빌드 시 `public-api/` 하위의 모든 헤더를 스캔하여
`@CHAIN_CLASS` 태그가 있는 파일에 대해 autogen 파일을 생성합니다.

## 직접 실행

```bash
python3 gen-chaining-macro.py <directory_or_file> [...]
```

디렉토리를 지정하면 하위의 모든 `.h` 파일을 스캔합니다.
