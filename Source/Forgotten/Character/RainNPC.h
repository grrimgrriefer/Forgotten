// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Forgotten/Interactables/InteractableInterface.h"
#include "RainNPC.generated.h"

class UStaticMeshComponent;

/*
 * Rain character
 */
UCLASS()
class FORGOTTEN_API ARainNPC : public ACharacter, public IInteractableInterface
{
	GENERATED_BODY()

public:
	ARainNPC();

	#pragma region IInteractableInterface
		virtual void Interact(APlayerController* instigator) override;
		virtual FText GetInteractionPrompt() const override;
	#pragma endregion IInteractableInterface

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> m_placeholderMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText m_interactionPrompt;
};
