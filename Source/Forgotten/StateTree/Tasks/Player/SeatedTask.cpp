// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "SeatedTask.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Forgotten/Character/FirstPersonCharacter.h"
#include "Forgotten/Interactables/Implementations/ChairInteractable.h"
#include "Forgotten/Utils/AssertMacros.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"

FSeatedTask::FSeatedTask()
{
	bShouldCallTick = true;
}
const UScriptStruct* FSeatedTask::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}
bool FSeatedTask::Link(FStateTreeLinker& linker)
{
	linker.LinkExternalData(m_PlayerCharacterHandle);
	return true;
}
EStateTreeRunStatus FSeatedTask::EnterState(FStateTreeExecutionContext& context, const FStateTreeTransitionResult& transitions) const
{
	const AFirstPersonCharacter* player = context.GetExternalDataPtr(m_PlayerCharacterHandle);
	const FInstanceDataType& instanceData = context.GetInstanceData(*this);
	AChairInteractable* chair = instanceData.m_Chair;

	ASSERT_CHECK_RETURN(player, EStateTreeRunStatus::Failed);
	ASSERT_CHECK_RETURN(chair, EStateTreeRunStatus::Failed);

	chair->SetIsOccupied(true);

	UCharacterMovementComponent* moveComp = player->GetCharacterMovement();
	ASSERT_CHECK_RETURN(moveComp, EStateTreeRunStatus::Failed);
	moveComp->SetMovementMode(MOVE_None);

	player->GetCapsuleComponent()->IgnoreActorWhenMoving(chair, true);

	return EStateTreeRunStatus::Running;
}
void FSeatedTask::ExitState(FStateTreeExecutionContext& context, const FStateTreeTransitionResult& transitions) const
{
	AFirstPersonCharacter* player = context.GetExternalDataPtr(m_PlayerCharacterHandle);
	const FInstanceDataType& instanceData = context.GetInstanceData(*this);
	AChairInteractable* chair = instanceData.m_Chair;

	if (player)
	{
		if (UCharacterMovementComponent* moveComp = player->GetCharacterMovement())
		{
			moveComp->SetMovementMode(MOVE_Walking);
		}

		if (chair)
		{
			player->GetCapsuleComponent()->IgnoreActorWhenMoving(chair, false);
			const FVector standUpOffset = chair->GetActorForwardVector() * 60.0f + FVector(0, 0, 30.0f);
			player->SetActorLocation(chair->GetActorLocation() + standUpOffset, true);

			chair->SetIsOccupied(false);
		}
	}
}
EStateTreeRunStatus FSeatedTask::Tick(FStateTreeExecutionContext& context, const float deltaTime) const
{
	AFirstPersonCharacter* player = context.GetExternalDataPtr(m_PlayerCharacterHandle);
	const FInstanceDataType& instanceData = context.GetInstanceData(*this);
	const AChairInteractable* chair = instanceData.m_Chair;

	if (player && chair)
	{
		const USceneComponent* targetPoint = chair->GetInteractionPoint();
		const FVector targetLoc = targetPoint ? targetPoint->GetComponentLocation() : chair->GetActorLocation();
		const FRotator targetRot = targetPoint ? targetPoint->GetComponentRotation() : chair->GetActorRotation();

		const FVector newLoc = FMath::VInterpTo(player->GetActorLocation(), targetLoc, deltaTime, 8.0f);
		player->SetActorLocation(newLoc);

		if (APlayerController* playerController = Cast<APlayerController>(player->GetController()))
		{
			const FRotator newRot = FMath::RInterpTo(playerController->GetControlRotation(), targetRot, deltaTime, 8.0f);
			playerController->SetControlRotation(newRot);
		}
	}

	return EStateTreeRunStatus::Running;
}
