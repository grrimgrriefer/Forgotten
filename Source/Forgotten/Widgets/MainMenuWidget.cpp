// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "MainMenuWidget.h"
#include "Components/Button.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	check(m_continueButton);
	check(m_loadSaveButton);
	check(m_newGameButton);
	check(m_quitButton);
	check(m_openSavesMenuButton);
	check(m_openSettingsButton);
	check(m_backButton);

	m_continueButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnContinueClicked);
	m_loadSaveButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnLoadSaveClicked);
	m_newGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnNewGameClicked);
	m_quitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitClicked);
	m_openSavesMenuButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnOpenSavesMenuClicked);
	m_openSettingsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnOpenSettingsClicked);
	m_backButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnBackClicked);
}

void UMainMenuWidget::NativeDestruct()
{
	m_continueButton->OnClicked.RemoveAll(this);
	m_loadSaveButton->OnClicked.RemoveAll(this);
	m_newGameButton->OnClicked.RemoveAll(this);
	m_quitButton->OnClicked.RemoveAll(this);
	m_openSavesMenuButton->OnClicked.RemoveAll(this);
	m_openSettingsButton->OnClicked.RemoveAll(this);
	m_backButton->OnClicked.RemoveAll(this);

	Super::NativeDestruct();
}

void UMainMenuWidget::OnContinueClicked() const
{
	// TODO
	m_OnActionRequested.Broadcast(EMainMenuAction::Continue);
}
void UMainMenuWidget::OnNewGameClicked() const
{
	// TODO
	m_OnActionRequested.Broadcast(EMainMenuAction::Continue);
}
void UMainMenuWidget::OnLoadSaveClicked() const
{
	// TODO
	m_OnActionRequested.Broadcast(EMainMenuAction::Continue);
}
void UMainMenuWidget::OnQuitClicked() const
{
	m_OnActionRequested.Broadcast(EMainMenuAction::Quit);
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
