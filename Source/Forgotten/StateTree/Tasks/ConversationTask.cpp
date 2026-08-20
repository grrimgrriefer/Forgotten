// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "ConversationTask.h"
#include "StateTreeAsyncExecutionContext.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Forgotten/CustomGameplayTags.h"
#include "Forgotten/Character/FirstPersonCharacter.h"
#include "Forgotten/Character/ConversableNPC.h"
#include "Forgotten/StateTree/MainStateTreeSubsystem.h"
#include "Forgotten/Utils/AssertMacros.h"
#include "Forgotten/Widgets/ConversationWidget.h"

FConversationTask::FConversationTask()
{
	bShouldCallTick = false;
}
const UScriptStruct* FConversationTask::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}
bool FConversationTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(m_ConversableNpcHandle);
	Linker.LinkExternalData(m_PlayerCharacterHandle);
	return true;
}
EStateTreeRunStatus FConversationTask::EnterState(
	FStateTreeExecutionContext& context,
	const FStateTreeTransitionResult& transitions) const
{
	AFirstPersonCharacter* playerCharacter = context.GetExternalDataPtr(m_PlayerCharacterHandle);
	AConversableNPC* conversableNpc = context.GetExternalDataPtr(m_ConversableNpcHandle);

	ASSERT_CHECK_RETURN(playerCharacter, EStateTreeRunStatus::Failed);
	ASSERT_CHECK_RETURN(conversableNpc, EStateTreeRunStatus::Failed);
	playerCharacter->EnterConversationMode(conversableNpc);
	conversableNpc->StartConversation(playerCharacter);

	const UWorld* world = context.GetWorld();
	ASSERT_CHECK_RETURN(world, EStateTreeRunStatus::Failed);
	APlayerController* playerController = world->GetFirstPlayerController();

	ASSERT_CHECK_RETURN(m_ConversationWidgetClass, EStateTreeRunStatus::Failed,
		TEXT("FConversationTask: m_ConversationWidgetClass is not assigned, check the StateTree."));
	UConversationWidget* conversationWidget = CreateWidget<UConversationWidget>(playerController, m_ConversationWidgetClass);

	FInstanceDataType& instanceData = context.GetInstanceData(*this);
	ASSERT_CHECK_RETURN(conversationWidget, EStateTreeRunStatus::Failed);
	conversationWidget->AddToViewport();
	instanceData.m_WidgetPtr = conversationWidget;

	FInputModeGameAndUI inputMode;
	inputMode.SetWidgetToFocus(conversationWidget->TakeWidget());
	inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	playerController->SetInputMode(inputMode);
	playerController->SetShowMouseCursor(true);

	instanceData.m_TextSubmittedHandle = conversationWidget->m_OnTextSubmitted.AddLambda(
		[weakWidget = TWeakObjectPtr(conversationWidget)](const FText& textSubmitted)
		{
			if (const UConversationWidget* strongWidget = weakWidget.Get())
			{
				strongWidget->AddTranscriptEntry(NSLOCTEXT("Conversation", "You", "You"), textSubmitted);
			}
		});

	instanceData.m_EndedHandle = conversationWidget->m_OnConversationEnded.AddLambda(
		[weakContext = context.MakeWeakExecutionContext()]() mutable
		{
			const FStateTreeStrongExecutionContext strongContext = weakContext.MakeStrongExecutionContext();
			if (!strongContext.IsValid())
			{
				return;
			}

			const UObject* owner = strongContext.GetOwner().Get();
			const UWorld* worldContext = IsValid(owner) ? owner->GetWorld() : nullptr;
			UMainStateTreeSubsystem* stateTreeSubsystem = IsValid(worldContext)
				? worldContext->GetGameInstance()->GetSubsystem<UMainStateTreeSubsystem>()
				: nullptr;

			ASSERT_CHECK(stateTreeSubsystem, TEXT("FConversationTask: MainStateTreeSubsystem is not valid when trying "
											"to end the conversation, this should not happen."));
			if (IsValid(stateTreeSubsystem))
			{
				stateTreeSubsystem->TrySendFlowEvent(TAG_State_Conversation_End);
			}
		});

	return EStateTreeRunStatus::Running;
}
void FConversationTask::ExitState(
	FStateTreeExecutionContext& context,
	const FStateTreeTransitionResult& transitions) const
{
	FInstanceDataType& instanceData = context.GetInstanceData(*this);

	if (AFirstPersonCharacter* playerCharacter = context.GetExternalDataPtr(m_PlayerCharacterHandle))
	{
		playerCharacter->ExitConversationMode();

		if (APlayerController* playerController = Cast<APlayerController>(playerCharacter->GetController()))
		{
			const FInputModeGameOnly gameMode;
			playerController->SetInputMode(gameMode);
			playerController->SetShowMouseCursor(false);
		}
	}

	if (AConversableNPC* playerCharacter = context.GetExternalDataPtr(m_ConversableNpcHandle))
	{
		playerCharacter->EndConversation();
	}

	if (UConversationWidget* conversationWidget = instanceData.m_WidgetPtr.Get())
	{
		conversationWidget->m_OnTextSubmitted.Remove(instanceData.m_TextSubmittedHandle);
		conversationWidget->m_OnConversationEnded.Remove(instanceData.m_EndedHandle);
		conversationWidget->RemoveFromParent();
	}
	instanceData.m_WidgetPtr = nullptr;
}
