// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "StateTreeInstanceData.h"
#include "Forgotten/Interactables/Implementations/ChairInteractable.h"
#include "Forgotten/Interactables/Implementations/GenericInspectable.h"
#include "StateTree/StateTreeContextBinder.h"
#include "FirstPersonCharacter.generated.h"

class UCharacterSubsystem;
class AConversableNPC;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UConversationWidget;
class UStateTree;

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
	virtual void EndPlay(const EEndPlayReason::Type endPlayReason) override;
	virtual void Tick(const float deltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* playerInputComponent) override;

	bool TryBindContextData(UObject* data);
	bool TryUnbindContextData(UObject* data);

	void EnterFocusedConvoMode();
	void ExitFocusedConvoMode();

	UCameraComponent* GetCameraComponent() const;
	float GetCameraInterpSpeed() const;
	void StartFocusedConversation(AConversableNPC* conversableNpc);
	void SitDown(AChairInteractable* chairInteractable);
	void Inspect3dInteractable(AGenericInspectable* genericInspectable);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StateTree", meta = (RequiredAssetDataTags = "Schema=UPlayerStateTreeSchema"))
	TObjectPtr<UStateTree> m_stateTreeAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> m_cameraComponent;
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UConversationWidget> m_chatWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> m_defaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> m_moveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> m_lookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> m_interactAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> m_triggerChatUiAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> m_focusChatAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> m_exitAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float m_interactionDistance = 250.0f;
	UPROPERTY(EditAnywhere, Category = "Interaction")
	float m_cameraInterpSpeed = 5.0f;

	UPROPERTY(Transient)
	TObjectPtr<UConversationWidget> m_chatWidget = nullptr;

private:
	bool IsPlayerInRangeForChat();

	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void AttemptInteraction();
	void TriggerChatUi();
	void FocusChat();
	void OnChatFocusLost();
	void ExitCurrentActivity();
	void UpdateInputState() const;

	UCharacterSubsystem* GetCharacterSubsystem(const bool allowNullptr = false) const;

	UPROPERTY()
	FStateTreeInstanceData m_stateTreeInstanceData;

	StateTreeContextBinder m_contextBinder;
	bool m_isStateTreeRunning = false;
};
