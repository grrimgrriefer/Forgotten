// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "Templates/SubclassOf.h"
#include "MainMenuUiTask.generated.h"

class UMainMenuWidget;

USTRUCT()
struct FMainMenuUiTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TWeakObjectPtr<UMainMenuWidget> m_WidgetPtr = nullptr;
	UPROPERTY(Transient)
	TWeakObjectPtr<APlayerController> m_PlayerControllerPtr = nullptr;

	FDelegateHandle m_ActionDelegateHandle;
};

/**
 * Manages the lifespan of the Main Menu and forwards the user's desires to the main StateTree
 */
USTRUCT(meta = (DisplayName = "Manage Main Menu UI", Category = "Main Flow"))
struct FORGOTTEN_API FMainMenuUiTask : public FStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FMainMenuUiTaskInstanceData;

	FMainMenuUiTask();

	virtual const UScriptStruct* GetInstanceDataType() const override;
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& context, const FStateTreeTransitionResult& transitions) const override;
	virtual void ExitState(FStateTreeExecutionContext& context, const FStateTreeTransitionResult& transitions) const override;

	UPROPERTY(EditAnywhere, Category = "Main Menu UI")
	TSubclassOf<UMainMenuWidget> m_MenuWidgetClass;
};
