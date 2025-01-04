// Fill out your copyright notice in the Description page of Project Settings.


#include "ResidentInventory/Public/Items/ItemBase.h"
#include "Items/ItemDataStruct.h"

UItemBase::UItemBase()
{	
}

void UItemBase::NativeOnConstruct()
{
	Size = Item->Size;
	bIsRotated = false;
	SizeInCells = Item->GetSizeInCells();
}

void UItemBase::Rotate()
{
	if (!Item->CanBeRotated())
	{
		return;
	}

	if (bIsRotated)
	{
		Size = Item->Size;
		bIsRotated = false;
		SizeInCells = Item->GetSizeInCells();

		NotifyItemRotated();
		return;
	}

	SizeInCells.Empty();
	Size = FPoint2D(Item->Size.Y, Item->Size.X);

	for (const FPoint2D& Point: Item->GetSizeInCells())
	{
		SizeInCells.Add(FPoint2D(Point.Y, Point.X));
	}

	bIsRotated = true;
	NotifyItemRotated();
}

bool UItemBase::IsRotated() const
{
	return bIsRotated;
}

void UItemBase::ResetRotation()
{
	Size = Item->Size;
	bIsRotated = false;
	SizeInCells = Item->GetSizeInCells();
}

void UItemBase::NotifyItemRotated()
{
	OnRotated();
	OnItemRotated.Broadcast();
}







