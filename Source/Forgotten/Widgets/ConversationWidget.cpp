// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "ConversationWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Forgotten/Utils/AssertMacros.h"

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

	m_inputTextBox->OnTextCommitted.AddDynamic(this, &UConversationWidget::OnInputTextCommitted);
}
void UConversationWidget::NativeDestruct()
{
	m_inputTextBox->OnTextCommitted.RemoveAll(this);

	Super::NativeDestruct();
}
void UConversationWidget::SubmitCurrentInputText()
{
	ASSERT_CHECK(m_inputTextBox);
	const FText textToSubmit = m_inputTextBox->GetText();
	if (!textToSubmit.IsEmptyOrWhitespace())
	{
		m_inputTextBox->SetText(FText::GetEmpty());
		m_OnTextSubmitted.Broadcast(textToSubmit);
	}
	UnfocusInput();
}
void UConversationWidget::OnSendClicked()
{
	SubmitCurrentInputText();
}
void UConversationWidget::OnInputTextCommitted(const FText& text, ETextCommit::Type commitMethod)
{
	if (commitMethod == ETextCommit::OnEnter)
	{
		SubmitCurrentInputText();
	}
	else if (commitMethod == ETextCommit::OnCleared)
	{
		UnfocusInput();
	}
}
