// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "FocusedConversationTask.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Camera/CameraComponent.h"
#include "Forgotten/Character/FirstPersonCharacter.h"
#include "Forgotten/Character/ConversableNPC.h"
#include "Forgotten/Utils/AssertMacros.h"
#include "../Plugins/UnrealVoxta/Source/UnrealVoxta/Public/SubSystems/CharacterSubsystem.h"

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
	linker.LinkExternalData(m_CharacterSubsystemHandle);
	return true;
}
EStateTreeRunStatus FFocusedConversationTask::EnterState(FStateTreeExecutionContext& context, const FStateTreeTransitionResult& transitions) const
{
	UCharacterSubsystem* characterSubsystem = context.GetExternalDataPtr(m_CharacterSubsystemHandle);
	AFirstPersonCharacter* playerCharacter = context.GetExternalDataPtr(m_PlayerCharacterHandle);
	const FInstanceDataType& instanceData = context.GetInstanceData(*this);
	AConversableNPC* conversableNpc = instanceData.m_ConversableNpc;

	ASSERT_CHECK_RETURN(characterSubsystem, EStateTreeRunStatus::Failed);
	ASSERT_CHECK_RETURN(playerCharacter, EStateTreeRunStatus::Failed);
	ASSERT_CHECK_RETURN(conversableNpc, EStateTreeRunStatus::Failed);
	playerCharacter->TryBindContextData(conversableNpc);
	characterSubsystem->StartConversation(conversableNpc);
	playerCharacter->EnterFocusedConvoMode();

	return EStateTreeRunStatus::Running;
}
void FFocusedConversationTask::ExitState(FStateTreeExecutionContext& context, const FStateTreeTransitionResult& transitions) const
{
	UCharacterSubsystem* characterSubsystem = context.GetExternalDataPtr(m_CharacterSubsystemHandle);
	AFirstPersonCharacter* playerCharacter = context.GetExternalDataPtr(m_PlayerCharacterHandle);
	const FInstanceDataType& instanceData = context.GetInstanceData(*this);
	AConversableNPC* conversableNpc = instanceData.m_ConversableNpc;

	if (conversableNpc && playerCharacter)
	{
		playerCharacter->TryUnbindContextData(conversableNpc);
	}

	if (characterSubsystem)
	{
		characterSubsystem->StartConversation(nullptr);
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
