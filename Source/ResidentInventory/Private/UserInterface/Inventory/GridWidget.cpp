// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Inventory/GridWidget.h"

#include "Components/InventoryComponent.h"

UGridWidget::UGridWidget(const FObjectInitializer& ObjectInitializer)
{
}

void UGridWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGridWidget::SetInventoryComponent(UInventoryComponent* InInventory)
{
	if (!InInventory) return;

	Inventory = InInventory;
    
	// Clear existing widgets
	CellWidgets.Empty();
	SlotWidgets.Empty();

	// Bind to inventory events
	Inventory->OnInventoryUpdated.AddDynamic(this, &UGridWidget::OnInventoryUpdated);

	// Create cell widgets
	for (const FPoint2D& Cell : Inventory->Cells)
	{
		UCellWidget* CellWidget = CreateWidget<UCellWidget>(GetOwningPlayer(), CellWidgetClass);
		if (CellWidget)
		{
			CellWidget->SetCellData(Cell, Inventory->CellSize, this);
			CellWidgets.Add(CellWidget);
			OnCellWidgetCreated(CellWidget);
		}
	}

	// Create slot widgets for existing items
	for (const FSlot& Slot : Inventory->Slots)
	{
		USlotWidget* SlotWidget = CreateWidget<USlotWidget>(GetOwningPlayer(), SlotWidgetClass);
		if (SlotWidget)
		{
			SlotWidget->SetSlotData(Slot, this);
			SlotWidgets.Add(SlotWidget);
			OnSlotWidgetCreated(SlotWidget);
		}
	}

	OnInventorySet();
	UpdateSlotPositions();
}

void UGridWidget::OnInventoryUpdated()
{
	// Clear existing slot widgets
	SlotWidgets.Empty();

	// Recreate slot widgets for current inventory state
	for (const FSlot& Slot : Inventory->Slots)
	{
		USlotWidget* SlotWidget = CreateWidget<USlotWidget>(GetOwningPlayer(), SlotWidgetClass);
		if (SlotWidget)
		{
			SlotWidget->SetSlotData(Slot, this);
			SlotWidgets.Add(SlotWidget);
			OnSlotWidgetCreated(SlotWidget);
		}
	}

	UpdateSlotPositions();
}

int32 UGridWidget::GetCellIndex(const FPoint2D& Coordinates) const
{
	for (int32 i = 0; i < CellWidgets.Num(); ++i)
	{
		if (CellWidgets[i]->Coordinates == Coordinates)
		{
			return i;
		}
	}
	return INDEX_NONE;
}
