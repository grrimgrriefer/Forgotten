// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "Inspect3dTask.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Forgotten/Character/FirstPersonCharacter.h"
#include "Forgotten/Utils/AssertMacros.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"

FInspect3dTask::FInspect3dTask()
{
	bShouldCallTick = true;
}
const UScriptStruct* FInspect3dTask::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}
bool FInspect3dTask::Link(FStateTreeLinker& linker)
{
	linker.LinkExternalData(m_PlayerCharacterHandle);
	return true;
}
EStateTreeRunStatus FInspect3dTask::EnterState(FStateTreeExecutionContext& context, const FStateTreeTransitionResult& transitions) const
{
	AFirstPersonCharacter* player = context.GetExternalDataPtr(m_PlayerCharacterHandle);
	FInstanceDataType& instanceData = context.GetInstanceData(*this);
	ASodaCanInteractable* interactable = instanceData.m_Inspectable;

	ASSERT_CHECK_RETURN(player, EStateTreeRunStatus::Failed);
	ASSERT_CHECK_RETURN(interactable, EStateTreeRunStatus::Failed);

	UCharacterMovementComponent* moveComp = player->GetCharacterMovement();
	ASSERT_CHECK_RETURN(moveComp, EStateTreeRunStatus::Failed);
	moveComp->SetMovementMode(MOVE_None);

	APlayerController* playerController = Cast<APlayerController>(player->GetController());
	ASSERT_CHECK_RETURN(playerController, EStateTreeRunStatus::Failed);
	playerController->SetIgnoreLookInput(true);

	interactable->SetIsBeingInspected(true);

	UStaticMesh* inspectMesh = interactable->GetInspectMesh();
	ASSERT_CHECK_RETURN(inspectMesh, EStateTreeRunStatus::Failed);
	UStaticMeshComponent* previewComp = NewObject<UStaticMeshComponent>(player, TEXT("InspectPreviewMesh"));
	ASSERT_CHECK_RETURN(previewComp, EStateTreeRunStatus::Failed);

	previewComp->RegisterComponent();
	previewComp->SetStaticMesh(inspectMesh);
	previewComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	previewComp->AttachToComponent(player->GetCameraComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	previewComp->SetRelativeLocation(instanceData.m_PreviewOffset);
	previewComp->SetRelativeRotation(FRotator::ZeroRotator);

	instanceData.m_PreviewMeshComp = previewComp;

	return EStateTreeRunStatus::Running;
}
void FInspect3dTask::ExitState(FStateTreeExecutionContext& context, const FStateTreeTransitionResult& transitions) const
{
	const AFirstPersonCharacter* player = context.GetExternalDataPtr(m_PlayerCharacterHandle);
	FInstanceDataType& instanceData = context.GetInstanceData(*this);
	ASodaCanInteractable* interactable = instanceData.m_Inspectable;

	if (instanceData.m_PreviewMeshComp)
	{
		instanceData.m_PreviewMeshComp->DestroyComponent();
		instanceData.m_PreviewMeshComp = nullptr;
	}

	if (interactable)
	{
		interactable->SetIsBeingInspected(false);
	}

	if (player)
	{
		if (UCharacterMovementComponent* moveComp = player->GetCharacterMovement())
		{
			moveComp->SetMovementMode(MOVE_Walking);
		}

		if (APlayerController* playerController = Cast<APlayerController>(player->GetController()))
		{
			playerController->ResetIgnoreLookInput();
		}
	}
}
EStateTreeRunStatus FInspect3dTask::Tick(FStateTreeExecutionContext& context, const float deltaTime) const
{
	const AFirstPersonCharacter* player = context.GetExternalDataPtr(m_PlayerCharacterHandle);
	const FInstanceDataType& instanceData = context.GetInstanceData(*this);

	if (player && instanceData.m_PreviewMeshComp)
	{
		if (const APlayerController* pc = Cast<APlayerController>(player->GetController()))
		{
			float mouseX = 0.0f;
			float mouseY = 0.0f;
			pc->GetInputMouseDelta(mouseX, mouseY);

			constexpr float rotationSpeed = 2.0f;
			instanceData.m_PreviewMeshComp->AddLocalRotation(FRotator(-mouseY * rotationSpeed, -mouseX * rotationSpeed, 0.0f));
		}
	}

	return EStateTreeRunStatus::Running;
}
