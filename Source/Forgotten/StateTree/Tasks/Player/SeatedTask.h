// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "SeatedTask.generated.h"

class AFirstPersonCharacter;
class AChairInteractable;

USTRUCT(BlueprintType)
struct FSeatedPayload
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TObjectPtr<AChairInteractable> m_Chair = nullptr;
};

USTRUCT()
struct FSeatedTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AChairInteractable> m_Chair = nullptr;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = "0.0", ClampMax = "180.0"))
	float m_MaxYawAngle = 80.0f;
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = "0.0", ClampMax = "90.0"))
	float m_MaxPitchAngle = 60.0f;
};

/**
 * Handles the lifetime of the 'sitting down' state.
 * Currently only for player. TODO: maybe generic for NPC too? Or separate variant maybe. Idk yet
 */
USTRUCT(meta = (DisplayName = "Seated State", Category = "Gameplay"))
struct FORGOTTEN_API FSeatedTask : public FStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSeatedTaskInstanceData;

	FSeatedTask();

	virtual const UScriptStruct* GetInstanceDataType() const override;
	virtual bool Link(FStateTreeLinker& linker) override;
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& context, const FStateTreeTransitionResult& transitions) const override;
	virtual void ExitState(FStateTreeExecutionContext& context, const FStateTreeTransitionResult& transitions) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& context, const float deltaTime) const override;

	TStateTreeExternalDataHandle<AFirstPersonCharacter> m_PlayerCharacterHandle;
};
