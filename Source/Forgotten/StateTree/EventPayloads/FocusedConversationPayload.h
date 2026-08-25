// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectPtr.h"
#include "FocusedConversationPayload.generated.h"

class AConversableNPC;

USTRUCT(BlueprintType)
struct FFocusedConversationPayload
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
	TObjectPtr<AConversableNPC> ConversableNpc = nullptr;
};
