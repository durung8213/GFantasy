// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/QuestDataTableRow.h"


FQuestDataTableRow::FQuestDataTableRow()
    : bCanAcceptQuest(true)
    , D_QuestID(NAME_None)
    , D_QuestName(FText::GetEmpty())
    , D_QuestDescription(FText::GetEmpty())
    , D_QuestType(EQuestType::Collection)
    , D_RequiredCount(10)
    , D_CurrentCount(0)
    , D_RewardCount(0)
{
}

