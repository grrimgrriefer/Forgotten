// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "IsLoadedLevelCondition.generated.h"

USTRUCT()
struct FIsLoadedLevelConditionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Level")
	TSoftObjectPtr<UWorld> m_LevelToCheck;
	UPROPERTY(EditAnywhere, Category = "Condition")
	bool m_Invert = false;
};

/**
 * Checks if the specified level is currently loaded as the active world.
 */
USTRUCT(meta = (DisplayName = "Is Loaded Level", Category = "Level"))
struct FORGOTTEN_API FIsLoadedLevelCondition : public FStateTreeConditionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FIsLoadedLevelConditionInstanceData;

	virtual const UScriptStruct* GetInstanceDataType() const override;
	virtual bool TestCondition(FStateTreeExecutionContext& context) const override;
#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& id,
								FStateTreeDataView instanceDataView,
								const IStateTreeBindingLookup& bindingLookup,
								EStateTreeNodeFormatting formatting) const override;
#endif
};
