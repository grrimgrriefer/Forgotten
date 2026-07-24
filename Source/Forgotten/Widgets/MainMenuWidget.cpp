// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "MainMenuWidget.h"
#include "Components/Button.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ContinueButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnContinueClicked);
	LoadSaveButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnLoadSaveClicked);
	NewGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnNewGameClicked);
	QuitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitClicked);

	OpenSavesMenuButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnOpenSavesMenuClicked);
	OpenSettingsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnOpenSettingsClicked);
	BackButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnBackClicked);
}

void UMainMenuWidget::NativeDestruct()
{
	ContinueButton->OnClicked.RemoveAll(this);
	NewGameButton->OnClicked.RemoveAll(this);
	OpenSavesMenuButton->OnClicked.RemoveAll(this);
	LoadSaveButton->OnClicked.RemoveAll(this);
	OpenSettingsButton->OnClicked.RemoveAll(this);
	QuitButton->OnClicked.RemoveAll(this);
	BackButton->OnClicked.RemoveAll(this);

	Super::NativeDestruct();
}

void UMainMenuWidget::OnContinueClicked() const
{
	OnLoadSaveRequested.Broadcast(TEXT("TODO: LAST SAVE OR SOMETHING"));
}
void UMainMenuWidget::OnNewGameClicked() const
{
	OnActionRequested.Broadcast(EMainMenuAction::NewGame);
}
void UMainMenuWidget::OnLoadSaveClicked() const
{
	OnLoadSaveRequested.Broadcast(TEXT("TODO: CUSTOM SAVE?"));
}
void UMainMenuWidget::OnQuitClicked() const
{
	OnActionRequested.Broadcast(EMainMenuAction::Quit);
}
void UMainMenuWidget::OnOpenSavesMenuClicked()
{
	// TODO
}
void UMainMenuWidget::OnOpenSettingsClicked()
{
	// TODO
}
void UMainMenuWidget::OnBackClicked()
{
	// TODO
}
