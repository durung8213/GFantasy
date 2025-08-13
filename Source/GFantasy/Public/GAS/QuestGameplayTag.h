// 게임플레이 태그를 더 편하게 사용할 수 있게 몇가지 추가
#pragma once

// 이 헤더파일만 추가하면게임플레이 태그에 관련된 것들 자동으로 포함하도록
#include "GameplayTagContainer.h"

// 이 명령 자체가 하나의 변수처럼 사용된다
// 런타임 시점에 해당 태그이름을 기반으로 로드, 에디터에도 등록되어있어야 한다.
// 민형님의 싱글턴 구조는 태그가 코드에만 존재하고 자동으로 등록됨.
#define QUEST_TYPE_INTERACTION FGameplayTag::RequestGameplayTag(FName("Quest.Type.Interaction"))
#define QUEST_TYPE_COLLECTION FGameplayTag::RequestGameplayTag(FName("Quest.Type.Collection"))
#define QUEST_TYPE_KILLCOUNT FGameplayTag::RequestGameplayTag(FName("Quest.Type.KillCount"))
#define QUEST_TYPE_DELIVERY FGameplayTag::RequestGameplayTag(FName("Quest.Type.Delivery"))