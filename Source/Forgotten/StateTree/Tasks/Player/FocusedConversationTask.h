// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "FocusedConversationTask.generated.h"

class AFirstPersonCharacter;
class AConversableNPC;

USTRUCT()
struct FConversationTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AConversableNPC> m_conversableNpc = nullptr;
};

/**
 * Handles the lifecycle of focused conversations. (i.e. face-to-face-, seated-, etc.)
 * Also takes care of notifying the NPC when entering & exiting.
 */
USTRUCT(meta = (DisplayName = "Focused Conversation State", Category = "Gameplay"))
struct FORGOTTEN_API FFocusedConversationTask : public FStateTreeTaskBase
{
	GENERATED_BODY()

public:
	using FInstanceDataType = FConversationTaskInstanceData;

	FFocusedConversationTask();

	virtual const UScriptStruct* GetInstanceDataType() const override;
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& context,
		const FStateTreeTransitionResult& transitions) const override;
	virtual void ExitState(
		FStateTreeExecutionContext& context,
		const FStateTreeTransitionResult& transitions) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& context, const float deltaTime) const override;

	TStateTreeExternalDataHandle<AFirstPersonCharacter> m_PlayerCharacterHandle;
};
