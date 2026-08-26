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
#include "Forgotten/SubSystems/ConversationSubsystem.h"
#include "Forgotten/SubSystems/MainStateTreeSubsystem.h"
#include "Forgotten/Utils/AssertMacros.h"
#include "Forgotten/Widgets/ConversationWidget.h"
#include "StateTreeExecutionContext.h"
#include "Forgotten/StateTree/EventPayloads/FocusedConversationPayload.h"

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

	UEnhancedInputLocalPlayerSubsystem* inputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		playerController->GetLocalPlayer());
	ASSERT_CHECK(inputSubsystem);

	ASSERT_CHECK(m_defaultMappingContext);
	inputSubsystem->AddMappingContext(m_defaultMappingContext, 0);

	ASSERT_CHECK(m_chatWidgetClass);
	m_chatWidget = CreateWidget<UConversationWidget>(playerController, m_chatWidgetClass);
	ASSERT_CHECK(m_chatWidget);

	m_chatWidget->AddToViewport();
	m_chatWidget->m_OnChatFocusLost.AddUObject(this, &AFirstPersonCharacter::OnChatFocusLost);

	UConversationSubsystem* conversationSubSystem = GetConversationSubsystem();
	m_chatWidget->m_OnTextSubmitted.AddUObject(conversationSubSystem, &UConversationSubsystem::SubmitPlayerMessage);
	conversationSubSystem->m_OnTranscriptEntryAdded.AddUObject(m_chatWidget.Get(), &UConversationWidget::AddTranscriptEntry);

	TryBindContextData(this);

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
		if (UConversationSubsystem* conversationSubSystem = GetConversationSubsystem(true))
		{
			conversationSubSystem->m_OnTranscriptEntryAdded.RemoveAll(m_chatWidget.Get());
			m_chatWidget->m_OnTextSubmitted.RemoveAll(conversationSubSystem);
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
	ASSERT_CHECK(m_toggleChatAction);
	enhancedInputComponent->BindAction(m_toggleChatAction, ETriggerEvent::Started, this, &AFirstPersonCharacter::ToggleChat);
	ASSERT_CHECK(m_focusChatAction);
	enhancedInputComponent->BindAction(m_focusChatAction, ETriggerEvent::Started, this, &AFirstPersonCharacter::FocusChat);
	ASSERT_CHECK(m_exitAction);
	enhancedInputComponent->BindAction(m_exitAction, ETriggerEvent::Started, this, &AFirstPersonCharacter::ExitCurrentActivity);
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
		payload.ConversableNpc = conversableNpc;
		context.SendEvent(TAG_State_FocusedConversation_Start, FConstStructView::Make(payload));
	}
}
bool AFirstPersonCharacter::TryBindContextData(UObject* data)
{
	return m_contextBinder.TryBindContextData(data);
}
bool AFirstPersonCharacter::TryUnbindContextData(UObject* data)
{
	return m_contextBinder.TryUnbindContextData(data);
}
void AFirstPersonCharacter::EnterFocusedConvoMode()
{
	FocusChat();
}
void AFirstPersonCharacter::ExitFocusedConvoMode()
{
	m_chatWidget->UnfocusInput();
}
void AFirstPersonCharacter::Move(const FInputActionValue& value)
{
	const FVector2D movementVector = value.Get<FVector2D>();
	ASSERT_CHECK(Controller);

	const FRotator yawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
	const FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
	const FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(forwardDirection, movementVector.Y);
	AddMovementInput(rightDirection, movementVector.X);
}
void AFirstPersonCharacter::Look(const FInputActionValue& value)
{
	const FVector2D lookAxisVector = value.Get<FVector2D>();
	ASSERT_CHECK(Controller);

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
void AFirstPersonCharacter::ToggleChat()
{
	ASSERT_CHECK(m_chatWidget);
	m_chatWidget->ToggleTranscriptVisibility();
}
void AFirstPersonCharacter::FocusChat()
{
	if (m_chatWidget->IsInputFocused())
	{
		return;
	}

	ASSERT_CHECK(m_chatWidget);
	m_chatWidget->FocusInput();

	UpdateInputState();
}
void AFirstPersonCharacter::OnChatFocusLost()
{
	UpdateInputState();
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
void AFirstPersonCharacter::UpdateInputState() const
{
	APlayerController* playerController = Cast<APlayerController>(GetController());
	if (!playerController)
	{
		return;
	}

	playerController->ResetIgnoreMoveInput();
	playerController->ResetIgnoreLookInput();

	const bool isChatFocused = IsValid(m_chatWidget) && m_chatWidget->IsInputFocused();

	// TODO refactor this away, the Task should take care of its own input blocking
	const bool inFocusedConvo = GetConversationSubsystem()->HasCurrentConversableNpc();

	if (isChatFocused || inFocusedConvo)
	{
		playerController->SetIgnoreMoveInput(true);
	}
	if (isChatFocused)
	{
		playerController->SetIgnoreLookInput(true);
	}

	if (isChatFocused)
	{
		FInputModeGameAndUI inputMode;
		inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		playerController->SetInputMode(inputMode);
	}
	else
	{
		const FInputModeGameOnly gameMode;
		playerController->SetInputMode(gameMode);
	}
}
UConversationSubsystem* AFirstPersonCharacter::GetConversationSubsystem(const bool allowNullptr) const
{
	const UWorld* world = GetWorld();
	if (allowNullptr && !IsValid(world))
	{
		return nullptr;
	}
	ASSERT_CHECK_RETURN(world, nullptr);
	UConversationSubsystem* conversationSubSystem = world->GetSubsystem<UConversationSubsystem>();
	if (allowNullptr && !IsValid(conversationSubSystem))
	{
		return nullptr;
	}
	ASSERT_CHECK_RETURN(conversationSubSystem, nullptr);
	return conversationSubSystem;
}
