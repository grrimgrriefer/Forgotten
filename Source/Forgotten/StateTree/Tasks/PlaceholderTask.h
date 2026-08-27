// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "PlaceholderTask.generated.h"

USTRUCT()
struct FPlaceholderTaskInstanceData
{
	GENERATED_BODY()
};

/**
 * Placeholder task to avoid the StateTree from transitioning,
 * i.e. keep it in place
 */
USTRUCT(meta = (DisplayName = "Placeholder Task", Category = "Main Flow"))
struct FORGOTTEN_API FPlaceholderTask : public FStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FPlaceholderTaskInstanceData;

	FPlaceholderTask();

	virtual const UScriptStruct* GetInstanceDataType() const override;
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& context, const FStateTreeTransitionResult& transitions) const override;
};
