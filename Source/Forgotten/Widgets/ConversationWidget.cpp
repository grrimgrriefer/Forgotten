// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "ConversationWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Forgotten/Utils/AssertMacros.h"
#include "Framework/Application/SlateApplication.h"
#include "TimerManager.h"

void UConversationWidget::AddTranscriptEntry(const FText& speakerName, const FText& messageText)
{
	const FText formattedText = FText::Format(NSLOCTEXT("Conversation", "TranscriptFormat", "{0}: {1}"), speakerName, messageText);

	UTextBlock* newEntry = WidgetTree->ConstructWidget<UTextBlock>();

	ASSERT_CHECK(newEntry);
	newEntry->SetText(formattedText);
	newEntry->SetAutoWrapText(true);

	ASSERT_CHECK(m_transcriptScrollBox);
	m_transcriptScrollBox->AddChild(newEntry);
	m_transcriptScrollBox->ScrollToEnd();

	SetTranscriptVisibility(true);
}
void UConversationWidget::SetTranscriptVisibility(const bool isVisible)
{
	if (isVisible)
	{
		SetVisibility(m_visibleValue);
		ASSERT_CHECK(m_fadeInAnimation);
		PlayAnimation(m_fadeInAnimation);

		if (IsInputFocused())
		{
			ClearFadeTimer();
		}
		else
		{
			StartFadeTimer();
		}
	}
	else
	{
		ClearFadeTimer();
		ASSERT_CHECK(m_fadeOutAnimation);
		PlayAnimation(m_fadeOutAnimation);
	}
}
void UConversationWidget::FocusInput()
{
	SetOutOfRangeFeedbackVisibility(false);

	SetVisibility(m_visibleValue);

	ClearFadeTimer();
	ASSERT_CHECK(m_fadeInAnimation);
	PlayAnimation(m_fadeInAnimation);

	ASSERT_CHECK(m_inputTextBox);
	m_inputTextBox->SetRenderOpacity(1.0f);
	m_inputTextBox->SetKeyboardFocus();
}
void UConversationWidget::UnfocusInput()
{
	ClearInputFieldAndSetLowOpacity();

	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().ClearUserFocus(0);
	}
	m_OnChatFocusLost.Broadcast();

	StartFadeTimer();
}
bool UConversationWidget::IsInputFocused() const
{
	return m_inputTextBox && m_inputTextBox->HasKeyboardFocus();
}
void UConversationWidget::SetOutOfRangeFeedbackVisibility(bool isVisible)
{
	if (isVisible)
	{
		SetTranscriptVisibility(true);
		ClearInputFieldAndSetLowOpacity();

		ASSERT_CHECK(m_outOfRangeTextBlock);
		m_outOfRangeTextBlock->SetText(m_outOfRangeMessage);
		m_outOfRangeTextBlock->SetVisibility(m_visibleValue);

		StartFadeTimer();
	}
	else
	{
		ASSERT_CHECK(m_outOfRangeTextBlock);
		m_outOfRangeTextBlock->SetVisibility(m_hiddenValue);
	}
}
void UConversationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ASSERT_CHECK(m_transcriptScrollBox);
	ASSERT_CHECK(m_inputTextBox);

	m_outOfRangeMessage = NSLOCTEXT("Conversation", "OutOfRange", "You are out of range to talk.");

	m_inputTextBox->SetRenderOpacity(m_unfocusedInputOpacity);
	m_inputTextBox->OnTextCommitted.RemoveDynamic(this, &UConversationWidget::OnInputTextCommitted);
	m_inputTextBox->OnTextCommitted.AddDynamic(this, &UConversationWidget::OnInputTextCommitted);

	SetOutOfRangeFeedbackVisibility(false);
	SetVisibility(m_hiddenValue);
}
void UConversationWidget::NativeDestruct()
{
	ClearFadeTimer();

	if (m_inputTextBox)
	{
		m_inputTextBox->OnTextCommitted.RemoveAll(this);
	}

	Super::NativeDestruct();
}
void UConversationWidget::OnAnimationFinished_Implementation(const UWidgetAnimation* animation)
{
	Super::OnAnimationFinished_Implementation(animation);
}
void UConversationWidget::StartFadeTimer()
{
	const UWorld* world = GetWorld();
	if (!IsValid(world))
	{
		return;
	}

	world->GetTimerManager().SetTimer(
		m_fadeTimerHandle,
		this,
		&UConversationWidget::OnFadeTimerExpired,
		m_fadeDelay,
		false
	);
}
void UConversationWidget::ClearFadeTimer()
{
	const UWorld* world = GetWorld();
	if (IsValid(world))
	{
		world->GetTimerManager().ClearTimer(m_fadeTimerHandle);
	}
}
void UConversationWidget::OnFadeTimerExpired()
{
	SetTranscriptVisibility(false);
}
void UConversationWidget::SubmitCurrentInputText(const FText& text)
{
	ASSERT_CHECK(m_inputTextBox);
	if (!text.IsEmptyOrWhitespace())
	{
		m_OnTextSubmitted.Broadcast(text);
	}
	m_inputTextBox->SetText(FText::GetEmpty());
	UnfocusInput();
}
void UConversationWidget::ClearInputFieldAndSetLowOpacity() const
{
	ASSERT_CHECK(m_inputTextBox);
	m_inputTextBox->SetText(FText::GetEmpty());
	m_inputTextBox->SetRenderOpacity(m_unfocusedInputOpacity);
}
void UConversationWidget::OnInputTextCommitted(const FText& text, ETextCommit::Type commitMethod)
{
	if (commitMethod == ETextCommit::OnEnter)
	{
		SubmitCurrentInputText(text);
	}
	else if (commitMethod == ETextCommit::OnCleared || commitMethod == ETextCommit::OnUserMovedFocus)
	{
		UnfocusInput();
	}
}
