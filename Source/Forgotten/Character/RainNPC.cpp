// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "RainNPC.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/GameInstance.h"
#include "Forgotten/CustomGameplayTags.h"
#include "Forgotten/StateTree/MainStateTreeSubsystem.h"
#include "Forgotten/Utils/AssertMacros.h"

ARainNPC::ARainNPC()
{
	PrimaryActorTick.bCanEverTick = false;

	m_placeholderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaceholderMesh"));
	m_placeholderMesh->SetupAttachment(RootComponent);

	m_interactionPrompt = NSLOCTEXT("Interaction", "TalkToRain", "Talk to Rain");
}


#pragma region IInteractableInterface
void ARainNPC::Interact(ACharacter* instigator)
{

}
FText ARainNPC::GetInteractionPrompt() const
{
	return m_interactionPrompt;
}
#pragma endregion IInteractableInterface

