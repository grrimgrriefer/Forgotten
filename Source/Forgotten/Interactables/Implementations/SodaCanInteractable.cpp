// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "SodaCanInteractable.h"
#include "Forgotten/Character/FirstPersonCharacter.h"
#include "Forgotten/Utils/AssertMacros.h"

ASodaCanInteractable::ASodaCanInteractable()
{
	m_itemName = NSLOCTEXT("SodaCan", "ItemName", "Soda Can");
	m_interactionPrompt = NSLOCTEXT("SodaCan", "PickUpPrompt", "Inspect Soda Can");
}
bool ASodaCanInteractable::CanInteract(ACharacter* instigator) const
{
	return AInteractable::CanInteract(instigator) && !m_isBeingInspected;
}
void ASodaCanInteractable::OnPlayerInteract(AFirstPersonCharacter* player)
{
	ASSERT_CHECK(player);

	if (!m_isBeingInspected)
	{
		m_isBeingInspected = true;

		UE_LOG(LogTemp, Log, TEXT("Player picked up %s for inspection!"), *m_itemName.ToString());

		// TODO: 3D inspection state event trigger
	}
}
