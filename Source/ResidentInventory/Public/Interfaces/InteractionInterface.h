// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

class AResidentInventoryCharacter;
// forcing each one of them to be just 1 byte
UENUM()
enum class EInteractableType : uint8
{
	Pickup UMETA(DisplayName = "Pickup"),
	NonPlayerCharacter UMETA(DisplayName = "NonPlayerCharacter"),
	Device UMETA(DisplayName = "Device"),
	Toggle UMETA(DisplayName = "Toggle"),
	Container UMETA(DisplayName = "Container")
};

USTRUCT()
struct FInteractableData
{
	GENERATED_BODY()

	// default constructor for the struct, this is called before the constructor is executed and sets the default values for all members
	// so whenever a new instance is overriding this, it will have all these default values
	FInteractableData() :
	InteractableType(EInteractableType::Pickup),
	Name(FText::GetEmpty()),
	Action(FText::GetEmpty()),
	Quantity(0),
	InteractionDuration(0.0f)
	{};

	EInteractableType InteractableType;

	UPROPERTY(EditInstanceOnly)
	FText Name;
	
	UPROPERTY(EditInstanceOnly)
	FText Action;

	// used only for pickups
	UPROPERTY(EditInstanceOnly)
	int8 Quantity;

	// used for having some duration after hiting the interact key
	UPROPERTY(EditInstanceOnly)
	float InteractionDuration;	
};


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RESIDENTINVENTORY_API IInteractionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// Begin Focus is when we look at an item and it glows
	virtual void BeginFocus();

	// ends the focus
	virtual void EndFocus();

	// begins the interaction of the item
	virtual void BeginInteract();

	// ends the interact
	virtual void EndInteract();

	// the actual interacting function
	virtual void Interact(AResidentInventoryCharacter* PlayerCharacter);

	// the interactions data
	FInteractableData InteractableData;
};
