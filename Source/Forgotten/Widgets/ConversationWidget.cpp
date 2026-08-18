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
void UConversationWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ASSERT_CHECK(m_transcriptScrollBox);
    ASSERT_CHECK(m_inputTextBox);
    ASSERT_CHECK(m_sendButton);
    ASSERT_CHECK(m_endConversationButton);

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
	ASSERT_CHECK(m_inputTextBox);
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
