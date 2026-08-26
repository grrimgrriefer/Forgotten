// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "Forgotten/Interactables/Interactable.h"
#include "ChairInteractable.generated.h"

/**
 * Chair
 */
UCLASS()
class FORGOTTEN_API AChairInteractable : public AInteractable
{
	GENERATED_BODY()

public:
	AChairInteractable();
	virtual bool CanInteract(ACharacter* instigator) const override;
	virtual FText GetInteractionUiMessage(ACharacter* instigator) const override;
	void SetIsOccupied(const bool isOccupied);

protected:
	virtual void OnPlayerInteract(AFirstPersonCharacter* player) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText m_sitUserPrompt;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText m_standUserPrompt;

	bool m_isOccupied = false;
};
