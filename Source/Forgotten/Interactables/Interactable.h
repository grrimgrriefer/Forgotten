// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Forgotten/Interactables/InteractableInterface.h"
#include "Interactable.generated.h"

class AConversableNPC;
class AFirstPersonCharacter;
class USceneComponent;
class UStaticMeshComponent;

/**
 * Base actor for scene objects that both Player and NPCs can interact with.
 */
UCLASS(Abstract)
class FORGOTTEN_API AInteractable : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	AInteractable();

#pragma region IInteractableInterface
	virtual bool CanInteract(ACharacter* instigator) const override;
	virtual void Interact(ACharacter* instigator) override;
	virtual FText GetInteractionUiMessage(ACharacter* instigator) const override;
	virtual USceneComponent* GetInteractionPoint() const override;
#pragma endregion IInteractableInterface

protected:
	virtual void OnPlayerInteract(AFirstPersonCharacter* player);
	virtual void OnNpcInteract(AConversableNPC* npc);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> m_rootComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> m_meshComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> m_interactionPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText m_interactionPrompt;
};
