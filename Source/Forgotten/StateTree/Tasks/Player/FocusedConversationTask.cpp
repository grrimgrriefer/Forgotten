// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "FocusedConversationTask.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Engine/World.h"
#include "Forgotten/Character/FirstPersonCharacter.h"
#include "Forgotten/Character/ConversableNPC.h"
#include "Forgotten/SubSystems/ConversationSubsystem.h"
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
	Linker.LinkExternalData(m_PlayerCharacterHandle);
	Linker.LinkExternalData(m_ConversableNpcHandle);
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

	ASSERT_CHECK_RETURN(conversationSubsystem, EStateTreeRunStatus::Failed);
	conversationSubsystem->SetCurrentConversableNpc(conversableNpc);
	playerCharacter->EnterFocusedConvoMode(conversableNpc);

	return EStateTreeRunStatus::Running;
}
void FFocusedConversationTask::ExitState(
	FStateTreeExecutionContext& context,
	const FStateTreeTransitionResult& transitions) const
{
	AFirstPersonCharacter* playerCharacter = context.GetExternalDataPtr(m_PlayerCharacterHandle);
	if (playerCharacter)
	{
		playerCharacter->ExitFocusedConvoMode();
	}

	AConversableNPC* conversableNpc = context.GetExternalDataPtr(m_ConversableNpcHandle);
	if (conversableNpc && playerCharacter)
	{
		playerCharacter->TryUnbindContextData(conversableNpc);
	}
}
