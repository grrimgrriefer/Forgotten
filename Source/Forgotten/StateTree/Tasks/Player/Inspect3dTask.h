// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "Inspect3dTask.generated.h"

class ASodaCanInteractable;
class UStaticMesh;
class AFirstPersonCharacter;
class UStaticMeshComponent;

USTRUCT(BlueprintType)
struct FInspect3dPayload
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspect3d")
	TObjectPtr<ASodaCanInteractable> m_Inspectable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspect3d")
	FVector m_PreviewOffset = FVector(50.0f, 0.0f, -10.0f);
};

USTRUCT()
struct FInspect3dTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<ASodaCanInteractable> m_Inspectable = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	FVector m_PreviewOffset = FVector(50.0f, 0.0f, -10.0f);

	UPROPERTY(Transient)
	TObjectPtr<UStaticMeshComponent> m_PreviewMeshComp = nullptr;

	FVector2D m_AccumulatedRotationDelta = FVector2D::ZeroVector;
};

/**
 * Handles the lifetime of the '3D preview interaction' state.
 * Where the player can look up close at something and rotate it etc.
 */
USTRUCT(meta = (DisplayName = "3D Inspect State", Category = "Gameplay"))
struct FORGOTTEN_API FInspect3dTask : public FStateTreeTaskBase
{
	GENERATED_BODY()

public:
	using FInstanceDataType = FInspect3dTaskInstanceData;

	FInspect3dTask();

	virtual const UScriptStruct* GetInstanceDataType() const override;
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& context,
		const FStateTreeTransitionResult& transitions) const override;
	virtual void ExitState(
		FStateTreeExecutionContext& context,
		const FStateTreeTransitionResult& transitions) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& context, const float deltaTime) const override;

	TStateTreeExternalDataHandle<AFirstPersonCharacter> m_PlayerCharacterHandle;
};
