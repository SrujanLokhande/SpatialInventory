// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ResidentInventoryCharacter.generated.h"

class IInteractionInterface;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

USTRUCT()
struct FInteractionData
{
	GENERATED_BODY()

	FInteractionData() : CurrentInteractable(nullptr), LastInteractionCheckTime(0.0f) {}	

	// CurrentInteractable is what we already have 
	UPROPERTY()
	AActor* CurrentInteractable;

	// to shoot the line trace in intervals for checking the interactable actor
	UPROPERTY()
	float LastInteractionCheckTime;
};

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AResidentInventoryCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	//=============================================================================
	// PROPERTIES
	//=============================================================================


	//=============================================================================
	// FUNCTIONS
	//=============================================================================
	
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }	
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// Constructor
	AResidentInventoryCharacter();

protected:

	//=============================================================================
	// PROPERTIES
	//=============================================================================

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	// This TScriptInterface is Unreal Interface that lets us implement all functionalities for the UObject that implements an interface
	// TargetInteractable is the UObject that we are currently interacting with
	UPROPERTY(VisibleAnywhere, Category = "Character | Interaction")
	TScriptInterface<IInteractionInterface> TargetInteractable;

	//=============================================================================
	// FUNCTIONS
	//=============================================================================			
	void Move(const FInputActionValue& Value);	
	void Look(const FInputActionValue& Value);	
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;	
	
	virtual void BeginPlay();

};

