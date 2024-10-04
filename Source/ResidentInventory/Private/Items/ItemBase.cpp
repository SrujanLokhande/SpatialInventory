// Fill out your copyright notice in the Description page of Project Settings.


#include "ResidentInventory/Public/Items/ItemBase.h"

UItemBase::UItemBase()
{
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
	ItemCopy->bIsCopy = true;

	return ItemCopy;
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
