// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "ConversationWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Forgotten/Utils/AssertMacros.h"
#include "Framework/Application/SlateApplication.h"

void UConversationWidget::AddTranscriptEntry(const FText& speakerName, const FText& messageText) const
{
	const FText formattedText = FText::Format(
		NSLOCTEXT("Conversation", "TranscriptFormat", "{0}: {1}"),
		speakerName,
		messageText);

	UTextBlock* newEntry = WidgetTree->ConstructWidget<UTextBlock>();

	ASSERT_CHECK(newEntry);
	newEntry->SetText(formattedText);
	newEntry->SetAutoWrapText(true);

	ASSERT_CHECK(m_transcriptScrollBox);
	m_transcriptScrollBox->AddChild(newEntry);
	m_transcriptScrollBox->ScrollToEnd();
}
void UConversationWidget::ToggleTranscriptVisibility()
{
	const ESlateVisibility currentVis = GetVisibility();
	const ESlateVisibility newVis = (currentVis == ESlateVisibility::Collapsed || currentVis == ESlateVisibility::Hidden)
										? ESlateVisibility::SelfHitTestInvisible
										: ESlateVisibility::Collapsed;

	SetVisibility(newVis);
}
void UConversationWidget::FocusInput()
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	ASSERT_CHECK(m_inputTextBox);
	m_inputTextBox->SetKeyboardFocus();
}
void UConversationWidget::UnfocusInput()
{
	ASSERT_CHECK(m_inputTextBox);
	m_inputTextBox->SetText(FText::GetEmpty());
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().ClearUserFocus(0);
	}
	m_OnChatFocusLost.Broadcast();
}
bool UConversationWidget::IsInputFocused() const
{
	return m_inputTextBox && m_inputTextBox->HasKeyboardFocus();
}
void UConversationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ASSERT_CHECK(m_transcriptScrollBox);
	ASSERT_CHECK(m_inputTextBox);

	m_inputTextBox->OnTextCommitted.RemoveDynamic(this, &UConversationWidget::OnInputTextCommitted);
	m_inputTextBox->OnTextCommitted.AddDynamic(this, &UConversationWidget::OnInputTextCommitted);
}
void UConversationWidget::NativeDestruct()
{
	if (m_inputTextBox)
	{
		m_inputTextBox->OnTextCommitted.RemoveAll(this);
	}

	Super::NativeDestruct();
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
