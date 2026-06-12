# OverlayEffect

InteractiveView에 기본 state effect로 적용되는 OverlayEffect가 Pressed/Focused 상태와 primary target 설정을 올바르게 반영하는지 확인한다.

## 화면 구성

- `TOUCH FOCUS ON`: 터치 시 Focused 상태가 유지되는 카드
- `TOUCH FOCUS OFF`: 터치로 Focused 상태가 되지 않는 카드
- `PRIMARY TARGET`: owner가 터치를 받되 overlay는 안쪽 target에 표시되는 카드
- `ROUND`: `OverlayEffect::Round()` preset을 적용한 카드

## 테스트 1: Touch focus ON 카드

1. `TOUCH FOCUS ON` 카드를 짧게 탭한 뒤 손을 뗀다.
2. **기대 결과**: 카드가 Focused 상태로 남아 기본 overlay보다 진한 dim이 유지된다.
3. 같은 카드를 다시 누른 채로 유지한다.
4. **기대 결과**: Focused overlay와 Pressed overlay가 함께 적용된 것처럼 더 어둡게 보인다.
5. 손을 뗀다.
6. **기대 결과**: Pressed overlay만 사라지고 Focused overlay는 유지된다.

## 테스트 2: Touch focus OFF 카드

1. `TOUCH FOCUS OFF` 카드를 누른 채로 유지한다.
2. **기대 결과**: 누르고 있는 동안에만 Pressed overlay가 표시된다.
3. 손을 뗀다.
4. **기대 결과**: Focused 상태로 남지 않으므로 overlay가 완전히 사라진다.


## 테스트 3: Primary target 카드

1. `PRIMARY TARGET` 카드를 누른 채로 유지한다.
2. **기대 결과**: 안쪽 주황색 target 영역에만 Pressed overlay가 표시된다.
3. 손을 뗀다.
4. **기대 결과**: target overlay가 완전히 사라진다.

## 테스트 4: Round preset 카드

1. `ROUND` 카드를 누른 채로 유지한다.
2. **기대 결과**: target view의 corner radius와 관계없이 relative `0.5` radius overlay가 표시된다.
3. 손을 뗀다.
4. **기대 결과**: Pressed overlay가 사라진다.

## 통과 기준

- Pressed와 Focused가 동시에 적용될 때 overlay가 중첩된 것처럼 더 어둡게 보여야 한다.
- touch focusable이 꺼진 카드는 release 후 overlay가 남지 않아야 한다.
- primary target이 설정된 카드는 overlay가 owner가 아니라 target view에만 표시되어야 한다.
- `OverlayEffect::Round()`는 항상 relative `0.5` corner radius를 사용해야 한다.
