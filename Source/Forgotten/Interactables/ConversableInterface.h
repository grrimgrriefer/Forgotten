// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ConversableInterface.generated.h"

UINTERFACE(MinimalAPI)
class UConversableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for characters that the player can talk to.
 */
class FORGOTTEN_API IConversableInterface
{
	GENERATED_BODY()

public:
	virtual void StartConversation(ACharacter* instigator) = 0;
	virtual void EndConversation() = 0;
	virtual FText GetSpeakerName() const = 0;
};
