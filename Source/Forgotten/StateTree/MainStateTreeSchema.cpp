// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.


#include "MainStateTreeSchema.h"
#include "MainStateTreeSubsystem.h"
#include "StateTreeConditionBase.h"
#include "StateTreeEvaluatorBase.h"
#include "StateTreeTaskBase.h"

UMainStateTreeSchema::UMainStateTreeSchema()
	: m_subsystemData(
		TEXT("Subsystem"),
		UMainStateTreeSubsystem::StaticClass(),
		FGuid::NewGuid())
{
}
TConstArrayView<FStateTreeExternalDataDesc> UMainStateTreeSchema::GetContextDataDescs() const
{
	return TConstArrayView(&m_subsystemData, 1);
}
bool UMainStateTreeSchema::IsStructAllowed(const UScriptStruct* inScriptStruct) const
{
    return inScriptStruct->IsChildOf(FStateTreeTaskBase::StaticStruct())
        || inScriptStruct->IsChildOf(FStateTreeEvaluatorBase::StaticStruct())
        || inScriptStruct->IsChildOf(FStateTreeConditionBase::StaticStruct());
}
