// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "MainStateTreeSchema.h"
#include "Forgotten/SubSystems/MainStateTreeSubsystem.h"
#include "StateTreeConditionBase.h"
#include "StateTreeEvaluatorBase.h"
#include "StateTreeTaskBase.h"

const FName UMainStateTreeSchema::MAIN_STATE_TREE_SUBSYSTEM_BINDING_NAME = TEXT("MAIN_STATE_TREE_SUBSYSTEM_BINDING");

UMainStateTreeSchema::UMainStateTreeSchema() : m_subsystemData(MAIN_STATE_TREE_SUBSYSTEM_BINDING_NAME,
																UMainStateTreeSubsystem::StaticClass(),
																FGuid::NewDeterministicGuid(MAIN_STATE_TREE_SUBSYSTEM_BINDING_NAME.ToString()))
{
	m_subsystemData.Requirement = EStateTreeExternalDataRequirement::Required;
	m_contextDescs = { m_subsystemData };
}
TConstArrayView<FStateTreeExternalDataDesc> UMainStateTreeSchema::GetContextDataDescs() const
{
	return m_contextDescs;
}
bool UMainStateTreeSchema::IsStructAllowed(const UScriptStruct* inScriptStruct) const
{
	return inScriptStruct->IsChildOf(FStateTreeTaskBase::StaticStruct())
		|| inScriptStruct->IsChildOf(FStateTreeEvaluatorBase::StaticStruct())
		|| inScriptStruct->IsChildOf(FStateTreeConditionBase::StaticStruct());
}
bool UMainStateTreeSchema::IsExternalItemAllowed(const UStruct& inStruct) const
{
	if (const UClass* itemClass = Cast<const UClass>(&inStruct))
	{
		return itemClass->IsChildOf(UGameInstanceSubsystem::StaticClass());
	}
	return false;
}
