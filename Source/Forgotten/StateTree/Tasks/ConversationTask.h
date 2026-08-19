// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "Templates/SubclassOf.h"
#include "ConversationTask.generated.h"

class AFirstPersonCharacter;
class ARainNPC;
class UConversationWidget;

USTRUCT()
struct FConversationTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TWeakObjectPtr<AFirstPersonCharacter> m_PlayerCharacter = nullptr;
	UPROPERTY(Transient)
	TWeakObjectPtr<ARainNPC> m_RainNPC = nullptr;
	UPROPERTY(Transient)
	TWeakObjectPtr<UConversationWidget> m_WidgetPtr = nullptr;

	FDelegateHandle m_TextSubmittedHandle;
	FDelegateHandle m_EndedHandle;
};

/**
 * Tells the player to lock movement, spawn conversation UI, and change camera orientation.
 */
USTRUCT(meta = (DisplayName = "Conversation State", Category = "Gameplay"))
struct FORGOTTEN_API FConversationTask : public FStateTreeTaskBase
{
	GENERATED_BODY()

public:
	using FInstanceDataType = FConversationTaskInstanceData;

	FConversationTask();

	virtual const UScriptStruct* GetInstanceDataType() const override;
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& context,
		const FStateTreeTransitionResult& transitions) const override;
	virtual void ExitState(
		FStateTreeExecutionContext& context,
		const FStateTreeTransitionResult& transitions) const override;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UConversationWidget> m_ConversationWidgetClass;

	TStateTreeExternalDataHandle<AFirstPersonCharacter, EStateTreeExternalDataRequirement::Optional> m_PlayerCharacterHandle;
	TStateTreeExternalDataHandle<ARainNPC, EStateTreeExternalDataRequirement::Optional> m_RainHandle;
};
