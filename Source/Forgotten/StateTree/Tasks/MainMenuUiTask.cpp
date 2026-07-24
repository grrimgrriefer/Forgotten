// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "MainMenuUiTask.h"
#include "Engine/World.h"
#include "StateTreeExecutionContext.h"
#include "Blueprint/UserWidget.h"
#include "Forgotten/Widgets/MainMenuWidget.h"

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
	 
	if (world && world->IsGameWorld())
	{
		if (UMainMenuWidget* mainMenuWidget = CreateWidget<UMainMenuWidget>(world, m_MenuWidgetClass))
		{
			mainMenuWidget->AddToViewport();
			instanceData.m_WidgetPtr = mainMenuWidget;
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
		instanceData.m_WidgetPtr->RemoveFromParent();
		instanceData.m_WidgetPtr = nullptr;
	}
}
