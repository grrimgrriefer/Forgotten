// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "ConversableNPC.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/GameInstance.h"
#include "Forgotten/CustomGameplayTags.h"
#include "Forgotten/Utils/AssertMacros.h"

AConversableNPC::AConversableNPC()
{
	PrimaryActorTick.bCanEverTick = false;

	m_placeholderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaceholderMesh"));
	m_placeholderMesh->SetupAttachment(RootComponent);
}
FText AConversableNPC::GetSpeakerName() const
{
	return m_characterName;
}
FVector AConversableNPC::GetLocation() const
{
	return GetActorLocation();
}
