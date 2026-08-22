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

AFirstPersonCharacter::AFirstPersonCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

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

	ASSERT_CHECK(m_cameraComponent, TEXT("AFirstPersonCharacter: "
									  "m_cameraComponent has been removed? Check the blueprint."));

	APlayerController* playerController = Cast<APlayerController>(GetController());
	ASSERT_CHECK(playerController);

	UEnhancedInputLocalPlayerSubsystem* inputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		playerController->GetLocalPlayer());
	ASSERT_CHECK(inputSubsystem);

	ASSERT_CHECK(m_defaultMappingContext, TEXT("AFirstPersonCharacter: m_defaultMappingContext is not assigned, "
										  "check the blueprint."));
	inputSubsystem->AddMappingContext(m_defaultMappingContext, 0);


	ASSERT_CHECK(m_chatWidgetClass, TEXT("AFirstPersonCharacter: m_chatWidgetClass has not been assigned, "
									  "check the blueprint."));

	m_chatWidget = CreateWidget<UConversationWidget>(playerController, m_chatWidgetClass);
	ASSERT_CHECK(m_chatWidget);

	m_chatWidget->AddToViewport();
	m_chatWidget->m_OnChatFocusLost.AddUObject(this, &AFirstPersonCharacter::OnChatFocusLost);

	UWorld* world = GetWorld();
	ASSERT_CHECK(world);
	UConversationSubsystem* conversationSubSystem = world->GetSubsystem<UConversationSubsystem>();
	ASSERT_CHECK(conversationSubSystem);
	m_chatWidget->m_OnTextSubmitted.AddUObject(conversationSubSystem, &UConversationSubsystem::SubmitPlayerMessage);
	conversationSubSystem->m_OnTranscriptEntryAdded.AddUObject(m_chatWidget.Get(), &UConversationWidget::AddTranscriptEntry);

	UMainStateTreeSubsystem* stateTreeSubsystem = GetGameInstance()->GetSubsystem<UMainStateTreeSubsystem>();
	ASSERT_CHECK(stateTreeSubsystem);
	stateTreeSubsystem->TryBindContextData(this);
}
void AFirstPersonCharacter::Tick(const float deltaTime)
{
	Super::Tick(deltaTime);

	if (IsInFocusedConvo())
	{
		ASSERT_CHECK(m_cameraComponent);
		const FVector cameraLoc = m_cameraComponent->GetComponentLocation();
		const FVector targetLoc = m_focusedConversationNpc->GetActorLocation();
		const FRotator targetRotation = (targetLoc - cameraLoc).Rotation();

		APlayerController* playerController = Cast<APlayerController>(GetController());
		ASSERT_CHECK(playerController);

		const FRotator currentRotation = playerController->GetControlRotation();
		const FRotator newRotation = FMath::RInterpTo(
			currentRotation,
			targetRotation,
			deltaTime,
			m_cameraInterpSpeed);

		playerController->SetControlRotation(newRotation);
	}
}
void AFirstPersonCharacter::SetupPlayerInputComponent(UInputComponent* playerInputComponent)
{
	Super::SetupPlayerInputComponent(playerInputComponent);

	UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(playerInputComponent);
	ASSERT_CHECK(enhancedInputComponent);

	ASSERT_CHECK(m_moveAction, TEXT("AFirstPersonCharacter: m_moveAction is not assigned, check the blueprint."));
	enhancedInputComponent->BindAction(m_moveAction, ETriggerEvent::Triggered, this, &AFirstPersonCharacter::Move);
	ASSERT_CHECK(m_lookAction, TEXT("AFirstPersonCharacter: m_lookAction is not assigned, check the blueprint."));
	enhancedInputComponent->BindAction(m_lookAction, ETriggerEvent::Triggered, this, &AFirstPersonCharacter::Look);
	ASSERT_CHECK(m_interactAction, TEXT("AFirstPersonCharacter: m_interactAction is not assigned, check the blueprint."));
	enhancedInputComponent->BindAction(m_interactAction, ETriggerEvent::Started, this, &AFirstPersonCharacter::AttemptInteraction);
	ASSERT_CHECK(m_toggleChatAction, TEXT("AFirstPersonCharacter: m_toggleChatAction is not assigned, check the blueprint."));
	enhancedInputComponent->BindAction(m_toggleChatAction, ETriggerEvent::Started, this, &AFirstPersonCharacter::ToggleChat);
	ASSERT_CHECK(m_focusChatAction, TEXT("AFirstPersonCharacter: m_focusChatAction is not assigned, check the blueprint."));
	enhancedInputComponent->BindAction(m_focusChatAction, ETriggerEvent::Started, this, &AFirstPersonCharacter::FocusChat);
	ASSERT_CHECK(m_exitAction, TEXT("AFirstPersonCharacter: m_exitAction is not assigned, check the blueprint."));
	enhancedInputComponent->BindAction(m_exitAction, ETriggerEvent::Started, this, &AFirstPersonCharacter::ExitCurrentActivity);
}
void AFirstPersonCharacter::EnterFocusedConvoMode(AConversableNPC* conversableNpc)
{
	ASSERT_CHECK(conversableNpc);
	m_focusedConversationNpc = conversableNpc;
	PrimaryActorTick.SetTickFunctionEnable(true);

	FocusChat();
}
void AFirstPersonCharacter::ExitFocusedConvoMode()
{
	m_focusedConversationNpc = nullptr;
	PrimaryActorTick.SetTickFunctionEnable(false);

	m_chatWidget->UnfocusInput();
}
bool AFirstPersonCharacter::IsInFocusedConvo() const
{
	return IsValid(m_focusedConversationNpc);
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
		if (AConversableNPC* conversableNpc = Cast<AConversableNPC>(hitResult.GetActor()))
		{
			UMainStateTreeSubsystem* stateTreeSubsystem = GetGameInstance()->GetSubsystem<UMainStateTreeSubsystem>();
			ASSERT_CHECK(stateTreeSubsystem);
			stateTreeSubsystem->TryBindContextData(conversableNpc);
			stateTreeSubsystem->TrySendFlowEvent(TAG_State_FocusedConversation_Start);
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
	if (IsInFocusedConvo())
	{
		UMainStateTreeSubsystem* stateTreeSubsystem = GetGameInstance()->GetSubsystem<UMainStateTreeSubsystem>();
		ASSERT_CHECK(stateTreeSubsystem);
		stateTreeSubsystem->TrySendFlowEvent(TAG_State_FocusedConversation_End);
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

	const bool bChatFocused = IsValid(m_chatWidget) && m_chatWidget->IsInputFocused();
	const bool bInFocusedConvo = IsInFocusedConvo();

	if (bChatFocused || bInFocusedConvo)
	{
		playerController->SetIgnoreMoveInput(true);
	}
	if (bChatFocused)
	{
		playerController->SetIgnoreLookInput(true);
	}

	if (bChatFocused)
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
