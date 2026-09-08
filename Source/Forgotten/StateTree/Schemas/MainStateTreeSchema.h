// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeExecutionTypes.h"
#include "StateTreeSchema.h"
#include "MainStateTreeSchema.generated.h"

/**
 * Custom schema for the main StateTree
 */
UCLASS()
class FORGOTTEN_API UMainStateTreeSchema : public UStateTreeSchema
{
	GENERATED_BODY()

public:
	static const FName MAIN_STATE_TREE_SUBSYSTEM_BINDING_NAME;

	UMainStateTreeSchema();

	virtual TConstArrayView<FStateTreeExternalDataDesc> GetContextDataDescs() const override;
	virtual bool IsStructAllowed(const UScriptStruct* inScriptStruct) const override;
	virtual bool IsExternalItemAllowed(const UStruct& inStruct) const override;

protected:
	UPROPERTY()
	FStateTreeExternalDataDesc m_subsystemData;

	TArray<FStateTreeExternalDataDesc> m_contextDescs;
};
