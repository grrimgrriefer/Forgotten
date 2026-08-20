// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "RainNPC.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/GameInstance.h"
#include "Forgotten/CustomGameplayTags.h"
#include "Forgotten/Utils/AssertMacros.h"

ARainNPC::ARainNPC()
{
	PrimaryActorTick.bCanEverTick = false;

	m_placeholderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaceholderMesh"));
	m_placeholderMesh->SetupAttachment(RootComponent);

	m_characterName = NSLOCTEXT("Names", "NameRain", "Rain");
}


#pragma region IConversableInterface
void ARainNPC::StartConversation(ACharacter* instigator)
{
}
void ARainNPC::EndConversation()
{
}
FText ARainNPC::GetSpeakerName() const
{
	return m_characterName;
}
#pragma endregion IConversableInterface

