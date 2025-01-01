// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemDataStruct.h"
#include "Items/ItemBase.h"

UItemDataStruct::UItemDataStruct()
{
	//AssetType = UAssetManager_Custom::InventoryItem;
	ItemBaseClass = UItemBase::StaticClass();
	
	bCanBeRotated = false;
	bCanBeStacked = false;

	Weight = 0.0f;
	MaxStackSize = 2;

	Size = FPoint2D(1, 1);
}

FPrimaryAssetId UItemDataStruct::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(AssetType, GetFName());
}

FString UItemDataStruct::GetIdentifierString() const
{
	return GetPrimaryAssetId().ToString();
}

FString UItemDataStruct::GetAssetName() const
{
	return GetPrimaryAssetId().PrimaryAssetName.ToString();
}

TArray<FPoint2D> UItemDataStruct::GetSizeInCells() const
{
	TArray<FPoint2D> SizeInCells;
	
	for (int32 I = 0; I < Size.X; I++)
	{
		for (int32 J = 0; J < Size.Y; J++)
		{
			SizeInCells.Add(FPoint2D(I, J));			
		}
	}

	return SizeInCells;
}

bool UItemDataStruct::CanBeRotated() const
{
	return (Size.X != Size.Y || bCanBeRotated);
}

float UItemDataStruct::GetScaledWeight() const
{
	if (bCanBeStacked)
	{
		return ((Size.X * Size.Y) / MaxStackSize); 
	}

	return (Size.X * Size.Y); 
}
