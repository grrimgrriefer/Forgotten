// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "Templates/SubclassOf.h"
#include "MainMenuUiTask.generated.h"

class UMainMenuWidget;

/** 
 * Holds a reference to the instantiated Main Menu widget
 */
USTRUCT()
struct FMainMenuUiTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TWeakObjectPtr<UMainMenuWidget> m_WidgetPtr = nullptr;
};

/**
 * Manages the lifespan of the Main Menu and forwards the user's desires to the main StateTree
 */
USTRUCT(meta = (DisplayName = "Manage Main Menu UI"))
struct FORGOTTEN_API FMainMenuUiTask : public FStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FMainMenuUiTaskInstanceData;

	virtual const UScriptStruct* GetInstanceDataType() const override;
	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& context, 
		const FStateTreeTransitionResult& transitions) const override;
	virtual void ExitState(
		FStateTreeExecutionContext& context, 
		const FStateTreeTransitionResult& transitions) const override;
	
	UPROPERTY(EditAnywhere, Category = "Man Menu UI")
	TSubclassOf<UMainMenuWidget> m_MenuWidgetClass;
};
