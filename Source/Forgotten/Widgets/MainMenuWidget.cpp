// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "MainMenuWidget.h"
#include "Components/Button.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	check(m_continueButton);
	check(m_quitButton);
	m_continueButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnContinueClicked);
	m_quitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitClicked);
}
void UMainMenuWidget::NativeDestruct()
{
	m_continueButton->OnClicked.RemoveAll(this);
	m_quitButton->OnClicked.RemoveAll(this);

	Super::NativeDestruct();
}
void UMainMenuWidget::OnContinueClicked() const
{
	m_OnActionRequested.Broadcast(EMainMenuAction::Continue);
}
void UMainMenuWidget::OnQuitClicked() const
{
	m_OnActionRequested.Broadcast(EMainMenuAction::Quit);
}
