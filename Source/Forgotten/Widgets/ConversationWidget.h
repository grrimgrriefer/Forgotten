// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "ConversationWidget.generated.h"

class UButton;
class UEditableTextBox;
class UScrollBox;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnConversationTextSubmitted, const FText&);
DECLARE_MULTICAST_DELEGATE(FOnChatFocusLost);

/**
 * Toggleable Widget that displays the current status of the ongoing conversation.
 * Also handles manual textmessage entry.
 */
UCLASS(Abstract, Blueprintable)
class FORGOTTEN_API UConversationWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnConversationTextSubmitted m_OnTextSubmitted;
	FOnChatFocusLost m_OnChatFocusLost;

	void AddTranscriptEntry(const FText& speakerName, const FText& messageText) const;
	void ToggleTranscriptVisibility();
	void FocusInput();
	void UnfocusInput();
	bool IsInputFocused() const;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> m_transcriptScrollBox;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> m_inputTextBox;

private:
	void SubmitCurrentInputText();

	UFUNCTION()
	void OnSendClicked();
	UFUNCTION()
	void OnInputTextCommitted(const FText& text, ETextCommit::Type commitMethod);
};
