// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "ConversableNPC.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Forgotten/Character/FirstPersonCharacter.h"
#include "Forgotten/SubSystems/ConversationSubsystem.h"
#include "Forgotten/Utils/AssertMacros.h"

AConversableNPC::AConversableNPC()
{
	PrimaryActorTick.bCanEverTick = false;

	m_placeholderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaceholderMesh"));
	m_placeholderMesh->SetupAttachment(RootComponent);
	m_interactionUiMessage = NSLOCTEXT("NPC", "TalkPrompt", "Talk");
}


#pragma region ACharacter
void AConversableNPC::BeginPlay()
{
	Super::BeginPlay();

	const UWorld* world = GetWorld();
	ASSERT_CHECK(world);

	UConversationSubsystem* conversationSubSystem = world->GetSubsystem<UConversationSubsystem>();
	ASSERT_CHECK(conversationSubSystem);
	conversationSubSystem->RegisterConversableNPC(this);
}
void AConversableNPC::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	if (const UWorld* world = GetWorld())
	{
		if (UConversationSubsystem* conversationSubsystem = world->GetSubsystem<UConversationSubsystem>())
		{
			conversationSubsystem->UnregisterConversableNPC(this);
		}
	}

	Super::EndPlay(endPlayReason);
}
#pragma endregion ACharacter


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


bool AConversableNPC::IsInRangeForChat(const ACharacter* other) const
{
	if (!IsValid(other))
	{
		return false;
	}

	const float distSq = FVector::DistSquared(GetActorLocation(), other->GetActorLocation());
	return distSq <= (m_maxChatDistance * m_maxChatDistance);
}
float AConversableNPC::GetMaxChatDistance() const
{
	return m_maxChatDistance;
}
