// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "ConversationWidget.generated.h"

class UButton;
class UEditableTextBox;
class UScrollBox;
class UTextBlock;
class UWidgetAnimation;

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

	void AddTranscriptEntry(const FText& speakerName, const FText& messageText);
	void SetTranscriptVisibility(bool isVisible);
	void FocusInput();
	void UnfocusInput();
	bool IsInputFocused() const;
	void SetOutOfRangeFeedbackVisibility(bool isVisible);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* animation) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> m_transcriptScrollBox;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> m_inputTextBox;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> m_outOfRangeTextBlock;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> m_fadeOutAnimation;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> m_fadeInAnimation;

	UPROPERTY(EditAnywhere, Category = "UI effects")
	float m_fadeDelay = 3.5f;
	UPROPERTY(EditAnywhere, Category = "UI effects")
	float m_unfocusedInputOpacity = 0.4f;

	UPROPERTY(EditAnywhere, Category = "UI Localization")
	FText m_outOfRangeMessage;

private:
	const ESlateVisibility m_visibleValue = ESlateVisibility::SelfHitTestInvisible;
	const ESlateVisibility m_hiddenValue = ESlateVisibility::Collapsed;

	void StartFadeTimer();
	void ClearFadeTimer();
	void OnFadeTimerExpired();
	void SubmitCurrentInputText(const FText& text);
	void ClearInputFieldAndSetLowOpacity() const;

	UFUNCTION()
	void OnInputTextCommitted(const FText& text, ETextCommit::Type commitMethod);

	FTimerHandle m_fadeTimerHandle;
};
