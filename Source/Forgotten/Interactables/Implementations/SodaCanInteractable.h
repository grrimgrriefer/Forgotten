// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "Forgotten/Interactables/Interactable.h"
#include "SodaCanInteractable.generated.h"

/**
 * Soda Can
 */
UCLASS()
class FORGOTTEN_API ASodaCanInteractable : public AInteractable
{
	GENERATED_BODY()

public:
	ASodaCanInteractable();
	virtual bool CanInteract(ACharacter* instigator) const override;
	void SetIsBeingInspected(const bool isBeingInspected);
	UStaticMesh* GetInspectMesh() const;

protected:
	virtual void OnPlayerInteract(AFirstPersonCharacter* player) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText m_itemName;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = "Interaction")
	bool m_isBeingInspected = false;
};
