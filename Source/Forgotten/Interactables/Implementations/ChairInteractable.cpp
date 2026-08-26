// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "ChairInteractable.h"
#include "Forgotten/Character/FirstPersonCharacter.h"
#include "Forgotten/Utils/AssertMacros.h"

AChairInteractable::AChairInteractable()
{
	m_sitUserPrompt = NSLOCTEXT("Chair", "SitPrompt", "Sit");
	m_standUserPrompt = NSLOCTEXT("Chair", "StandPrompt", "Stand Up");
	m_interactionPrompt = m_sitUserPrompt;
}
bool AChairInteractable::CanInteract(ACharacter* instigator) const
{
	return AInteractable::CanInteract(instigator) && !m_isOccupied;
}
FText AChairInteractable::GetInteractionUiMessage(ACharacter* instigator) const
{
	return m_isOccupied ? m_standUserPrompt : m_sitUserPrompt;
}
void AChairInteractable::OnPlayerInteract(AFirstPersonCharacter* player)
{
	ASSERT_CHECK(player);

	m_isOccupied = true;
	player->SitDown(this);
}
void AChairInteractable::SetIsOccupied(const bool isOccupied)
{
	m_isOccupied = isOccupied;
}
