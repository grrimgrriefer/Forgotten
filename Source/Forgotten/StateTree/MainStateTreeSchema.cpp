// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "MainStateTreeSchema.h"
#include "Forgotten/SubSystems/MainStateTreeSubsystem.h"
#include "StateTreeConditionBase.h"
#include "StateTreeEvaluatorBase.h"
#include "StateTreeTaskBase.h"
#include "Forgotten/Character/FirstPersonCharacter.h"
#include "Forgotten/Character/ConversableNPC.h"

const FName UMainStateTreeSchema::m_PlayerBindingName = FName(TEXT("Player"));
const FName UMainStateTreeSchema::m_ConversableNpcBindingName = FName(TEXT("ConversableNpc"));
const FName UMainStateTreeSchema::m_SubsystemBindingName = FName(TEXT("Subsystem"));

UMainStateTreeSchema::UMainStateTreeSchema()
	: m_subsystemData(m_SubsystemBindingName, UMainStateTreeSubsystem::StaticClass(), FGuid::NewGuid())
	, m_playerData(m_PlayerBindingName, AFirstPersonCharacter::StaticClass(), FGuid::NewGuid())
	, m_conversableNpcData(m_ConversableNpcBindingName, AConversableNPC::StaticClass(), FGuid::NewGuid())
{
	m_playerData.Requirement = EStateTreeExternalDataRequirement::Optional;
	m_conversableNpcData.Requirement = EStateTreeExternalDataRequirement::Optional;

	m_contextDescs = { m_subsystemData, m_playerData, m_conversableNpcData };
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
		return itemClass->IsChildOf(ACharacter::StaticClass())
			|| itemClass->IsChildOf(UGameInstanceSubsystem::StaticClass());
	}
	return false;
}
