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
	UMainStateTreeSchema();

	virtual TConstArrayView<FStateTreeExternalDataDesc> GetContextDataDescs() const override;
	virtual bool IsStructAllowed(const UScriptStruct* inScriptStruct) const override;
	virtual bool IsExternalItemAllowed(const UStruct& inStruct) const override;

	static const FName m_SubsystemBindingName;

protected:
	UPROPERTY()
	FStateTreeExternalDataDesc m_subsystemData;

	TArray<FStateTreeExternalDataDesc> m_contextDescs;
};
