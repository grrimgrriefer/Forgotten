// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

UINTERFACE(MinimalAPI)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Generic interface for interactable object (focus objects, doors, chairs, etc.)
 */
class FORGOTTEN_API IInteractableInterface
{
	GENERATED_BODY()

public:
	virtual void Interact(APlayerController* instigator) = 0;
	virtual FText GetInteractionPrompt() const = 0;
};
