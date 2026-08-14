// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "ConversationTask.generated.h"

class AFirstPersonCharacter;
class ARainNPC;

USTRUCT()
struct FConversationTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TWeakObjectPtr<AFirstPersonCharacter> m_PlayerCharacter = nullptr;
	UPROPERTY(Transient)
	TWeakObjectPtr<ARainNPC> m_TargetNPC = nullptr;
};

/**
 * Tells the player to lock movement and change camera orientation during NPC conversation.
 */
USTRUCT(meta = (DisplayName = "Conversation State", Category = "Gameplay"))
struct FORGOTTEN_API FConversationTask : public FStateTreeTaskBase
{
	GENERATED_BODY()

public:
	using FInstanceDataType = FConversationTaskInstanceData;

	FConversationTask();

	virtual const UScriptStruct* GetInstanceDataType() const override;
	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& context,
		const FStateTreeTransitionResult& transitions) const override;
	virtual void ExitState(
		FStateTreeExecutionContext& context,
		const FStateTreeTransitionResult& transitions) const override;
};
