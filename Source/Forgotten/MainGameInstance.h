// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MainGameInstance.generated.h"

/**
 * Holds references to the subsystems to avoid lazyload issues
 */
UCLASS()
class FORGOTTEN_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subsystems")
	TArray<TSubclassOf<UGameInstanceSubsystem>> m_PreloadedSubsystems;
};
