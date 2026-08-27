// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "ConversableNPC.h"
#include "Components/StaticMeshComponent.h"
#include "Forgotten/Character/FirstPersonCharacter.h"
#include "Forgotten/Utils/AssertMacros.h"

AConversableNPC::AConversableNPC()
{
	PrimaryActorTick.bCanEverTick = false;

	m_placeholderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaceholderMesh"));
	m_placeholderMesh->SetupAttachment(RootComponent);
	m_interactionUiMessage = NSLOCTEXT("NPC", "TalkPrompt", "Talk");
}

#pragma region IInteractableInterface
bool AConversableNPC::CanInteract(ACharacter* instigator) const
{
	return IsValid(instigator) && instigator != this;
}
void AConversableNPC::Interact(ACharacter* instigator)
{
	if (!CanInteract(instigator))
	{
		return;
	}

	if (AFirstPersonCharacter* player = Cast<AFirstPersonCharacter>(instigator))
	{
		player->StartFocusedConversation(this);
	}
}
FText AConversableNPC::GetInteractionUiMessage(ACharacter* instigator) const
{
	return m_interactionUiMessage;
}
USceneComponent* AConversableNPC::GetInteractionPoint() const
{
	return GetRootComponent();
}
#pragma endregion IInteractableInterface
