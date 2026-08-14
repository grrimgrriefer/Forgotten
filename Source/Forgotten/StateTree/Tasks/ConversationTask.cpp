// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "ConversationTask.h"
#include "StateTreeAsyncExecutionContext.h"
#include "StateTreeExecutionContext.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Forgotten/CustomGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "Forgotten/Character/FirstPersonCharacter.h"
#include "Forgotten/Character/RainNPC.h"
#include "Forgotten/StateTree/MainStateTreeSubsystem.h"
#include "Forgotten/Widgets/ConversationWidget.h"

FConversationTask::FConversationTask()
{
	bShouldCallTick = false;
}
const UScriptStruct* FConversationTask::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}
EStateTreeRunStatus FConversationTask::EnterState(
	FStateTreeExecutionContext& context,
	const FStateTreeTransitionResult& transitions) const
{
	const UWorld* world = context.GetWorld();
	FInstanceDataType& instanceData = context.GetInstanceData(*this);

	check(world);
	APlayerController* playerController = world->GetFirstPlayerController();

	check(playerController);
	AFirstPersonCharacter* playerCharacter = Cast<AFirstPersonCharacter>(playerController->GetPawn());
	ARainNPC* rainNPC = Cast<ARainNPC>(UGameplayStatics::GetActorOfClass(world, ARainNPC::StaticClass()));

	check(playerCharacter);
	check(rainNPC);
	instanceData.m_PlayerCharacter = playerCharacter;
	instanceData.m_TargetNPC = rainNPC;
	playerCharacter->EnterConversationMode(rainNPC);

	ensureMsgf(m_ConversationWidgetClass, TEXT("FConversationTask: m_ConversationWidgetClass is not assigned, check the StateTree."));
	UConversationWidget* conversationWidget = CreateWidget<UConversationWidget>(playerController, m_ConversationWidgetClass);

	check(conversationWidget);
	conversationWidget->AddToViewport();
	instanceData.m_WidgetPtr = conversationWidget;

	FInputModeGameAndUI inputMode;
	inputMode.SetWidgetToFocus(conversationWidget->TakeWidget());
	inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	playerController->SetInputMode(inputMode);
	playerController->SetShowMouseCursor(true);

	instanceData.m_TextSubmittedHandle = conversationWidget->m_OnTextSubmitted.AddLambda(
		[conversationWidget](const FText& textSubmitted)
		{
			ensureMsgf(conversationWidget, TEXT("FConversationTask: conversationWidget is not valid when trying to "
											"add a transcript entry, this should not happen."));
			if (IsValid(conversationWidget))
			{
				conversationWidget->AddTranscriptEntry(
					NSLOCTEXT("Conversation", "You", "You"),
					textSubmitted);
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

			ensureMsgf(stateTreeSubsystem, TEXT("FConversationTask: MainStateTreeSubsystem is not valid when trying "
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

	if (AFirstPersonCharacter* playerCharacter = instanceData.m_PlayerCharacter.Get())
	{
		playerCharacter->ExitConversationMode();

		if (APlayerController* playerController = Cast<APlayerController>(playerCharacter->GetController()))
		{
			const FInputModeGameOnly gameMode;
			playerController->SetInputMode(gameMode);
			playerController->SetShowMouseCursor(false);
		}
	}

	if (UConversationWidget* conversationWidget = instanceData.m_WidgetPtr.Get())
	{
		conversationWidget->m_OnTextSubmitted.Remove(instanceData.m_TextSubmittedHandle);
		conversationWidget->m_OnConversationEnded.Remove(instanceData.m_EndedHandle);
		conversationWidget->RemoveFromParent();
	}

	instanceData.m_WidgetPtr = nullptr;
	instanceData.m_PlayerCharacter = nullptr;
	instanceData.m_TargetNPC = nullptr;
}
