// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"
#include "StateTreeInstanceData.h"
#include "Engine/World.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MainStateTreeSubsystem.generated.h"

class UStateTree;
class UObject;
class FSubsystemCollectionBase;

/**
 * Holds the main (persistent) StateTree and controls the high-level flow of the game.
 * Should not contain pointers to objects that are destroyed when leaving a Level.
 */
UCLASS(Abstract, Blueprintable)
class FORGOTTEN_API UMainStateTreeSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
	
public:	
	#pragma region UGameInstanceSubsystem
		virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
		virtual void Initialize(FSubsystemCollectionBase& Collection) override;
		virtual void Deinitialize() override;
	#pragma endregion UGameInstanceSubsystem

	#pragma region FTickableGameObject
		virtual void Tick(const float deltaTime) override;
		virtual ETickableTickType GetTickableTickType() const override;
		virtual TStatId GetStatId() const override;
		virtual bool IsTickableWhenPaused() const override;
		virtual bool IsTickableInEditor() const override;
		virtual bool IsTickable() const override;
	#pragma endregion FTickableGameObject

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	bool TrySendFlowEvent(const FGameplayTag tag);
	 
protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UStateTree> m_stateTreeAsset;
	 
private:
	void OnPostWorldInitialization(UWorld* world, const UWorld::InitializationValues iValues);

	UPROPERTY()
	FStateTreeInstanceData m_instanceData;
	
	uint32 m_lastFrameNumberWeTicked = INDEX_NONE;
	bool m_isRunning = false;
};
