// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "IsLoadedLevelCondition.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeNodeDescriptionHelpers.h"
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

	const bool isMatch = (currentPackageName == targetPackageName);
	return instanceData.m_invert ? !isMatch : isMatch;
}
FText FIsLoadedLevelCondition::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	const FInstanceDataType* instanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	const FString levelName = (instanceData && !instanceData->m_LevelToCheck.IsNull()) ? instanceData->m_LevelToCheck.GetAssetName() : TEXT("None");

	const FText invertPrefix = instanceData->m_invert ? NSLOCTEXT("StateTree", "InvertPrefix", "NOT ") : FText::GetEmpty();
	return FText::Format(
		NSLOCTEXT("StateTree", "IsLoadedLevelCondition_CombinedDesc", "{0} Is Loaded Level ({1})"),
		invertPrefix,
		FText::FromString(levelName)
	);
}
