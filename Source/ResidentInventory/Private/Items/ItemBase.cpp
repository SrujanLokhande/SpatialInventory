// Fill out your copyright notice in the Description page of Project Settings.


#include "ResidentInventory/Public/Items/ItemBase.h"

UItemBase::UItemBase()
{
	bIsRotated = false;
	TopLeftCoordinates = FPoint2D(0, 0);
	UpdateOccupiedCells();
}

UItemBase* UItemBase::CreateItemCopy() const
{
	// creating a new copy
	UItemBase* ItemCopy = NewObject<UItemBase>(StaticClass());
	ItemCopy->ID = this->ID;
	ItemCopy->ItemQuantity = this->ItemQuantity;
	ItemCopy->ItemRarity = this->ItemRarity;
	ItemCopy->ItemStatistics = this->ItemStatistics;
	ItemCopy->ItemType = this->ItemType;
	ItemCopy->NumericData = this->NumericData;
	ItemCopy->TextData = this->TextData;
	ItemCopy->AssetData = this->AssetData;

	// Copy grid-related properties
	ItemCopy->TopLeftCoordinates = this->TopLeftCoordinates;
	ItemCopy->bIsRotated = this->bIsRotated;
	ItemCopy->OccupiedCells = this->OccupiedCells;
	
	ItemCopy->bIsCopy = true;

	return ItemCopy;
}

void UItemBase::Rotate()
{
	// Only allow rotation if dimensions are different
	if (!CanBeRotated())
	{
		return;
	}

	bIsRotated = !bIsRotated;
    
	// Swap dimensions
	const float TempX = NumericData.ItemDimensions.X;
	NumericData.ItemDimensions.X = NumericData.ItemDimensions.Y;
	NumericData.ItemDimensions.Y = TempX;

	UpdateOccupiedCells();
	OnItemRotated.Broadcast();
}

void UItemBase::ResetRotation()
{
	if (bIsRotated)
	{
		Rotate();
	}
}

bool UItemBase::CanBeRotated() const
{
	// Item can be rotated if dimensions are different
	return NumericData.ItemDimensions.X != NumericData.ItemDimensions.Y;
}

TArray<FPoint2D> UItemBase::GetOccupiedCells() const
{
	return OccupiedCells;
}

void UItemBase::UpdateOccupiedCells()
{
	OccupiedCells.Empty();

	for (int32 X = 0; X < NumericData.ItemDimensions.X; X++)
	{
		for (int32 Y = 0; Y < NumericData.ItemDimensions.Y; Y++)
		{
			OccupiedCells.Add(FPoint2D(
				TopLeftCoordinates.X + X,
				TopLeftCoordinates.Y + Y
			));
		}
	}
}

void UItemBase::ResetItemFlags()
{
	bIsCopy = false;
	bIsPickup = false;
}

void UItemBase::SetItemQuantity(const int32 NewQuantity)
{
	if(NewQuantity != ItemQuantity)
	{
		ItemQuantity = FMath::Clamp(NewQuantity, 0, NumericData.bIsStackable ? NumericData.maxStackSize : 1);

		// if(OwningInventory)
		// {
		// 	if(ItemQuantity <= 0)
		// 	{
		// 		OwningInventory->RemoveSingleInstanceOfItem(this);
		// 	}
		// }
	}
}
