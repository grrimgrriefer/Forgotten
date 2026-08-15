// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "ConversationWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

void UConversationWidget::AddTranscriptEntry(const FText& speakerName, const FText& messageText) const
{
	const FText formattedText = FText::Format(
		NSLOCTEXT("Conversation", "TranscriptFormat", "{0}: {1}"),
		speakerName,
		messageText);

	check(m_transcriptScrollBox);
	UTextBlock* newEntry = NewObject<UTextBlock>(m_transcriptScrollBox);

	check(newEntry);
	newEntry->SetText(formattedText);
	newEntry->SetAutoWrapText(true);

	m_transcriptScrollBox->AddChild(newEntry);
	m_transcriptScrollBox->ScrollToEnd();
}
void UConversationWidget::NativeConstruct()
{
    Super::NativeConstruct();

    check(m_transcriptScrollBox);
    check(m_inputTextBox);
    check(m_sendButton);
    check(m_endConversationButton);

    m_sendButton->OnClicked.AddDynamic(this, &UConversationWidget::OnSendClicked);
    m_inputTextBox->OnTextCommitted.AddDynamic(this, &UConversationWidget::OnInputTextCommitted);
    m_endConversationButton->OnClicked.AddDynamic(this, &UConversationWidget::OnEndConversationClicked);
}
void UConversationWidget::NativeDestruct()
{
    m_sendButton->OnClicked.RemoveAll(this);
    m_inputTextBox->OnTextCommitted.RemoveAll(this);
    m_endConversationButton->OnClicked.RemoveAll(this);

    Super::NativeDestruct();
}
void UConversationWidget::SubmitCurrentInputText() const
{
	check(m_inputTextBox);
	const FText textToSubmit = m_inputTextBox->GetText();
	if (textToSubmit.IsEmptyOrWhitespace())
	{
		return;
	}

	m_inputTextBox->SetText(FText::GetEmpty());
	m_OnTextSubmitted.Broadcast(textToSubmit);
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
}
void UConversationWidget::OnEndConversationClicked()
{
    m_OnConversationEnded.Broadcast();
}
