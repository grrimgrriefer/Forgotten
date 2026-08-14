// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "RainNPC.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/GameInstance.h"
#include "Forgotten/CustomGameplayTags.h"
#include "Forgotten/StateTree/MainStateTreeSubsystem.h"

ARainNPC::ARainNPC()
{
	PrimaryActorTick.bCanEverTick = false;

	m_placeholderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaceholderMesh"));
	m_placeholderMesh->SetupAttachment(RootComponent);

	m_interactionPrompt = NSLOCTEXT("Interaction", "TalkToRain", "Talk to Rain");
}


#pragma region IInteractableInterface
void ARainNPC::Interact(APlayerController* instigator)
{
	const UWorld* world = GetWorld();
	check(world);
	UMainStateTreeSubsystem* stateTreeSubsystem = world->GetGameInstance()->GetSubsystem<UMainStateTreeSubsystem>();

	check(stateTreeSubsystem);
	stateTreeSubsystem->TrySendFlowEvent(TAG_State_Conversation_Start);
}
FText ARainNPC::GetInteractionPrompt() const
{
	return m_interactionPrompt;
}
#pragma endregion IInteractableInterface

