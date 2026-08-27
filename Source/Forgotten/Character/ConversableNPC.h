// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Forgotten/Interactables/InteractableInterface.h"
#include "ConversableNPC.generated.h"

class UStaticMeshComponent;

/**
 * Base class for all conversable characters (Rain, Zannah, etc)
 */
UCLASS()
class FORGOTTEN_API AConversableNPC : public ACharacter, public IInteractableInterface
{
	GENERATED_BODY()

public:
	AConversableNPC();

#pragma region IInteractableInterface
	virtual bool CanInteract(ACharacter* instigator) const override;
	virtual void Interact(ACharacter* instigator) override;
	virtual FText GetInteractionUiMessage(ACharacter* instigator) const override;
	virtual USceneComponent* GetInteractionPoint() const override;
#pragma endregion IInteractableInterface

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> m_placeholderMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterInfo")
	FText m_characterName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterInfo")
	FText m_interactionUiMessage;
};
