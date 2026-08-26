// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "LoadLevelTask.generated.h"

USTRUCT()
struct FLoadLevelTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Level")
	TSoftObjectPtr<UWorld> m_LevelToLoad;
};

/**
 * Manages the loading of levels and the switching between them
 */
USTRUCT(meta = (DisplayName = "Load Level", Category = "Main Flow"))
struct FORGOTTEN_API FLoadLevelTask : public FStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FLoadLevelTaskInstanceData;

	FLoadLevelTask();

	virtual const UScriptStruct* GetInstanceDataType() const override;
	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& context,
		const FStateTreeTransitionResult& transitions) const override;
};
