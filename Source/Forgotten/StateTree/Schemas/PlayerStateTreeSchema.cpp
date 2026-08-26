// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "PlayerStateTreeSchema.h"
#include "StateTreeConditionBase.h"
#include "StateTreeEvaluatorBase.h"
#include "StateTreeTaskBase.h"
#include "Forgotten/Character/ConversableNPC.h"
#include "Forgotten/Character/FirstPersonCharacter.h"

const FName UPlayerStateTreeSchema::m_PlayerBindingName = FName(TEXT("Player"));
const FName UPlayerStateTreeSchema::m_ConversableNpcBindingName = FName(TEXT("ConversableNpc"));

UPlayerStateTreeSchema::UPlayerStateTreeSchema() : m_playerData(m_PlayerBindingName,
																AFirstPersonCharacter::StaticClass(),
																FGuid::NewDeterministicGuid(m_PlayerBindingName.ToString())),
													m_conversableNpcData(m_ConversableNpcBindingName,
																		AConversableNPC::StaticClass(),
																		FGuid::NewDeterministicGuid(m_ConversableNpcBindingName.ToString()))
{
	m_playerData.Requirement = EStateTreeExternalDataRequirement::Required;
	m_conversableNpcData.Requirement = EStateTreeExternalDataRequirement::Optional;

	m_contextDescs = { m_playerData, m_conversableNpcData };
}
TConstArrayView<FStateTreeExternalDataDesc> UPlayerStateTreeSchema::GetContextDataDescs() const
{
	return m_contextDescs;
}
bool UPlayerStateTreeSchema::IsStructAllowed(const UScriptStruct* inScriptStruct) const
{
	return inScriptStruct->IsChildOf(FStateTreeTaskBase::StaticStruct())
		|| inScriptStruct->IsChildOf(FStateTreeEvaluatorBase::StaticStruct())
		|| inScriptStruct->IsChildOf(FStateTreeConditionBase::StaticStruct());
}
bool UPlayerStateTreeSchema::IsExternalItemAllowed(const UStruct& inStruct) const
{
	if (const UClass* itemClass = Cast<const UClass>(&inStruct))
	{
		return itemClass->IsChildOf(ACharacter::StaticClass());
	}
	return false;
}
