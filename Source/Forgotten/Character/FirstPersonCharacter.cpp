// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "FirstPersonCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Forgotten/Interactables/InteractableInterface.h"

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

	const APlayerController* playerController = Cast<APlayerController>(GetController());
	check(playerController);

	UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		playerController->GetLocalPlayer());
	check(subsystem);

	ensureMsgf(m_defaultMappingContext, TEXT("AFirstPersonCharacter: m_defaultMappingContext is not assigned, "
										  "check the blueprint."));
	subsystem->AddMappingContext(m_defaultMappingContext, 0);

	ensureMsgf(m_cameraComponent, TEXT("AFirstPersonCharacter: m_cameraComponen has been removed? Check the blueprint."));
}
void AFirstPersonCharacter::Tick(const float deltaTime)
{
	Super::Tick(deltaTime);

	if (IsValid(m_conversationTarget))
	{
		check(m_cameraComponent);
		const FVector cameraLoc = m_cameraComponent->GetComponentLocation();
		const FVector targetLoc = m_conversationTarget->GetActorLocation();
		const FRotator targetRotation = (targetLoc - cameraLoc).Rotation();
		APlayerController* playerController = Cast<APlayerController>(GetController());

		check(playerController);
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
	check(enhancedInputComponent);

	ensureMsgf(m_moveAction, TEXT("AFirstPersonCharacter: m_moveAction is not assigned, check the blueprint."));
	enhancedInputComponent->BindAction(m_moveAction, ETriggerEvent::Triggered, this, &AFirstPersonCharacter::Move);
	ensureMsgf(m_lookAction, TEXT("AFirstPersonCharacter: m_lookAction is not assigned, check the blueprint."));
	enhancedInputComponent->BindAction(m_lookAction, ETriggerEvent::Triggered, this, &AFirstPersonCharacter::Look);
	ensureMsgf(m_interactAction, TEXT("AFirstPersonCharacter: m_interactAction is not assigned, check the blueprint."));
	enhancedInputComponent->BindAction(m_interactAction, ETriggerEvent::Started, this, &AFirstPersonCharacter::AttemptInteraction);
}
void AFirstPersonCharacter::EnterConversationMode(AActor* targetActor)
{
	ensureMsgf(targetActor, TEXT("AFirstPersonCharacter: EnterConversationMode should always provide a valid target."));
	SetInternalConversationMode(targetActor);
}
void AFirstPersonCharacter::ExitConversationMode()
{
	SetInternalConversationMode(nullptr);
}
void AFirstPersonCharacter::Move(const FInputActionValue& value)
{
	const FVector2D movementVector = value.Get<FVector2D>();
	check(Controller);

	const FRotator yawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
	const FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
	const FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(forwardDirection, movementVector.Y);
	AddMovementInput(rightDirection, movementVector.X);
}
void AFirstPersonCharacter::Look(const FInputActionValue& value)
{
	const FVector2D lookAxisVector = value.Get<FVector2D>();
	check(Controller);

	AddControllerYawInput(lookAxisVector.X);
	AddControllerPitchInput(lookAxisVector.Y);
}
void AFirstPersonCharacter::AttemptInteraction()
{
	check(m_cameraComponent);
	const FVector traceStart = m_cameraComponent->GetComponentLocation();
	const FVector traceEnd = traceStart + (m_cameraComponent->GetForwardVector() * m_interactionDistance);

	FHitResult hitResult;
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(this);

	const UWorld* world = GetWorld();
	check(world);
	const bool bHit = world->LineTraceSingleByChannel(
		hitResult,
		traceStart,
		traceEnd,
		ECC_Visibility,
		queryParams);

	if (bHit && hitResult.GetActor())
	{
		if (IInteractableInterface* interactable = Cast<IInteractableInterface>(hitResult.GetActor()))
		{
			APlayerController* playerController = Cast<APlayerController>(GetController());
			check(playerController);
			interactable->Interact(playerController);
		}
	}
}
void AFirstPersonCharacter::SetInternalConversationMode(AActor* targetActor)
{
	m_conversationTarget = targetActor;
	const bool isInConversation = m_conversationTarget != nullptr;
	PrimaryActorTick.SetTickFunctionEnable(isInConversation);

	APlayerController* playerController = Cast<APlayerController>(GetController());
	if (isInConversation)
	{
		check(playerController);
	}

	if (IsValid(playerController))
	{
		playerController->SetIgnoreMoveInput(isInConversation);
	}
}
