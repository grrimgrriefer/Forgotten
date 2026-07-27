// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "MainStateTreeSubsystem.h"
#include "StateTreeExecutionContext.h"

#pragma region UGameInstanceSubsystem
bool UMainStateTreeSubsystem::ShouldCreateSubsystem(UObject* outer) const
{
	return GetClass() != StaticClass();
}
void UMainStateTreeSubsystem::Initialize(FSubsystemCollectionBase& collection)
{
	Super::Initialize(collection);
	FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &UMainStateTreeSubsystem::OnPostWorldInitialization);
}
void UMainStateTreeSubsystem::Deinitialize()
{
	FWorldDelegates::OnPostWorldInitialization.RemoveAll(this);
	if (m_isRunning && IsValid(m_stateTreeAsset))
	{
		FStateTreeExecutionContext context(*this, *m_stateTreeAsset, m_instanceData);
		context.Stop();
		m_isRunning = false;
		UE_LOG(LogTemp, Log, TEXT("%s: StateTree stopped."), *GetNameSafe(this));
	}
	Super::Deinitialize();
}
#pragma endregion FTickableGameObject


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
    if (!world || world->IsPreparingMapChange())
    {
        return false;
    }

	if (m_isRunning && IsValid(m_stateTreeAsset))
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
void UMainStateTreeSubsystem::OnPostWorldInitialization(UWorld* world, const UWorld::InitializationValues initValues)
{
	if (world && world->IsGameWorld())
	{
		if (m_isRunning || !IsValid(m_stateTreeAsset))
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
}
