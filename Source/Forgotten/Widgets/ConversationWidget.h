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
DECLARE_MULTICAST_DELEGATE(FOnConversationEnded);

/**
 * Manages the UI during conversations with Rain.
 */
UCLASS(Abstract, Blueprintable)
class FORGOTTEN_API UConversationWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnConversationTextSubmitted m_OnTextSubmitted;
	FOnConversationEnded m_OnConversationEnded;

	void AddTranscriptEntry(const FText& speakerName, const FText& messageText) const;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> m_transcriptScrollBox;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> m_inputTextBox;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> m_sendButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> m_endConversationButton;

private:
	void SubmitCurrentInputText() const;

	UFUNCTION()
	void OnSendClicked();
	UFUNCTION()
	void OnInputTextCommitted(const FText& text, ETextCommit::Type commitMethod);
	UFUNCTION()
	void OnEndConversationClicked();
};
