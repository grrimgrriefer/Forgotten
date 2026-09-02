// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "PlayerStateTreeSchema.h"
#include "StateTreeConditionBase.h"
#include "StateTreeEvaluatorBase.h"
#include "StateTreeTaskBase.h"
#include "Forgotten/Character/ConversableNPC.h"
#include "Forgotten/Character/FirstPersonCharacter.h"

const FName UPlayerStateTreeSchema::PLAYER_BINDING_NAME = TEXT("Player");
const FName UPlayerStateTreeSchema::CONVERSATION_NPC_BINDING_NAME = TEXT("ConversableNpc");

UPlayerStateTreeSchema::UPlayerStateTreeSchema() : m_playerData(PLAYER_BINDING_NAME,
																AFirstPersonCharacter::StaticClass(),
																FGuid::NewDeterministicGuid(PLAYER_BINDING_NAME.ToString())),
													m_conversableNpcData(CONVERSATION_NPC_BINDING_NAME,
																		AConversableNPC::StaticClass(),
																		FGuid::NewDeterministicGuid(CONVERSATION_NPC_BINDING_NAME.ToString()))
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
