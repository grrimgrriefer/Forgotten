// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Forgotten/Interactables/ConversableInterface.h"
#include "RainNPC.generated.h"

class UStaticMeshComponent;

/*
 * Rain character
 */
UCLASS()
class FORGOTTEN_API ARainNPC : public ACharacter, public IConversableInterface
{
	GENERATED_BODY()

public:
	ARainNPC();

	#pragma region IConversableInterface
		virtual void StartConversation(ACharacter* instigator) override;
		virtual void EndConversation() override;
		virtual FText GetSpeakerName() const override;
	#pragma endregion IConversableInterface

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> m_placeholderMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText m_characterName;
};
