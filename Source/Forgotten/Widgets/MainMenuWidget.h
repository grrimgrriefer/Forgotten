// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;

UENUM(BlueprintType)
enum class EMainMenuAction : uint8
{
	Continue,
	Quit
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMenuActionRequested, EMainMenuAction);

/**
 * Manages the UI for the Main Menu, forwards the results back to the StateTree.
 */
UCLASS(Abstract, Blueprintable)
class FORGOTTEN_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnMenuActionRequested m_OnActionRequested;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> m_continueButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> m_quitButton;

private:
	UFUNCTION()
	void OnContinueClicked() const;
	UFUNCTION()
	void OnQuitClicked() const;
};
