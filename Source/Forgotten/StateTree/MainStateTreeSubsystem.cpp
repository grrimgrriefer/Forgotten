// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "MainStateTreeSubsystem.h"

#include "MainStateTreeSchema.h"
#include "StateTreeExecutionContext.h"
#include "Forgotten/Character/FirstPersonCharacter.h"
#include "Forgotten/Character/RainNPC.h"
#include "Forgotten/Utils/AssertMacros.h"
#include "GameFramework/GameModeBase.h"
#include "UObject/FastReferenceCollector.h"

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
		if (SetContextRequirements(context))
		{
			context.Stop();
		}
		m_isRunning = false;
		UE_LOG(LogTemp, Log, TEXT("%s: StateTree stopped."), *GetNameSafe(this));
	}
	m_contextObjects.Empty();
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

	ASSERT_CHECK(IsValid(m_stateTreeAsset), TEXT("UMainStateTreeSubsystem: "
												"m_stateTreeAsset is not assigned, check the blueprint"));
	if (m_isRunning && IsValid(m_stateTreeAsset))
	{
		FStateTreeExecutionContext context(*this, *m_stateTreeAsset, m_instanceData);
		if (SetContextRequirements(context))
		{
			context.Tick(deltaTime);
		}
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


bool UMainStateTreeSubsystem::SetContextRequirements(FStateTreeExecutionContext& context)
{
	if (!context.IsValid())
	{
		return false;
	}

	context.SetContextDataByName(FName(TEXT("Subsystem")), FStateTreeDataView(this));

	UObject* playerObj = nullptr;
	if (const TWeakObjectPtr<UObject>* found = m_contextObjects.Find(UMainStateTreeSchema::m_PlayerBindingName))
	{
		playerObj = found->Get();
	}
	context.SetContextDataByName(
		UMainStateTreeSchema::m_PlayerBindingName,
		FStateTreeDataView(AFirstPersonCharacter::StaticClass(), playerObj));

	UObject* rainObj = nullptr;
	if (const TWeakObjectPtr<UObject>* found = m_contextObjects.Find(UMainStateTreeSchema::m_RainBindingName))
	{
		rainObj = found->Get();
	}
	context.SetContextDataByName(
		UMainStateTreeSchema::m_RainBindingName,
		FStateTreeDataView(ARainNPC::StaticClass(), rainObj));

	context.SetCollectExternalDataCallback(FOnCollectStateTreeExternalData::CreateUObject(
		this,
		&UMainStateTreeSubsystem::CollectExternalData));

	const bool isValid = context.AreContextDataViewsValid();
	ASSERT_CHECK_RETURN(isValid, false);
	return isValid;
}
bool UMainStateTreeSubsystem::CollectExternalData(
	const FStateTreeExecutionContext& context,
	const UStateTree* stateTree,
	TArrayView<const FStateTreeExternalDataDesc> externalDataDescs,
	TArrayView<FStateTreeDataView> outDataViews)
{
	for (int32 i = 0; i < externalDataDescs.Num(); i++)
	{
		const FStateTreeExternalDataDesc& desc = externalDataDescs[i];
		if (desc.Struct && desc.Struct->IsChildOf(StaticClass()))
		{
			outDataViews[i] = FStateTreeDataView(this);
		}
		else
		{
			UObject* objectPtr = nullptr;
			if (const TWeakObjectPtr<UObject>* foundData = m_contextObjects.Find(desc.Name))
			{
				objectPtr = foundData->Get();
			}

			if (!objectPtr && desc.Struct)
			{
				for (const auto& [name, weakObj] : m_contextObjects)
				{
					if (UObject* obj = weakObj.Get())
					{
						if (const UClass* itemClass = Cast<const UClass>(desc.Struct))
						{
							if (obj->IsA(itemClass))
							{
								objectPtr = obj;
								break;
							}
						}
					}
				}
			}
			outDataViews[i] = FStateTreeDataView(desc.Struct, objectPtr);
		}
	}
	return true;
}
bool UMainStateTreeSubsystem::TrySendFlowEvent(const FGameplayTag tag)
{
	const UWorld* world = GetWorld();

	ASSERT_CHECK_RETURN(world, false);
	ASSERT_CHECK_RETURN(!world->IsPreparingMapChange(), false,
		TEXT("UMainStateTreeSubsystem: Level is being changed while sending a flowevent, this should not happen."));
	ASSERT_CHECK_RETURN(IsValid(m_stateTreeAsset), false,
		TEXT("UMainStateTreeSubsystem: m_stateTreeAsset is not assigned, check the blueprint"));

	if (world->IsPreparingMapChange() || !IsValid(m_stateTreeAsset))
	{
		return false;
	}

	if (m_isRunning)
	{
		FStateTreeExecutionContext context(*this, *m_stateTreeAsset, m_instanceData);
		if (SetContextRequirements(context))
		{
			context.SendEvent(tag);
			return true;
		}
	}
	return false;
}
bool UMainStateTreeSubsystem::TryBindContextData(const FName contextName, UObject* data)
{
	m_contextObjects.FindOrAdd(contextName) = data;
	return true;
}
void UMainStateTreeSubsystem::OnGameModePostLoginEvent(AGameModeBase* gameMode, APlayerController* newPlayer)
{
	if (m_isRunning)
	{
		return;
	}

	ASSERT_CHECK(IsValid(m_stateTreeAsset), TEXT("UMainStateTreeSubsystem: "
												"m_stateTreeAsset is not assigned, check the blueprint"));
	if (!IsValid(m_stateTreeAsset))
	{
		return;
	}

	FStateTreeExecutionContext context(*this, *m_stateTreeAsset, m_instanceData);
	if (SetContextRequirements(context))
	{
		if (context.Start() == EStateTreeRunStatus::Running)
		{
			m_isRunning = true;
			UE_LOG(LogTemp, Log, TEXT("%s: StateTree started."), *GetNameSafe(this));
		}
	}
}
