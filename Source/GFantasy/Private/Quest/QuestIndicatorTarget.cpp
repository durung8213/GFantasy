
#include "Quest/QuestIndicatorTarget.h"


UQuestIndicatorTarget::UQuestIndicatorTarget()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UQuestIndicatorTarget::BeginPlay()
{
	Super::BeginPlay();
}

void UQuestIndicatorTarget::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

