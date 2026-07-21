// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AMainGameState.generated.h"

struct FGameplayTag;
class UStateTree;
class UStateTreeComponent;

/**
 * Global flow hosted by the GameState
 */
UCLASS()
class FORGOTTEN_API AMainGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AMainGameState();

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void TriggerFlowEvent(FGameplayTag EventTag) const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StateTree", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStateTreeComponent> m_stateTreeComponent;

	virtual void BeginPlay() override;
};
