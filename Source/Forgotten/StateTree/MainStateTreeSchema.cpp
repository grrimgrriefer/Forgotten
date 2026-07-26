// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.


#include "MainStateTreeSchema.h"
#include "MainStateTreeSubsystem.h"
#include "StateTreeConditionBase.h"
#include "StateTreeEvaluatorBase.h"
#include "StateTreeTaskBase.h"

UMainStateTreeSchema::UMainStateTreeSchema()
	: m_SubsystemData(
		TEXT("Subsystem"), 
		UMainStateTreeSubsystem::StaticClass(), 
		FGuid::NewGuid())
{
}
TConstArrayView<FStateTreeExternalDataDesc> UMainStateTreeSchema::GetContextDataDescs() const
{
	return TConstArrayView<FStateTreeExternalDataDesc>(&m_SubsystemData, 1);
}
bool UMainStateTreeSchema::IsStructAllowed(const UScriptStruct* InScriptStruct) const
{
    return InScriptStruct->IsChildOf(FStateTreeTaskBase::StaticStruct())
        || InScriptStruct->IsChildOf(FStateTreeEvaluatorBase::StaticStruct())
        || InScriptStruct->IsChildOf(FStateTreeConditionBase::StaticStruct());
}
