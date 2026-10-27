// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "ConversableNPC.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Forgotten/Character/FirstPersonCharacter.h"
#include "Forgotten/Utils/AssertMacros.h"
#include "SubSystems/CharacterSubsystem.h"

AConversableNPC::AConversableNPC()
{
	PrimaryActorTick.bCanEverTick = false;

	m_placeholderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaceholderMesh"));
	m_placeholderMesh->SetupAttachment(RootComponent);
	m_interactionUiMessage = NSLOCTEXT("NPC", "TalkPrompt", "Talk");

	m_chatRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ChatRangeSphere"));
	m_chatRangeSphere->SetupAttachment(RootComponent);
	m_chatRangeSphere->SetSphereRadius(m_maxChatDistance);
	m_chatRangeSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	m_chatRangeSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}


#pragma region ACharacter
void AConversableNPC::BeginPlay()
{
	Super::BeginPlay();

	ASSERT_CHECK(m_chatRangeSphere);
	m_chatRangeSphere->SetSphereRadius(m_maxChatDistance);
	m_chatRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AConversableNPC::OnChatRangeBeginOverlap);

	// TODO: Check the VoxtaClient to see if the VoxtaCharacters are already available.
	// Register this one to the correct one if so, if not, subscribe to the event and register it for whenever they do get loaded.
	// RegisterCharacter()
}
void AConversableNPC::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	if (const UWorld* world = GetWorld())
	{
		if (UCharacterSubsystem* characterSubsystem = world->GetSubsystem<UCharacterSubsystem>())
		{
			characterSubsystem->UnregisterNPC(this);
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


#pragma region IVoxtaCharacter
void AConversableNPC::OnRegistered(const FVoxtaCharacterData& VoxtaCharData)
{
	// TODO: something? idk what yet
}
#pragma endregion IVoxtaCharacter


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
const FText& AConversableNPC::GetCharacterName() const
{
	return m_characterName;
}
void AConversableNPC::RegisterCharacter(const FVoxtaCharacterData& voxtaCharData)
{
	const UWorld* world = GetWorld();
	ASSERT_CHECK(world);

	UCharacterSubsystem* characterSubsystem = world->GetSubsystem<UCharacterSubsystem>();
	ASSERT_CHECK(characterSubsystem);
	characterSubsystem->TryRegisterNPC(this, voxtaCharData);
}
void AConversableNPC::OnChatRangeBeginOverlap(
	UPrimitiveComponent* overlappedComp,
	AActor* otherActor,
	UPrimitiveComponent* otherComp,
	int32 otherBodyIndex,
	bool bFromSweep,
	const FHitResult& sweepResult)
{
	if (AFirstPersonCharacter* player = Cast<AFirstPersonCharacter>(otherActor))
	{
		const UWorld* world = GetWorld();
		ASSERT_CHECK(world);

		UCharacterSubsystem* characterSubsystem = world->GetSubsystem<UCharacterSubsystem>();
		ASSERT_CHECK(characterSubsystem);
		characterSubsystem->TryStartConversation(this);
	}
}
