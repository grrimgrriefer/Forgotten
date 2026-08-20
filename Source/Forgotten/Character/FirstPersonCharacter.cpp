// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "FirstPersonCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Forgotten/CustomGameplayTags.h"
#include "Forgotten/Interactables/ConversableInterface.h"
#include "Forgotten/Interactables/InteractableInterface.h"
#include "Forgotten/StateTree/MainStateTreeSubsystem.h"
#include "Forgotten/Utils/AssertMacros.h"

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

	const APlayerController* playerController = Cast<APlayerController>(GetController());
	ASSERT_CHECK(playerController);

	UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		playerController->GetLocalPlayer());
	ASSERT_CHECK(subsystem);

	ASSERT_CHECK(m_defaultMappingContext, TEXT("AFirstPersonCharacter: m_defaultMappingContext is not assigned, "
										  "check the blueprint."));
	subsystem->AddMappingContext(m_defaultMappingContext, 0);

	ASSERT_CHECK(m_cameraComponent, TEXT("AFirstPersonCharacter: "
									  "m_cameraComponen has been removed? Check the blueprint."));
}
void AFirstPersonCharacter::Tick(const float deltaTime)
{
	Super::Tick(deltaTime);

	if (IsValid(m_conversationTarget))
	{
		ASSERT_CHECK(m_cameraComponent);
		const FVector cameraLoc = m_cameraComponent->GetComponentLocation();
		const FVector targetLoc = m_conversationTarget->GetActorLocation();
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
}
void AFirstPersonCharacter::EnterConversationMode(ACharacter* targetActor)
{
	ASSERT_CHECK(targetActor, TEXT("AFirstPersonCharacter: EnterConversationMode should always provide a valid target."));
	SetInternalConversationMode(targetActor);
}
void AFirstPersonCharacter::ExitConversationMode()
{
	SetInternalConversationMode(nullptr);
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
	const bool bHit = world->LineTraceSingleByChannel(
		hitResult,
		traceStart,
		traceEnd,
		ECC_Visibility,
		queryParams);

	if (bHit && hitResult.GetActor())
	{
		AActor* hitActor = hitResult.GetActor();
		if (hitActor->Implements<UConversableInterface>())
		{
			UMainStateTreeSubsystem* stateTreeSubsystem = world->GetGameInstance()->GetSubsystem<UMainStateTreeSubsystem>();

			ASSERT_CHECK(stateTreeSubsystem);

			stateTreeSubsystem->TryBindContextData(this);
			stateTreeSubsystem->TryBindContextData(hitActor);

			stateTreeSubsystem->TrySendFlowEvent(TAG_State_Conversation_Start);
		}
		else if (IInteractableInterface* interactable = Cast<IInteractableInterface>(hitActor))
		{
			interactable->Interact(this);
		}
	}
}
void AFirstPersonCharacter::SetInternalConversationMode(ACharacter* targetActor)
{
	m_conversationTarget = targetActor;
	const bool isInConversation = m_conversationTarget != nullptr;
	PrimaryActorTick.SetTickFunctionEnable(isInConversation);

	APlayerController* playerController = Cast<APlayerController>(GetController());
	if (isInConversation)
	{
		ASSERT_CHECK(playerController);
	}

	if (IsValid(playerController))
	{
		playerController->SetIgnoreMoveInput(isInConversation);
	}
}
