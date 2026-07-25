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

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMenuActionRequested, EMainMenuAction, Action);

/**
 * Manages the UI for the Main Menu, forwards the results back to the StateTree.
 */
UCLASS(Abstract, Blueprintable)
class FORGOTTEN_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(Category = "Events")
	FOnMenuActionRequested OnActionRequested;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ContinueButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> LoadSaveButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> NewGameButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> OpenSavesMenuButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> OpenSettingsButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:
	UFUNCTION()
	void OnContinueClicked() const;

	UFUNCTION()
	void OnNewGameClicked() const;
	
	UFUNCTION()
	void OnLoadSaveClicked() const;
	
	UFUNCTION()
	void OnQuitClicked() const;
	
	UFUNCTION()
	void OnOpenSavesMenuClicked();
	
	UFUNCTION()
	void OnOpenSettingsClicked();
	
	UFUNCTION()
	void OnBackClicked();
};
