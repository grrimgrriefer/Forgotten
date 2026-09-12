// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "GenericInspectable.h"

#include "Components/StaticMeshComponent.h"
#include "Forgotten/Character/FirstPersonCharacter.h"
#include "Forgotten/Utils/AssertMacros.h"

AGenericInspectable::AGenericInspectable()
{
	m_itemName = NSLOCTEXT("GenericInspectable", "ItemName", "GenericInspectable name");
	m_interactionPrompt = NSLOCTEXT("GenericInspectable", "PickUpPrompt", "Inspect GenericInspectable");
}
bool AGenericInspectable::CanInteract(ACharacter* instigator) const
{
	return AInteractable::CanInteract(instigator) && !m_isBeingInspected;
}
void AGenericInspectable::OnPlayerInteract(AFirstPersonCharacter* player)
{
	ASSERT_CHECK(player);

	m_isBeingInspected = true;
	player->Inspect3dInteractable(this);
}
void AGenericInspectable::SetIsBeingInspected(const bool isBeingInspected)
{
	m_isBeingInspected = isBeingInspected;
}
UStaticMesh* AGenericInspectable::GetInspectMesh() const
{
	ASSERT_CHECK_RETURN(m_isBeingInspected, nullptr);
	return m_meshComp->GetStaticMesh();
}
FVector AGenericInspectable::GetInspectScale() const
{
	ASSERT_CHECK_RETURN(m_meshComp, FVector::OneVector);
	return m_meshComp->GetComponentScale();
}
