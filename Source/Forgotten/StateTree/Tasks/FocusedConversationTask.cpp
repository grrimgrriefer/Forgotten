// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "FocusedConversationTask.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Forgotten/CustomGameplayTags.h"
#include "Forgotten/Character/FirstPersonCharacter.h"
#include "Forgotten/Character/ConversableNPC.h"
#include "Forgotten/SubSystems/ConversationSubsystem.h"
#include "Forgotten/SubSystems/MainStateTreeSubsystem.h"
#include "Forgotten/Utils/AssertMacros.h"

FFocusedConversationTask::FFocusedConversationTask()
{
	bShouldCallTick = false;
}
const UScriptStruct* FFocusedConversationTask::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}
bool FFocusedConversationTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(m_ConversableNpcHandle);
	Linker.LinkExternalData(m_PlayerCharacterHandle);
	return true;
}
EStateTreeRunStatus FFocusedConversationTask::EnterState(
	FStateTreeExecutionContext& context,
	const FStateTreeTransitionResult& transitions) const
{
	AFirstPersonCharacter* playerCharacter = context.GetExternalDataPtr(m_PlayerCharacterHandle);
	AConversableNPC* conversableNpc = context.GetExternalDataPtr(m_ConversableNpcHandle);

	ASSERT_CHECK_RETURN(playerCharacter, EStateTreeRunStatus::Failed);
	ASSERT_CHECK_RETURN(conversableNpc, EStateTreeRunStatus::Failed);

	const UWorld* world = context.GetWorld();
	UConversationSubsystem* conversationSubsystem = world->GetSubsystem<UConversationSubsystem>();
	conversationSubsystem->SetCurrentConversableNpc(conversableNpc);
	playerCharacter->EnterFocusedConvoMode(conversableNpc);

	return EStateTreeRunStatus::Running;
}
void FFocusedConversationTask::ExitState(
	FStateTreeExecutionContext& context,
	const FStateTreeTransitionResult& transitions) const
{
	if (AFirstPersonCharacter* playerCharacter = context.GetExternalDataPtr(m_PlayerCharacterHandle))
	{
		playerCharacter->ExitFocusedConvoMode();
	}

	if (AConversableNPC* conversableNpc = context.GetExternalDataPtr(m_ConversableNpcHandle))
	{
		const UWorld* world = context.GetWorld();
		const UGameInstance* gameInstance = world->GetGameInstance();
		if (IsValid(world) && IsValid(gameInstance))
		{
			UMainStateTreeSubsystem* stateTreeSubsystem = gameInstance->GetSubsystem<UMainStateTreeSubsystem>();
			ASSERT_CHECK(stateTreeSubsystem);
			stateTreeSubsystem->TryUnbindContextData(conversableNpc);
		}
	}
}
