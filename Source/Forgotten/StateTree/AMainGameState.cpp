// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "AMainGameState.h"
#include "Components/StateTreeComponent.h"

AMainGameState::AMainGameState()
{
	m_stateTreeComponent = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComponent"));
	m_stateTreeComponent->SetStartLogicAutomatically(true);
}

void AMainGameState::TriggerFlowEvent(FGameplayTag EventTag) const
{
	if (m_stateTreeComponent)
	{
		FStateTreeEvent Event;
		Event.Tag = EventTag;
		m_stateTreeComponent->SendStateTreeEvent(Event);
	}
}

void AMainGameState::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("Game Flow StateTree Initialized."));
}