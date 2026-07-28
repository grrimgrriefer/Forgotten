// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "IsLoadedLevelCondition.h"
#include "StateTreeExecutionContext.h"
#include "Engine/World.h"
#include "Misc/PackageName.h"
#include "UObject/Package.h"

const UScriptStruct* FIsLoadedLevelCondition::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}
bool FIsLoadedLevelCondition::TestCondition(FStateTreeExecutionContext& context) const
{
	const UWorld* world = context.GetWorld();
	if (!world)
	{
		return false;
	}

	const FInstanceDataType& instanceData = context.GetInstanceData(*this);
	if (instanceData.m_LevelToCheck.IsNull())
	{
		return false;
	}

	const FString targetPackageName = FPackageName::ObjectPathToPackageName(instanceData.m_LevelToCheck.ToString());
	FString currentPackageName = world->GetOutermost()->GetName();
#if WITH_EDITOR
	currentPackageName = UWorld::RemovePIEPrefix(currentPackageName);
#endif

	return currentPackageName == targetPackageName;
}
