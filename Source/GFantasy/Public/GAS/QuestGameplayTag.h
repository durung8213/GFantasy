// �����÷��� �±׸� �� ���ϰ� ����� �� �ְ� ��� �߰�
#pragma once

// �� ������ϸ� �߰��ϸ�����÷��� �±׿� ���õ� �͵� �ڵ����� �����ϵ���
#include "GameplayTagContainer.h"

// �� ��� ��ü�� �ϳ��� ����ó�� ���ȴ�
// ��Ÿ�� ������ �ش� �±��̸��� ������� �ε�, �����Ϳ��� ��ϵǾ��־�� �Ѵ�.
// �������� �̱��� ������ �±װ� �ڵ忡�� �����ϰ� �ڵ����� ��ϵ�.
#define QUEST_TYPE_INTERACTION FGameplayTag::RequestGameplayTag(FName("Quest.Type.Interaction"))
#define QUEST_TYPE_COLLECTION FGameplayTag::RequestGameplayTag(FName("Quest.Type.Collection"))
#define QUEST_TYPE_KILLCOUNT FGameplayTag::RequestGameplayTag(FName("Quest.Type.KillCount"))
#define QUEST_TYPE_DELIVERY FGameplayTag::RequestGameplayTag(FName("Quest.Type.Delivery"))