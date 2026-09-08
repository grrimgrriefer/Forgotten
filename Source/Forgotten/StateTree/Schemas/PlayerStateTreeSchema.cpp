// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "PlayerStateTreeSchema.h"
#include "StateTreeConditionBase.h"
#include "StateTreeEvaluatorBase.h"
#include "StateTreeTaskBase.h"
#include "Forgotten/Character/ConversableNPC.h"
#include "Forgotten/Character/FirstPersonCharacter.h"
#include "SubSystems/CharacterSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"

const FName UPlayerStateTreeSchema::PLAYER_BINDING_NAME = TEXT("PLAYER_BINDING");
const FName UPlayerStateTreeSchema::CONVERSATION_NPC_BINDING_NAME = TEXT("CONVERSATION_NPC_BINDING");
const FName UPlayerStateTreeSchema::CHARACTER_SUBSYSTEM_BINDING_NAME = TEXT("CHARACTER_SUBSYSTEM_BINDING");

UPlayerStateTreeSchema::UPlayerStateTreeSchema() : m_playerData(PLAYER_BINDING_NAME,
																AFirstPersonCharacter::StaticClass(),
																FGuid::NewDeterministicGuid(PLAYER_BINDING_NAME.ToString())),
													m_conversableNpcData(CONVERSATION_NPC_BINDING_NAME,
																		AConversableNPC::StaticClass(),
																		FGuid::NewDeterministicGuid(CONVERSATION_NPC_BINDING_NAME.ToString())),
													m_characterSubsystemData(CHARACTER_SUBSYSTEM_BINDING_NAME,
																		UCharacterSubsystem::StaticClass(),
																		FGuid::NewDeterministicGuid(CHARACTER_SUBSYSTEM_BINDING_NAME.ToString()))
{
	m_playerData.Requirement = EStateTreeExternalDataRequirement::Required;
	m_conversableNpcData.Requirement = EStateTreeExternalDataRequirement::Optional;
	m_characterSubsystemData.Requirement = EStateTreeExternalDataRequirement::Required;

	m_contextDescs = { m_playerData, m_conversableNpcData, m_characterSubsystemData };
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
		return itemClass->IsChildOf(ACharacter::StaticClass())
			|| itemClass->IsChildOf(UCharacterSubsystem::StaticClass());
	}
	return false;
}
