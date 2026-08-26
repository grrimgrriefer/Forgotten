// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "SodaCanInteractable.h"

#include "Components/StaticMeshComponent.h"
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

	m_isBeingInspected = true;
	player->Inspect3dInteractable(this);
}
void ASodaCanInteractable::SetIsBeingInspected(const bool isBeingInspected)
{
	m_isBeingInspected = isBeingInspected;
}
UStaticMesh* ASodaCanInteractable::GetInspectMesh() const
{
	ASSERT_CHECK_RETURN(m_isBeingInspected, nullptr);
	return m_meshComp->GetStaticMesh();
}
