// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "FirstPersonCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Forgotten/CustomGameplayTags.h"
#include "Forgotten/Character/ConversableNPC.h"
#include "Forgotten/SubSystems/MainStateTreeSubsystem.h"
#include "Forgotten/Utils/AssertMacros.h"
#include "Forgotten/Widgets/ConversationWidget.h"
#include "StateTreeExecutionContext.h"
#include "Forgotten/StateTree/Tasks/Player/FocusedConversationTask.h"
#include "Forgotten/StateTree/Tasks/Player/Inspect3dTask.h"
#include "Forgotten/StateTree/Tasks/Player/SeatedTask.h"
#include "SubSystems/CharacterSubsystem.h"

AFirstPersonCharacter::AFirstPersonCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	m_cameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	m_cameraComponent->SetupAttachment(GetCapsuleComponent());
	m_cameraComponent->bUsePawnControlRotation = true;
}
void AFirstPersonCharacter::BeginPlay()
{
	Super::BeginPlay();

	ASSERT_CHECK(m_cameraComponent);

	APlayerController* playerController = Cast<APlayerController>(GetController());
	ASSERT_CHECK(playerController);

	UEnhancedInputLocalPlayerSubsystem* inputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer());
	ASSERT_CHECK(inputSubsystem);
	const FInputModeGameOnly inputMode;
	playerController->SetInputMode(inputMode);
	playerController->SetShowMouseCursor(false);

	ASSERT_CHECK(m_defaultMappingContext);
	inputSubsystem->AddMappingContext(m_defaultMappingContext, 0);

	ASSERT_CHECK(m_chatWidgetClass);
	m_chatWidget = CreateWidget<UConversationWidget>(playerController, m_chatWidgetClass);
	ASSERT_CHECK(m_chatWidget);

	m_chatWidget->AddToViewport();
	m_chatWidget->m_OnChatFocusLost.AddUObject(this, &AFirstPersonCharacter::OnChatFocusLost);

	UCharacterSubsystem* characterSubsystem = GetCharacterSubsystem();
	m_chatWidget->m_OnTextSubmitted.AddUObject(characterSubsystem, &UCharacterSubsystem::SubmitMessageFromPlayer);
	characterSubsystem->m_OnTranscriptEntryAdded.AddUObject(m_chatWidget.Get(), &UConversationWidget::AddTranscriptEntry);

	TryBindContextData(this);
	TryBindContextData(characterSubsystem);

	ASSERT_CHECK(m_stateTreeAsset);
	FStateTreeExecutionContext context(*this, *m_stateTreeAsset, m_stateTreeInstanceData);
	if (m_contextBinder.SetContextRequirements(context, m_stateTreeAsset, this))
	{
		if (context.Start() == EStateTreeRunStatus::Running)
		{
			m_isStateTreeRunning = true;
		}
	}
}
void AFirstPersonCharacter::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	if (m_isStateTreeRunning)
	{
		FStateTreeExecutionContext context(*this, *m_stateTreeAsset, m_stateTreeInstanceData);
		if (m_contextBinder.SetContextRequirements(context, m_stateTreeAsset, this))
		{
			context.Stop();
		}
		m_isStateTreeRunning = false;
		TryUnbindContextData(this);
	}

	if (m_chatWidget)
	{
		if (UCharacterSubsystem* characterSubsystem = GetCharacterSubsystem(true))
		{
			characterSubsystem->m_OnTranscriptEntryAdded.RemoveAll(m_chatWidget.Get());
			m_chatWidget->m_OnTextSubmitted.RemoveAll(characterSubsystem);
		}

		m_chatWidget->RemoveFromParent();
		m_chatWidget = nullptr;
	}

	Super::EndPlay(endPlayReason);
}
void AFirstPersonCharacter::Tick(const float deltaTime)
{
	Super::Tick(deltaTime);

	if (m_isStateTreeRunning)
	{
		FStateTreeExecutionContext context(*this, *m_stateTreeAsset, m_stateTreeInstanceData);
		if (m_contextBinder.SetContextRequirements(context, m_stateTreeAsset, this))
		{
			context.Tick(deltaTime);
		}
	}
}
void AFirstPersonCharacter::SetupPlayerInputComponent(UInputComponent* playerInputComponent)
{
	Super::SetupPlayerInputComponent(playerInputComponent);

	UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(playerInputComponent);
	ASSERT_CHECK(enhancedInputComponent);

	ASSERT_CHECK(m_moveAction);
	enhancedInputComponent->BindAction(m_moveAction, ETriggerEvent::Triggered, this, &AFirstPersonCharacter::Move);
	ASSERT_CHECK(m_lookAction);
	enhancedInputComponent->BindAction(m_lookAction, ETriggerEvent::Triggered, this, &AFirstPersonCharacter::Look);
	ASSERT_CHECK(m_interactAction);
	enhancedInputComponent->BindAction(m_interactAction, ETriggerEvent::Started, this, &AFirstPersonCharacter::AttemptInteraction);
	ASSERT_CHECK(m_triggerChatUiAction);
	enhancedInputComponent->BindAction(m_triggerChatUiAction, ETriggerEvent::Started, this, &AFirstPersonCharacter::TriggerChatUi);
	ASSERT_CHECK(m_focusChatAction);
	enhancedInputComponent->BindAction(m_focusChatAction, ETriggerEvent::Started, this, &AFirstPersonCharacter::FocusChatInput);
	ASSERT_CHECK(m_exitAction);
	enhancedInputComponent->BindAction(m_exitAction, ETriggerEvent::Started, this, &AFirstPersonCharacter::ExitCurrentActivity);
}
UCameraComponent* AFirstPersonCharacter::GetCameraComponent() const
{
	return m_cameraComponent;
}
void AFirstPersonCharacter::StartFocusedConversation(AConversableNPC* conversableNpc)
{
	if (!m_isStateTreeRunning || !IsValid(conversableNpc))
	{
		return;
	}

	FStateTreeExecutionContext context(*this, *m_stateTreeAsset, m_stateTreeInstanceData);
	if (m_contextBinder.SetContextRequirements(context, m_stateTreeAsset, this))
	{
		FFocusedConversationPayload payload;
		payload.m_ConversableNpc = conversableNpc;
		context.SendEvent(TAG_State_Start_FocusedConversation, FConstStructView::Make(payload));
	}
}
void AFirstPersonCharacter::SitDown(AChairInteractable* chairInteractable)
{
	if (!m_isStateTreeRunning || !IsValid(chairInteractable))
	{
		return;
	}

	FStateTreeExecutionContext context(*this, *m_stateTreeAsset, m_stateTreeInstanceData);
	if (m_contextBinder.SetContextRequirements(context, m_stateTreeAsset, this))
	{
		FSeatedPayload payload;
		payload.m_Chair = chairInteractable;
		context.SendEvent(TAG_State_Start_Seated, FConstStructView::Make(payload));
	}
}
void AFirstPersonCharacter::Inspect3dInteractable(AGenericInspectable* genericInspectable)
{
	if (!m_isStateTreeRunning || !IsValid(genericInspectable))
	{
		return;
	}

	FStateTreeExecutionContext context(*this, *m_stateTreeAsset, m_stateTreeInstanceData);
	if (m_contextBinder.SetContextRequirements(context, m_stateTreeAsset, this))
	{
		FInspect3dPayload payload;
		payload.m_Inspectable = genericInspectable;
		payload.m_PreviewOffset = FVector(45.0f, 0.0f, -8.0f);
		context.SendEvent(TAG_State_Start_Inspect3d, FConstStructView::Make(payload));
	}
}
bool AFirstPersonCharacter::IsPlayerInRangeForChat()
{
	const UCharacterSubsystem* characterSubsystem = GetCharacterSubsystem();
	const AConversableNPC* npc = Cast<AConversableNPC>(characterSubsystem->GetCurrentConversationNpc());

	if (!IsValid(npc))
	{
		npc = Cast<AConversableNPC>(characterSubsystem->TryGetNearestNPC(this));
	}

	if (IsValid(npc))
	{
		return npc->IsInRangeForChat(this);
	}

	return false;
}
bool AFirstPersonCharacter::TryBindContextData(UObject* data)
{
	return m_contextBinder.TryBindContextData(data);
}
bool AFirstPersonCharacter::TryUnbindContextData(UObject* data)
{
	return m_contextBinder.TryUnbindContextData(data);
}
void AFirstPersonCharacter::FocusChatInput()
{
	ASSERT_CHECK(m_chatWidget);
	if (m_chatWidget->IsInputFocused())
	{
		return;
	}

	const bool inRange = IsPlayerInRangeForChat();

	if (!inRange)
	{
		m_chatWidget->SetOutOfRangeFeedbackVisibility(true);
		return;
	}

	m_chatWidget->FocusInput();

	if (APlayerController* playerController = Cast<APlayerController>(GetController()))
	{
		FInputModeUIOnly inputMode;
		inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		playerController->SetInputMode(inputMode);
		playerController->SetShowMouseCursor(true);
	}
}
void AFirstPersonCharacter::UnfocusChatInput()
{
	if (!m_chatWidget || !m_chatWidget->IsInputFocused())
	{
		return;
	}

	m_chatWidget->UnfocusInput();

	if (APlayerController* playerController = Cast<APlayerController>(GetController()))
	{
		const FInputModeGameOnly inputMode;
		playerController->SetInputMode(inputMode);
		playerController->SetShowMouseCursor(false);
	}
}
void AFirstPersonCharacter::Move(const FInputActionValue& value)
{
	if (!Controller || Controller->IsMoveInputIgnored())
	{
		return;
	}

	const FVector2D movementVector = value.Get<FVector2D>();
	const FRotator yawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
	const FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
	const FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(forwardDirection, movementVector.Y);
	AddMovementInput(rightDirection, movementVector.X);
}
void AFirstPersonCharacter::Look(const FInputActionValue& value)
{
	if (!Controller || Controller->IsLookInputIgnored())
	{
		return;
	}

	const FVector2D lookAxisVector = value.Get<FVector2D>();
	AddControllerYawInput(lookAxisVector.X);
	AddControllerPitchInput(lookAxisVector.Y);
}
void AFirstPersonCharacter::AttemptInteraction()
{
	ASSERT_CHECK(m_cameraComponent);
	const FVector traceStart = m_cameraComponent->GetComponentLocation();
	const FVector traceEnd = traceStart + (m_cameraComponent->GetForwardVector() * m_interactionDistance);

	FHitResult hitResult;
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(this);

	const UWorld* world = GetWorld();
	ASSERT_CHECK(world);
	if (world->LineTraceSingleByChannel(hitResult, traceStart, traceEnd, ECC_Visibility, queryParams))
	{
		if (AActor* hitActor = hitResult.GetActor())
		{
			if (IInteractableInterface* interactable = Cast<IInteractableInterface>(hitActor))
			{
				if (interactable->CanInteract(this))
				{
					interactable->Interact(this);
				}
			}
		}
	}
}
void AFirstPersonCharacter::TriggerChatUi()
{
	if (!m_chatWidget)
	{
		return;
	}

	if (m_chatWidget->IsInputFocused())
	{
		UnfocusChatInput();
	}
	else if (IsPlayerInRangeForChat())
	{
		FocusChatInput();
	}
	else
	{
		m_chatWidget->SetOutOfRangeFeedbackVisibility(true);
	}
}
void AFirstPersonCharacter::OnChatFocusLost()
{
	if (APlayerController* playerController = Cast<APlayerController>(GetController()))
	{
		const FInputModeGameOnly inputMode;
		playerController->SetInputMode(inputMode);
		playerController->SetShowMouseCursor(false);
	}
}
void AFirstPersonCharacter::ExitCurrentActivity()
{
	if (m_chatWidget->IsInputFocused())
	{
		m_chatWidget->UnfocusInput();
		return;
	}

	if (m_isStateTreeRunning)
	{
		FStateTreeExecutionContext context(*this, *m_stateTreeAsset, m_stateTreeInstanceData);
		if (m_contextBinder.SetContextRequirements(context, m_stateTreeAsset, this))
		{
			context.SendEvent(TAG_State_Quit_CurrentActivity);
		}
	}
}
UCharacterSubsystem* AFirstPersonCharacter::GetCharacterSubsystem(const bool allowNullptr) const
{
	const UWorld* world = GetWorld();
	if (allowNullptr && !IsValid(world))
	{
		return nullptr;
	}
	ASSERT_CHECK_RETURN(world, nullptr);
	UCharacterSubsystem* characterSubsystem = world->GetSubsystem<UCharacterSubsystem>();
	if (allowNullptr && !IsValid(characterSubsystem))
	{
		return nullptr;
	}
	ASSERT_CHECK_RETURN(characterSubsystem, nullptr);
	return characterSubsystem;
}
