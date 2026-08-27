// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "PlaceholderTask.h"

#include "StateTreeExecutionContext.h"
#include "Engine/World.h"

FPlaceholderTask::FPlaceholderTask()
{
	bShouldCallTick = false;
}
const UScriptStruct* FPlaceholderTask::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}
EStateTreeRunStatus FPlaceholderTask::EnterState(FStateTreeExecutionContext& context, const FStateTreeTransitionResult& transitions) const
{
	return EStateTreeRunStatus::Running;
}
