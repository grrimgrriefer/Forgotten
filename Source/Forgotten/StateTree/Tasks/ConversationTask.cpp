// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "ConversationTask.h"
#include "StateTreeExecutionContext.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Forgotten/Character/FirstPersonCharacter.h"
#include "Forgotten/Character/RainNPC.h"

FConversationTask::FConversationTask()
{
	bShouldCallTick = false;
}
const UScriptStruct* FConversationTask::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}
EStateTreeRunStatus FConversationTask::EnterState(
	FStateTreeExecutionContext& context,
	const FStateTreeTransitionResult& transitions) const
{
	const UWorld* world = context.GetWorld();
	FInstanceDataType& instanceData = context.GetInstanceData(*this);

	check(world);
	APlayerController* playerController = world->GetFirstPlayerController();

	check(playerController);
	AFirstPersonCharacter* playerCharacter = Cast<AFirstPersonCharacter>(playerController->GetPawn());
	ARainNPC* rainNPC = Cast<ARainNPC>(UGameplayStatics::GetActorOfClass(world, ARainNPC::StaticClass()));

	check(playerCharacter);
	check(rainNPC);
	instanceData.m_PlayerCharacter = playerCharacter;
	instanceData.m_TargetNPC = rainNPC;
	playerCharacter->EnterConversationMode(rainNPC);

	return EStateTreeRunStatus::Running;
}

void FConversationTask::ExitState(
	FStateTreeExecutionContext& context,
	const FStateTreeTransitionResult& transitions) const
{
	FInstanceDataType& instanceData = context.GetInstanceData(*this);

	if (AFirstPersonCharacter* playerCharacter = instanceData.m_PlayerCharacter.Get())
	{
		playerCharacter->ExitConversationMode();
	}

	instanceData.m_PlayerCharacter = nullptr;
	instanceData.m_TargetNPC = nullptr;
}
