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
	UWorld* world = context.GetWorld();
	 
	if (world && world->IsGameWorld() && m_MenuWidgetClass)
	{
		if (UMainMenuWidget* mainMenuWidget = CreateWidget<UMainMenuWidget>(world, m_MenuWidgetClass))
		{
			mainMenuWidget->AddToViewport();
			instanceData.m_WidgetPtr = mainMenuWidget;
			instanceData.m_ActionDelegateHandle = mainMenuWidget->OnActionRequested.AddLambda(
				[weakContext = context.MakeWeakExecutionContext()](const EMainMenuAction action) mutable
				{
					const FStateTreeStrongExecutionContext strongContext = weakContext.MakeStrongExecutionContext();
					if (action == EMainMenuAction::Quit)
					{							
						UKismetSystemLibrary::QuitGame(
							strongContext.GetOwner()->GetWorld(), 
							nullptr, 
							EQuitPreference::Quit, 
							false);							
					}
					else if (action == EMainMenuAction::Continue)
					{	 
						strongContext.FinishTask(EStateTreeFinishTaskType::Succeeded);
					}					
				});
		}
	}
	
	return EStateTreeRunStatus::Running;
}
void FMainMenuUiTask::ExitState(
	FStateTreeExecutionContext& context, 
	const FStateTreeTransitionResult& transitions) const
{
	FInstanceDataType& instanceData = context.GetInstanceData(*this);
	 
	if (instanceData.m_WidgetPtr.IsValid())
	{
		instanceData.m_WidgetPtr->OnActionRequested.Remove(instanceData.m_ActionDelegateHandle);
		instanceData.m_WidgetPtr->RemoveFromParent();
		instanceData.m_WidgetPtr = nullptr;
	}
}
