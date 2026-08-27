// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "IsLoadedLevelCondition.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeNodeDescriptionHelpers.h"
#include "Engine/World.h"
#include "Forgotten/Utils/AssertMacros.h"
#include "UObject/Package.h"

const UScriptStruct* FIsLoadedLevelCondition::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}
bool FIsLoadedLevelCondition::TestCondition(FStateTreeExecutionContext& context) const
{
	const UWorld* world = context.GetWorld();
	const FInstanceDataType& instanceData = context.GetInstanceData(*this);

	ASSERT_CHECK_RETURN(world, false);
	ASSERT_CHECK_RETURN(!instanceData.m_LevelToCheck.IsNull(), false, TEXT("FIsLoadedLevelCondition: m_LevelToCheck is not assigned, check the StateTree."));
	if (instanceData.m_LevelToCheck.IsNull())
	{
		return false;
	}

	const FName targetPackageName = instanceData.m_LevelToCheck.GetLongPackageFName();
	FName currentPackageName;
#if WITH_EDITOR
	FString currentPackageString = world->GetOutermost()->GetName();
	currentPackageString = UWorld::RemovePIEPrefix(currentPackageString);
	currentPackageName = FName(*currentPackageString);
#else
	currentPackageName = world->GetOutermost()->GetFName();
#endif

	const bool isMatch = (currentPackageName == targetPackageName);
	return instanceData.m_Invert ? !isMatch : isMatch;
}

#if WITH_EDITOR
FText FIsLoadedLevelCondition::GetDescription(
	const FGuid& id,
	FStateTreeDataView instanceDataView,
	const IStateTreeBindingLookup& bindingLookup,
	EStateTreeNodeFormatting formatting) const
{
	const FInstanceDataType* instanceData = instanceDataView.GetPtr<FInstanceDataType>();
	const FString levelName = (instanceData && !instanceData->m_LevelToCheck.IsNull())
								? instanceData->m_LevelToCheck.GetAssetName()
								: TEXT("None");

	const FText invertPrefix = (instanceData && instanceData->m_Invert)
									? NSLOCTEXT("StateTree", "InvertPrefix", "NOT ")
									: FText::GetEmpty();
	return FText::Format(
		NSLOCTEXT("StateTree", "IsLoadedLevelCondition_CombinedDesc", "{0} Is Loaded Level ({1})"),
		invertPrefix,
		FText::FromString(levelName));
}
#endif
