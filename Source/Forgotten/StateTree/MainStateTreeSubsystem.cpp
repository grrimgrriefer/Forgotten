// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "MainStateTreeSubsystem.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/GameModeBase.h"

#pragma region UGameInstanceSubsystem
bool UMainStateTreeSubsystem::ShouldCreateSubsystem(UObject* outer) const
{
	return GetClass() != StaticClass();
}
void UMainStateTreeSubsystem::Initialize(FSubsystemCollectionBase& collection)
{
	Super::Initialize(collection);
	FGameModeEvents::GameModePostLoginEvent.AddUObject(this, &UMainStateTreeSubsystem::OnGameModePostLoginEvent);
}
void UMainStateTreeSubsystem::Deinitialize()
{
	FGameModeEvents::GameModePostLoginEvent.RemoveAll(this);
	if (m_isRunning && IsValid(m_stateTreeAsset))
	{
		FStateTreeExecutionContext context(*this, *m_stateTreeAsset, m_instanceData);
		context.Stop();
		m_isRunning = false;
		UE_LOG(LogTemp, Log, TEXT("%s: StateTree stopped."), *GetNameSafe(this));
	}
	Super::Deinitialize();
}
#pragma endregion UGameInstanceSubsystem


#pragma region FTickableGameObject
void UMainStateTreeSubsystem::Tick(const float deltaTime)
{
	if (m_lastFrameNumberWeTicked == GFrameCounter)
	{
		return;
	}
	m_lastFrameNumberWeTicked = GFrameCounter;

	const UWorld* world = GetWorld();
    if (!world || world->IsPreparingMapChange())
    {
        return;
    }

	ensureMsgf(IsValid(m_stateTreeAsset), TEXT("UMainStateTreeSubsystem: "
												"m_stateTreeAsset is not assigned, check the blueprint"));
	if (m_isRunning && IsValid(m_stateTreeAsset))
	{
		FStateTreeExecutionContext context(*this, *m_stateTreeAsset, m_instanceData);
		context.Tick(deltaTime);
	}
}
ETickableTickType UMainStateTreeSubsystem::GetTickableTickType() const
{
	return ETickableTickType::Always;
}
TStatId UMainStateTreeSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UMainStateTreeSubsystem, STATGROUP_Tickables);
}
bool UMainStateTreeSubsystem::IsTickableWhenPaused() const
{
	return true;
}
bool UMainStateTreeSubsystem::IsTickableInEditor() const
{
	return false;
}
bool UMainStateTreeSubsystem::IsTickable() const
{
    return !HasAnyFlags(RF_ClassDefaultObject) && m_isRunning && IsValid(m_stateTreeAsset);
}
#pragma endregion FTickableGameObject


bool UMainStateTreeSubsystem::TrySendFlowEvent(const FGameplayTag tag)
{
	const UWorld* world = GetWorld();

	check(world);
	ensureMsgf(!world->IsPreparingMapChange(), TEXT("UMainStateTreeSubsystem: "
												 "Level is being changed while sending a flowevent, this should not happen."));
	ensureMsgf(IsValid(m_stateTreeAsset), TEXT("UMainStateTreeSubsystem: "
												"m_stateTreeAsset is not assigned, check the blueprint"));

	if (world->IsPreparingMapChange() || !IsValid(m_stateTreeAsset))
	{
		return false;
	}

	if (m_isRunning)
	{
		FStateTreeExecutionContext context(*this, *m_stateTreeAsset, m_instanceData);
		context.SendEvent(tag);
		return true;
	}
	else
	{
		return false;
	}
}
void UMainStateTreeSubsystem::OnGameModePostLoginEvent(AGameModeBase* gameMode, APlayerController* newPlayer)
{
	if (m_isRunning)
	{
		return;
	}

	ensureMsgf(IsValid(m_stateTreeAsset), TEXT("UMainStateTreeSubsystem: "
												"m_stateTreeAsset is not assigned, check the blueprint"));
	if (!IsValid(m_stateTreeAsset))
	{
		return;
	}

	FStateTreeExecutionContext context(*this, *m_stateTreeAsset, m_instanceData);
	if (context.Start() == EStateTreeRunStatus::Running)
	{
		m_isRunning = true;
		UE_LOG(LogTemp, Log, TEXT("%s: StateTree started."), *GetNameSafe(this));
	}
}
