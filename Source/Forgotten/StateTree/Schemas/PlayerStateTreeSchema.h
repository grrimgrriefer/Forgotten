// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeExecutionTypes.h"
#include "StateTreeSchema.h"
#include "PlayerStateTreeSchema.generated.h"

/**
 * Custom schema for the player
 */
UCLASS()
class FORGOTTEN_API UPlayerStateTreeSchema : public UStateTreeSchema
{
	GENERATED_BODY()

public:
	UPlayerStateTreeSchema();

	virtual TConstArrayView<FStateTreeExternalDataDesc> GetContextDataDescs() const override;
	virtual bool IsStructAllowed(const UScriptStruct* inScriptStruct) const override;
	virtual bool IsExternalItemAllowed(const UStruct& inStruct) const override;

	static const FName m_PlayerBindingName;
	static const FName m_ConversableNpcBindingName;

protected:
	UPROPERTY()
	FStateTreeExternalDataDesc m_playerData;
	UPROPERTY()
	FStateTreeExternalDataDesc m_conversableNpcData;

	TArray<FStateTreeExternalDataDesc> m_contextDescs;
};
