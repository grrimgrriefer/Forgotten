// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "Interactable.h"
#include "Components/StaticMeshComponent.h"
#include "Forgotten/Character/ConversableNPC.h"
#include "Forgotten/Character/FirstPersonCharacter.h"
#include "Forgotten/Utils/AssertMacros.h"

AInteractable::AInteractable()
{
	PrimaryActorTick.bCanEverTick = false;

	m_rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(m_rootComp);

	m_meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	m_meshComp->SetupAttachment(m_rootComp);

	m_interactionPoint = CreateDefaultSubobject<USceneComponent>(TEXT("InteractionPoint"));
	m_interactionPoint->SetupAttachment(m_rootComp);

	m_interactionPrompt = NSLOCTEXT("Interactable", "DefaultPrompt", "Interact");
}

#pragma region IInteractableInterface
bool AInteractable::CanInteract(ACharacter* instigator) const
{
	return IsValid(instigator);
}

void AInteractable::Interact(ACharacter* instigator)
{
	if (!CanInteract(instigator))
	{
		return;
	}

	if (AFirstPersonCharacter* player = Cast<AFirstPersonCharacter>(instigator))
	{
		OnPlayerInteract(player);
	}
	else if (AConversableNPC* npc = Cast<AConversableNPC>(instigator))
	{
		OnNpcInteract(npc);
	}
}

FText AInteractable::GetInteractionUiMessage(ACharacter* instigator) const
{
	return m_interactionPrompt;
}

USceneComponent* AInteractable::GetInteractionPoint() const
{
	return m_interactionPoint;
}
#pragma endregion IInteractableInterface

void AInteractable::OnPlayerInteract(AFirstPersonCharacter* player)
{
}
void AInteractable::OnNpcInteract(AConversableNPC* npc)
{
}
