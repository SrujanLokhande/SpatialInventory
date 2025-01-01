// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/InventoryComponent.h"
#include "ResidentInventory/DataStructure/Point2D.h"
#include "UObject/Object.h"
#include "ItemBase.generated.h"

class UItemDataStruct;
class UInventoryComponent;
/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FItemEvent);
UCLASS(Abstract, BlueprintType, Blueprintable)
class RESIDENTINVENTORY_API UItemBase : public UObject
{
	GENERATED_BODY()

public:
	//==========================================================
	// PROPERTIES
	//==========================================================

	// Owner of the inventory
	UPROPERTY()
	UInventoryComponent* OwningInventory;	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ItemInstance")
	UItemDataStruct* Item;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ItemInstance")
	FPoint2D TopLeftCoordinates;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ItemInstance")
	TArray<FPoint2D> SizeInCells;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ItemInstance")
	FPoint2D Size;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ItemInstance")
	uint8 bIsRotated : 1;

	UPROPERTY(BlueprintAssignable)
	FItemEvent OnItemRotated;

	//==========================================================
	// FUNCTIONS
	//==========================================================
	
	// Constructor
	UItemBase();
	
	void NativeOnConstruct();

	UFUNCTION(BlueprintImplementableEvent, Category = "ItemInstance")
	void OnConstruct();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "ItemInstance")
	void OnRotated();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "ItemInstance")
	void OnUsed();

	UFUNCTION(BlueprintCallable, Category = "ItemInstance")
	void Rotate();

	UFUNCTION(BlueprintPure, Category = "ItemInstance")
	bool IsRotated() const;

	UFUNCTION(BlueprintCallable, Category = "ItemInstance")
	void ResetRotation();	

	void NotifyItemRotated();
};


