// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "MainStateTreeSchema.h"
#include "Forgotten/SubSystems/MainStateTreeSubsystem.h"
#include "StateTreeConditionBase.h"
#include "StateTreeEvaluatorBase.h"
#include "StateTreeTaskBase.h"

const FName UMainStateTreeSchema::m_SubsystemBindingName = TEXT("Subsystem");

UMainStateTreeSchema::UMainStateTreeSchema() : m_subsystemData(m_SubsystemBindingName,
																UMainStateTreeSubsystem::StaticClass(),
																FGuid::NewDeterministicGuid(m_SubsystemBindingName.ToString()))
{
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
