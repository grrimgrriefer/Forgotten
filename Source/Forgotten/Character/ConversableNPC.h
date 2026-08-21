// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ConversableNPC.generated.h"

class AFirstPersonCharacter;
class UStaticMeshComponent;

/*
 * Base class for all conversable characters (Rain, Zannah, etc)
 */
UCLASS()
class FORGOTTEN_API AConversableNPC : public ACharacter
{
	GENERATED_BODY()

public:
	AConversableNPC();

	void StartConversation(AFirstPersonCharacter* player);
	void EndConversation();
	FText GetSpeakerName() const;
	FVector GetLocation() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> m_placeholderMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterInfo")
	FText m_characterName;
};
