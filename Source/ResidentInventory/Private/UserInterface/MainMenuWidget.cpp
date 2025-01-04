// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/MainMenuWidget.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/GridSlot.h"
#include "ResidentInventory/ResidentInventoryCharacter.h"
#include "UserInterface/Inventory/CellWidget.h"
#include "UserInterface/Inventory/DraggedSlotWidget.h"
#include "UserInterface/Inventory/InventoryPanel.h"
#include "UserInterface/Inventory/SlotWidget.h"

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
	const UDraggedSlotWidget* DraggedSlotWidget = Cast<UDraggedSlotWidget>(InOperation->DefaultDragVisual);
	
	for (UCellWidget* CellWidget: DraggedSlotWidget->ParentWidget->CellsWidgets)
	{
		CellWidget->SetCellColor(CellWidget->DefaultColor);
	}

	for (USlotWidget* SlotWidget: DraggedSlotWidget->ParentWidget->SlotsWidgets)
	{
		UGridSlot* GridSlot = UWidgetLayoutLibrary::SlotAsGridSlot(SlotWidget);
		if (GridSlot)
		{
			GridSlot->SetLayer(1);
		}
	}

	DraggedSlotWidget->ParentWidget->Inventory->DropItemOnSlot(DraggedSlotWidget->InventorySlot);

	return true;
}
