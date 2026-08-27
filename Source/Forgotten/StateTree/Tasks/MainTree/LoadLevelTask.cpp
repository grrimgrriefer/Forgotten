// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "LoadLevelTask.h"

#include "StateTreeExecutionContext.h"
#include "Engine/World.h"
#include "Forgotten/Utils/AssertMacros.h"
#include "Kismet/GameplayStatics.h"
#include "VisualLogger/VisualLogger.h"

FLoadLevelTask::FLoadLevelTask()
{
	bShouldCallTick = false;
}
const UScriptStruct* FLoadLevelTask::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}
EStateTreeRunStatus FLoadLevelTask::EnterState(
	FStateTreeExecutionContext& context,
	const FStateTreeTransitionResult& transitions) const
{
	const UWorld* world = context.GetWorld();
	const FInstanceDataType& instanceData = context.GetInstanceData(*this);

	ASSERT_CHECK_RETURN(world, EStateTreeRunStatus::Failed);
	ASSERT_CHECK_RETURN(!instanceData.m_LevelToLoad.IsNull(),
						EStateTreeRunStatus::Failed,
						TEXT("FLoadLevelTask: m_LevelToLoad is not assigned, check the StateTree."));
	UGameplayStatics::OpenLevelBySoftObjectPtr(world, instanceData.m_LevelToLoad);

	return EStateTreeRunStatus::Running;
}
