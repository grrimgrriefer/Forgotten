// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

class ACharacter;

UINTERFACE(MinimalAPI)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Generic interface for interactable object (focus objects, doors, chairs, etc.)
 * Can be used by both Player and NPCs.
 */
class FORGOTTEN_API IInteractableInterface
{
	GENERATED_BODY()

public:
	virtual bool CanInteract(ACharacter* instigator) const = 0;
	virtual void Interact(ACharacter* instigator) = 0;
	virtual FText GetInteractionUiMessage(ACharacter* instigator) const = 0;
	virtual USceneComponent* GetInteractionPoint() const = 0;
};
