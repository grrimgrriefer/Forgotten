// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "MainMenuUiTask.h"
#include "StateTreeAsyncExecutionContext.h"
#include "StateTreeExecutionContext.h"
#include "Forgotten/Widgets/MainMenuWidget.h"
#include "Kismet/KismetSystemLibrary.h"

const UScriptStruct* FMainMenuUiTask::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}
EStateTreeRunStatus FMainMenuUiTask::EnterState(
	FStateTreeExecutionContext& context,
	const FStateTreeTransitionResult& transitions) const
{
	FInstanceDataType& instanceData = context.GetInstanceData(*this);
	const UWorld* world = context.GetWorld();

	if (!world || !world->IsGameWorld() || !m_MenuWidgetClass)
	{
		return EStateTreeRunStatus::Failed;
	}

	APlayerController* playerController = world->GetFirstPlayerController();
	if (!playerController)
	{
		return EStateTreeRunStatus::Failed;
	}
	if (UMainMenuWidget* mainMenuWidget = CreateWidget<UMainMenuWidget>(playerController, m_MenuWidgetClass))
	{
		mainMenuWidget->AddToViewport();
		instanceData.m_WidgetPtr = mainMenuWidget;
		instanceData.m_PlayerControllerPtr = playerController;

		FInputModeUIOnly inputMode;
		inputMode.SetWidgetToFocus(mainMenuWidget->TakeWidget());
		inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		instanceData.m_PlayerControllerPtr->SetInputMode(inputMode);
		instanceData.m_PlayerControllerPtr->SetShowMouseCursor(true);

		instanceData.m_ActionDelegateHandle = mainMenuWidget->m_OnActionRequested.AddLambda(
			[weakContext = context.MakeWeakExecutionContext()](const EMainMenuAction action) mutable
			{
				const FStateTreeStrongExecutionContext strongContext = weakContext.MakeStrongExecutionContext();
				if (!strongContext.IsValid())
				{
					return;
				}

				if (action == EMainMenuAction::Quit)
				{
					const UObject* owner = strongContext.GetOwner().Get();
					UKismetSystemLibrary::QuitGame(
						owner ? owner->GetWorld() : nullptr,
						nullptr,
						EQuitPreference::Quit,
						false);
				}
				else if (action == EMainMenuAction::Continue)
				{
					strongContext.FinishTask(EStateTreeFinishTaskType::Succeeded);
				}
			});
		return EStateTreeRunStatus::Running;
	}
	return EStateTreeRunStatus::Failed;
}
void FMainMenuUiTask::ExitState(
	FStateTreeExecutionContext& context,
	const FStateTreeTransitionResult& transitions) const
{
	FInstanceDataType& instanceData = context.GetInstanceData(*this);

	if (APlayerController* playerController = instanceData.m_PlayerControllerPtr.Get())
	{
		const FInputModeGameOnly gameMode;
		playerController->SetInputMode(gameMode);
		playerController->SetShowMouseCursor(false);
	}

	if (UMainMenuWidget* Widget = instanceData.m_WidgetPtr.Get())
	{
		Widget->m_OnActionRequested.Remove(instanceData.m_ActionDelegateHandle);
		Widget->RemoveFromParent();
	}
	instanceData.m_WidgetPtr = nullptr;
	instanceData.m_PlayerControllerPtr = nullptr;
}
