// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "Forgotten/Interactables/Interactable.h"
#include "GenericInspectable.generated.h"

/**
 * Generic 3d inspectable object
 */
UCLASS()
class FORGOTTEN_API AGenericInspectable : public AInteractable
{
	GENERATED_BODY()

public:
	AGenericInspectable();
	virtual bool CanInteract(ACharacter* instigator) const override;
	void SetIsBeingInspected(const bool isBeingInspected);
	UStaticMesh* GetInspectMesh() const;
	FVector GetInspectScale() const;

protected:
	virtual void OnPlayerInteract(AFirstPersonCharacter* player) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText m_itemName;
	UPROPERTY(Transient, VisibleInstanceOnly, Category = "Interaction")
	bool m_isBeingInspected = false;
};
