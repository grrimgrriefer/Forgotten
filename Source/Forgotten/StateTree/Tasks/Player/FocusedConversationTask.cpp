// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "FocusedConversationTask.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Forgotten/Character/FirstPersonCharacter.h"
#include "Forgotten/Character/ConversableNPC.h"
#include "Forgotten/SubSystems/ConversationSubsystem.h"
#include "Forgotten/Utils/AssertMacros.h"

FFocusedConversationTask::FFocusedConversationTask()
{
	bShouldCallTick = true;
}
const UScriptStruct* FFocusedConversationTask::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}
bool FFocusedConversationTask::Link(FStateTreeLinker& linker)
{
	linker.LinkExternalData(m_PlayerCharacterHandle);
	return true;
}
EStateTreeRunStatus FFocusedConversationTask::EnterState(FStateTreeExecutionContext& context, const FStateTreeTransitionResult& transitions) const
{
	AFirstPersonCharacter* playerCharacter = context.GetExternalDataPtr(m_PlayerCharacterHandle);
	const FInstanceDataType& instanceData = context.GetInstanceData(*this);
	AConversableNPC* conversableNpc = instanceData.m_ConversableNpc;

	ASSERT_CHECK_RETURN(playerCharacter, EStateTreeRunStatus::Failed);
	ASSERT_CHECK_RETURN(conversableNpc, EStateTreeRunStatus::Failed);
	playerCharacter->TryBindContextData(conversableNpc);

	const UWorld* world = context.GetWorld();
	UConversationSubsystem* conversationSubsystem = world->GetSubsystem<UConversationSubsystem>();

	ASSERT_CHECK_RETURN(conversationSubsystem, EStateTreeRunStatus::Failed);
	conversationSubsystem->SetCurrentConversableNpc(conversableNpc);
	playerCharacter->EnterFocusedConvoMode();

	return EStateTreeRunStatus::Running;
}
void FFocusedConversationTask::ExitState(FStateTreeExecutionContext& context, const FStateTreeTransitionResult& transitions) const
{
	AFirstPersonCharacter* playerCharacter = context.GetExternalDataPtr(m_PlayerCharacterHandle);
	const FInstanceDataType& instanceData = context.GetInstanceData(*this);
	AConversableNPC* conversableNpc = instanceData.m_ConversableNpc;

	if (conversableNpc && playerCharacter)
	{
		playerCharacter->TryUnbindContextData(conversableNpc);
	}

	const UWorld* world = context.GetWorld();
	if (UConversationSubsystem* conversationSubsystem = world ? world->GetSubsystem<UConversationSubsystem>() : nullptr)
	{
		conversationSubsystem->SetCurrentConversableNpc(nullptr);
	}

	if (playerCharacter)
	{
		playerCharacter->ExitFocusedConvoMode();
	}
}
EStateTreeRunStatus FFocusedConversationTask::Tick(FStateTreeExecutionContext& context, const float deltaTime) const
{
	const AFirstPersonCharacter* player = context.GetExternalDataPtr(m_PlayerCharacterHandle);
	const FInstanceDataType& instanceData = context.GetInstanceData(*this);
	const AConversableNPC* npc = instanceData.m_ConversableNpc;

	if (player && npc)
	{
		if (APlayerController* playerController = Cast<APlayerController>(player->GetController()))
		{
			const FVector cameraLoc = player->GetCameraComponent()->GetComponentLocation();
			const FVector targetLoc = npc->GetActorLocation();
			const FRotator targetRotation = (targetLoc - cameraLoc).Rotation();

			const FRotator currentRotation = playerController->GetControlRotation();
			const FRotator newRotation = FMath::RInterpTo(
				currentRotation,
				targetRotation,
				deltaTime,
				player->m_cameraInterpSpeed);

			playerController->SetControlRotation(newRotation);
		}
	}

	return EStateTreeRunStatus::Running;
}
