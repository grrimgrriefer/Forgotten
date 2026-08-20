// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "FirstPersonCharacter.generated.h"

class UCameraComponent;
class UInputMappingContext;
class UInputAction;

/**
 * Main player character
 */
UCLASS()
class FORGOTTEN_API AFirstPersonCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFirstPersonCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(const float deltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* playerInputComponent) override;

	void EnterConversationMode(ACharacter* targetActor);
	void ExitConversationMode();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> m_cameraComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> m_defaultMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> m_moveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> m_lookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> m_interactAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float m_interactionDistance = 250.0f;
	UPROPERTY(EditAnywhere, Category = "Interaction")
	float m_cameraInterpSpeed = 5.0f;
	UPROPERTY(Transient)
	TObjectPtr<ACharacter> m_conversationTarget = nullptr;

private:
	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void AttemptInteraction();
	void SetInternalConversationMode(ACharacter* targetActor);
};
