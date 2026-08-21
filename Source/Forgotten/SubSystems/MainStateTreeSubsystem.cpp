// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "MainStateTreeSubsystem.h"

#include "MainStateTreeSchema.h"
#include "StateTreeExecutionContext.h"
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
bool UMainStateTreeSubsystem::TryBindContextData(UObject* data)
{
	if (!IsValid(data))
	{
		return false;
	}

	m_contextObjects.AddUnique(data);
	return true;
}
bool UMainStateTreeSubsystem::TryUnbindContextData(UObject* data)
{
	if (!IsValid(data))
	{
		return false;
	}

	const int32 removedCount = m_contextObjects.RemoveAll([data](const TWeakObjectPtr<UObject>& weakObj)
	{
		return !weakObj.IsValid() || weakObj.Get() == data;
	});

	return removedCount > 0;
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
bool UMainStateTreeSubsystem::SetContextRequirements(FStateTreeExecutionContext& context)
{
	if (!context.IsValid())
	{
		return false;
	}

	context.SetContextDataByName(UMainStateTreeSchema::m_SubsystemBindingName, FStateTreeDataView(this));

	for (const FStateTreeExternalDataDesc& desc : GetDefault<UMainStateTreeSchema>()->GetContextDataDescs())
	{
		if (desc.Name == UMainStateTreeSchema::m_SubsystemBindingName)
		{
			continue;
		}

		UObject* matchedObject = nullptr;
		if (const UClass* expectedClass = Cast<const UClass>(desc.Struct))
		{
			for (const TWeakObjectPtr<UObject>& weakObj : m_contextObjects)
			{
				if (UObject* obj = weakObj.Get())
				{
					if (obj->IsA(expectedClass))
					{
						matchedObject = obj;
						break;
					}
				}
			}
		}
		context.SetContextDataByName(desc.Name, FStateTreeDataView(desc.Struct, matchedObject));
	}

	context.SetCollectExternalDataCallback(FOnCollectStateTreeExternalData::CreateUObject(
		this,
		&UMainStateTreeSubsystem::CollectExternalData));

	return context.AreContextDataViewsValid();
}
bool UMainStateTreeSubsystem::CollectExternalData(
	const FStateTreeExecutionContext& context,
	const UStateTree* stateTree,
	TArrayView<const FStateTreeExternalDataDesc> externalDataDescs,
	TArrayView<FStateTreeDataView> outDataViews)
{
	for (int32 i = 0; i < externalDataDescs.Num(); ++i)
	{
		const FStateTreeExternalDataDesc& desc = externalDataDescs[i];
		if (desc.Struct && desc.Struct->IsChildOf(StaticClass()))
		{
			outDataViews[i] = FStateTreeDataView(this);
			continue;
		}

		UObject* matchedObject = nullptr;
		if (const UClass* targetClass = Cast<const UClass>(desc.Struct))
		{
			for (const TWeakObjectPtr<UObject>& weakObj : m_contextObjects)
			{
				if (UObject* obj = weakObj.Get())
				{
					if (obj->IsA(targetClass))
					{
						matchedObject = obj;
						break;
					}
				}
			}
		}
		outDataViews[i] = FStateTreeDataView(desc.Struct, matchedObject);
	}
	return true;
}
