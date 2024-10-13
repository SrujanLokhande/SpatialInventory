// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/MainMenuWidget.h"
#include "ResidentInventory/ResidentInventoryCharacter.h"

void UMainMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerCharacter = Cast<AResidentInventoryCharacter>(GetOwningPlayerPawn());
}

bool UMainMenuWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	//const UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);

	// if(PlayerCharacter && ItemDragDrop->SourceItem)
	// {
	// 	PlayerCharacter->DropItem(ItemDragDrop->SourceItem, ItemDragDrop->SourceItem->ItemQuantity);
	// 	return true;
	// }
	// return false;
}
