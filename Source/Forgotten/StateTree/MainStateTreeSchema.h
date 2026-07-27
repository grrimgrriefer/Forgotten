// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeExecutionTypes.h"
#include "StateTreeSchema.h"
#include "MainStateTreeSchema.generated.h"

class UMainStateTreeSubsystem;

/**
 * Custom schema for the StateTree being on the subsystem
 */
UCLASS()
class FORGOTTEN_API UMainStateTreeSchema : public UStateTreeSchema
{
	GENERATED_BODY()

public:
	UMainStateTreeSchema();

	virtual TConstArrayView<FStateTreeExternalDataDesc> GetContextDataDescs() const override;
	virtual bool IsStructAllowed(const UScriptStruct* inScriptStruct) const override;

protected:
	UPROPERTY()
	FStateTreeExternalDataDesc m_subsystemData;
};
