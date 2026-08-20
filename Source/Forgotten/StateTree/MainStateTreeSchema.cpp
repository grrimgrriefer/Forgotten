// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "MainStateTreeSchema.h"
#include "MainStateTreeSubsystem.h"
#include "StateTreeConditionBase.h"
#include "StateTreeEvaluatorBase.h"
#include "StateTreeTaskBase.h"
#include "Forgotten/Character/FirstPersonCharacter.h"

const FName UMainStateTreeSchema::m_PlayerBindingName = FName(TEXT("Player"));
const FName UMainStateTreeSchema::m_ConversationNpcBindingName = FName(TEXT("ConversationNpc"));
const FName UMainStateTreeSchema::m_SubsystemBindingName = FName(TEXT("Subsystem"));

UMainStateTreeSchema::UMainStateTreeSchema()
	: m_subsystemData(m_SubsystemBindingName, UMainStateTreeSubsystem::StaticClass(), FGuid::NewGuid())
	, m_playerData(m_PlayerBindingName, AFirstPersonCharacter::StaticClass(), FGuid::NewGuid())
	, m_conversationNpcData(m_ConversationNpcBindingName, ACharacter::StaticClass(), FGuid::NewGuid())
{
	m_playerData.Requirement = EStateTreeExternalDataRequirement::Optional;
	m_conversationNpcData.Requirement = EStateTreeExternalDataRequirement::Optional;

	m_contextDescs = { m_subsystemData, m_playerData, m_conversationNpcData };
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
		return itemClass->IsChildOf(AActor::StaticClass())
			|| itemClass->IsChildOf(UGameInstanceSubsystem::StaticClass());
	}
	return false;
}
