// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "ConversableNPC.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Forgotten/Character/FirstPersonCharacter.h"
#include "../Plugins/UnrealVoxta/Source/UnrealVoxta/Public/SubSystems/ConversationSubsystem.h"
#include "Forgotten/Utils/AssertMacros.h"

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

	const UWorld* world = GetWorld();
	ASSERT_CHECK(world);

	UConversationSubsystem* conversationSubSystem = world->GetSubsystem<UConversationSubsystem>();
	ASSERT_CHECK(conversationSubSystem);
	conversationSubSystem->RegisterNPC(this);
}
void AConversableNPC::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	if (const UWorld* world = GetWorld())
	{
		if (UConversationSubsystem* conversationSubsystem = world->GetSubsystem<UConversationSubsystem>())
		{
			conversationSubsystem->UnregisterNPC(this);
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
const FText& AConversableNPC::GetCharacterName() const
{
	return m_characterName;
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

		UConversationSubsystem* conversationSubSystem = world->GetSubsystem<UConversationSubsystem>();
		ASSERT_CHECK(conversationSubSystem);
		conversationSubSystem->StartConversation(this);
	}
}
