// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ResidentInventory/DataStructure/ItemDataStruct.h"
#include "ItemBase.generated.h"

/**
 * 
 */
UCLASS()
class RESIDENTINVENTORY_API UItemBase : public UObject
{
	GENERATED_BODY()

public:
	//==========================================================
	// PROPERTIES
	//==========================================================

	UPROPERTY(VisibleAnywhere, Category = "ItemData")
	int32 ItemQuantity;

	// ALl the properties from the Data Struct
	UPROPERTY(VisibleAnywhere, Category = "Item")
	FName ID;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	EItemType ItemType;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	EItemRarity ItemRarity;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FITemStats ItemStatistics;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemTextData TextData;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemNumericData NumericData;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemAssetData AssetData;

	bool bIsCopy;
	bool bIsPickup;	

	//==========================================================
	// FUNCTIONS
	//==========================================================
	
	void ResetItemFlags();

	// Constructor
	UItemBase();

	// creating a copy of the item
	UItemBase* CreateItemCopy() const;

	// making it forceinline creates a copy of this function wherever it is called to reduce function calling times and optimizing
	// Returns the weight of the whole stack of items
	UFUNCTION(Category = "Item")
	FORCEINLINE float GetItemStackWeight() const { return ItemQuantity * NumericData.ItemWeight ; }

	// returns the weight of individual items
	UFUNCTION(Category = "Item")
	FORCEINLINE float GetItemSingleWeight() const { return NumericData.ItemWeight ; }

	// returns if the item stack is full
	UFUNCTION(Category = "Item")
	FORCEINLINE bool IsItemFullStack() const { return ItemQuantity == NumericData.maxStackSize ; }

	// Setter for the item quantity
	UFUNCTION(Category = "Item")
	void SetItemQuantity(const int32 NewQuantity);

protected:

	// overloading the == operator to compare the keys inside TArray for array of the items
	bool operator==(const FName& OtherID) const
	{
		return this->ID == OtherID;
	}	
};
