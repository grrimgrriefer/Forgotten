// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "FocusedConversationTask.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Forgotten/Character/FirstPersonCharacter.h"
#include "Forgotten/Character/ConversableNPC.h"
#include "Forgotten/Utils/AssertMacros.h"
#include "SubSystems/CharacterSubsystem.h"

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

	APlayerController* playerController = Cast<APlayerController>(playerCharacter->GetController());
	ASSERT_CHECK_RETURN(playerController, EStateTreeRunStatus::Failed);

	UCharacterMovementComponent* moveComp = playerCharacter->GetCharacterMovement();
	ASSERT_CHECK_RETURN(moveComp, EStateTreeRunStatus::Failed);

	moveComp->SetMovementMode(MOVE_None);
	playerController->SetIgnoreMoveInput(true);

	playerCharacter->TryBindContextData(conversableNpc);
	characterSubsystem->StartConversation(conversableNpc);
	playerCharacter->FocusChatInput();

	return EStateTreeRunStatus::Running;
}
void FFocusedConversationTask::ExitState(FStateTreeExecutionContext& context, const FStateTreeTransitionResult& transitions) const
{
	UCharacterSubsystem* characterSubsystem = context.GetExternalDataPtr(m_CharacterSubsystemHandle);
	AFirstPersonCharacter* playerCharacter = context.GetExternalDataPtr(m_PlayerCharacterHandle);
	const FInstanceDataType& instanceData = context.GetInstanceData(*this);
	AConversableNPC* conversableNpc = instanceData.m_ConversableNpc;

	if (playerCharacter)
	{
		if (conversableNpc)
		{
			playerCharacter->TryUnbindContextData(conversableNpc);
		}

		if (APlayerController* playerController = Cast<APlayerController>(playerCharacter->GetController()))
		{
			playerController->SetIgnoreMoveInput(false);
			playerController->ResetIgnoreLookInput();
		}

		if (UCharacterMovementComponent* moveComp = playerCharacter->GetCharacterMovement())
		{
			moveComp->SetMovementMode(MOVE_Walking);
		}

		playerCharacter->UnfocusChatInput();
	}

	if (characterSubsystem)
	{
		characterSubsystem->StopConversation();
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
			const FRotator lerpedRotation = FMath::RInterpTo(
				currentRotation,
				targetRotation,
				deltaTime,
				instanceData.m_ReturnInterpSpeed
			);

			const float yawDelta = FMath::FindDeltaAngleDegrees(targetRotation.Yaw, lerpedRotation.Yaw);
			const float clampedYawDelta = FMath::Clamp(yawDelta, -instanceData.m_MaxYawAngle, instanceData.m_MaxYawAngle);

			const float pitchDelta = FMath::FindDeltaAngleDegrees(targetRotation.Pitch, lerpedRotation.Pitch);
			const float clampedPitchDelta = FMath::Clamp(pitchDelta, -instanceData.m_MaxPitchAngle, instanceData.m_MaxPitchAngle);

			const FRotator newRotation(
				FRotator::NormalizeAxis(targetRotation.Pitch + clampedPitchDelta),
				FRotator::NormalizeAxis(targetRotation.Yaw + clampedYawDelta),
				currentRotation.Roll
			);

			playerController->SetControlRotation(newRotation);
		}
	}

	return EStateTreeRunStatus::Running;
}
