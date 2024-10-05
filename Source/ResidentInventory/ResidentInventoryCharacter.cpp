// Copyright Epic Games, Inc. All Rights Reserved.

#include "ResidentInventoryCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Interfaces/InteractionInterface.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AResidentInventoryCharacter

AResidentInventoryCharacter::AResidentInventoryCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// for line trace
	InteractionCheckFrequency = 0.1;
	InteractionCheckDistance = 225.f;
	BaseEyeHeight = 74.0f;
}


void AResidentInventoryCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
		{
		
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);		
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AResidentInventoryCharacter::Move);		
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AResidentInventoryCharacter::Look);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ThisClass::BeginInteract);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &ThisClass::EndInteract);
		// Open Menu and inventory
		EnhancedInputComponent->BindAction(ToggleMenuAction, ETriggerEvent::Triggered, this, &ThisClass::ToggleMenu);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AResidentInventoryCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AResidentInventoryCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AResidentInventoryCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}
void AResidentInventoryCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime > InteractionCheckFrequency))
	{
		PerformInteractionCheck();
	}
}

void AResidentInventoryCharacter::PerformInteractionCheck()
{
	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	// to initialize a variable, same as TraceStart = FVector::Zero
	//FVector TraceStart{FVector::Zero()};

	FVector TraceStart{GetPawnViewLocation()};
	FVector TraceEnd{TraceStart + (GetViewRotation().Vector() * InteractionCheckDistance)};

	// float LookDirection{FVector::DotProduct(GetActorForwardVector(), GetViewRotation().Vector())};
	// the above wont work because the dot product return a double but our variable is float
	// and using the brackets forces to use the returned type only
	float LookDirection = FVector::DotProduct(GetActorForwardVector(), GetViewRotation().Vector());

	// as in dot products +ve means they are looking in same direction and -ve means opposite direction
	if(LookDirection > 0)
	{
		// Debug Linetrace Line
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 2.0f);

		// for line trace collisions
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		// store the result of a line tarce
		FHitResult TraceHit;

		// using if because LineTrace returns true or false based on something we hit
		if(GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
		{
			// if the HitActor implements the Interface class
			// using the StaticClass return the C++ class
			if(TraceHit.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
			{			
				// if we are looking and interacting with the same interactable
				// this will be false if we are looking different and interacting with a different Actor
				if(TraceHit.GetActor() != InteractionData.CurrentInteractable)
				{
					FoundInteractable(TraceHit.GetActor());
					return;
				}

				if(TraceHit.GetActor() == InteractionData.CurrentInteractable) return;
			}
		}		
	}
	// if none of the checks are true, that means we haven't found any interactable
	NoInteractableFound();
}

void AResidentInventoryCharacter::FoundInteractable(AActor* NewInteractable)
{
	// to make sure that the previous interaction is not meddling with the new interactable
	if(IsInteracting())
	{
		EndInteract();
	}

	// there is a current Interactable
	if(InteractionData.CurrentInteractable)
	{
		TargetInteractable = InteractionData.CurrentInteractable;
		TargetInteractable->EndFocus();
	}

	InteractionData.CurrentInteractable = NewInteractable;
	TargetInteractable = NewInteractable;

	// Creates and Updates the HUD from the HUD class
	//HUDRef->UpdateInteractionWidget(&TargetInteractable->InteractableData);

	// makes the new interactable as the focus
	TargetInteractable->BeginFocus();
}

void AResidentInventoryCharacter::NoInteractableFound()
{
	// if we did not find any interactable clearing the timer manager
	if(IsInteracting())
	{
		GetWorldTimerManager().ClearTimer(TimerHandleInteraction);
	}

	if(InteractionData.CurrentInteractable)
	{
		if(IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->EndFocus();
		}
		
		// hide interaction widget on HUD
		//HUDRef->HideInteractionWidget();

		InteractionData.CurrentInteractable = nullptr;
		TargetInteractable = nullptr;
	}	
}

void AResidentInventoryCharacter::BeginInteract()
{
	// to double-check if we are interacting with the same thing since beginning interaction
	PerformInteractionCheck();

	// timed interaction, like a progress bar to interact with an object
	if(IsValid(InteractionData.CurrentInteractable))
	{
		if(IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->BeginInteract();

			// to set the interaction duration, check the APickup class because that's where all the values of this Interaction Interface struct 
			// are being set by taking the values from the data Table
			if(FMath::IsNearlyZero(TargetInteractable->InteractableData.InteractionDuration, 0.1f))
			{
				Interact();
			}
			else
			{
				GetWorldTimerManager().SetTimer(TimerHandleInteraction,
					this,
					&AResidentInventoryCharacter::Interact,
					TargetInteractable->InteractableData.InteractionDuration,
					false);
			}
		}
	}
}

void AResidentInventoryCharacter::EndInteract()
{
	GetWorldTimerManager().ClearTimer(TimerHandleInteraction);

	if(IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->EndInteract();
	}
}

void AResidentInventoryCharacter::Interact()
{
	GetWorldTimerManager().ClearTimer(TimerHandleInteraction);
	if(IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->Interact(this);
	}
}

void AResidentInventoryCharacter::ToggleMenu()
{
	UE_LOG(LogTemp, Warning, TEXT("Toogle Menu Called"));
}


