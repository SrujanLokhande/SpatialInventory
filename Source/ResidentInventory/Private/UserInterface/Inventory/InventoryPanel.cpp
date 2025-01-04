// Fill out your copyright notice in the Description page of Project Settings.

#include "UserInterface/Inventory/InventoryPanel.h"

#include "Components/InventoryComponent.h"
#include "UserInterface/Inventory/CellWidget.h"
#include "UserInterface/Inventory/SlotWidget.h"

UInventoryPanel::UInventoryPanel(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UInventoryPanel::NativeOnInventoryDataReceived()
{
	if (!Inventory->OnInventoryUpdated.IsAlreadyBound(this, &ThisClass::OnInventoryUpdated))
	{
		Inventory->OnInventoryUpdated.AddDynamic(this, &ThisClass::OnInventoryUpdated);
	}

	if (!Inventory->OnWeightChanged.IsAlreadyBound(this, &ThisClass::OnInventoryWeightChanged))
	{
		Inventory->OnWeightChanged.AddDynamic(this, &ThisClass::OnInventoryWeightChanged);
	}
	
	CellsWidgets.Empty();
	
	for (const FPoint2D& Cell: Inventory->Cells)
	{
		UCellWidget* CellWidget = CreateWidget<UCellWidget>(GetOwningPlayer(), CellWidgetClass);
		check(CellWidget != nullptr);

		CellWidget->SetCellData(Cell, Inventory->CellSize, this);
		
		CellsWidgets.Add(CellWidget);
		OnCellWidgetCreated(CellWidget);
	}

	SlotsWidgets.Empty();

	for (const FSlot& CurrentSlot: Inventory->Slots)
	{
		USlotWidget* SlotWidget = CreateWidget<USlotWidget>(GetOwningPlayer(), SlotWidgetClass);
		check(SlotWidget != nullptr);

		SlotWidget->SetSlotData(CurrentSlot, this);

		SlotsWidgets.Add(SlotWidget);
		OnSlotWidgetCreated(SlotWidget);
	}

	SetWeight(Inventory->CurrentWeight, Inventory->MaxWeight);
}

int32 UInventoryPanel::GetCellIndex(const FPoint2D& Coordinates)
{
	int32 Index = -1;
	for (const UCellWidget* Cell: CellsWidgets)
	{
		Index++;
		if (Cell->Coordinates == Coordinates)
		{
			return Index;
		}
	}

	return INDEX_NONE;
}

void UInventoryPanel::SetInventoryData(UInventoryComponent* NewInventory)
{
	OnPrePopulateData();
	
	Inventory = NewInventory;
	NativeOnInventoryDataReceived();
	OnInventoryDataReceived();
}

void UInventoryPanel::OnInventoryUpdated()
{
	// allow blueprints to remove existing slots from the grid before clearing slots array
	for (USlotWidget* SlotWidget: SlotsWidgets)
	{
		OnSlotWidgetRemoved(SlotWidget);
	}
	
	SlotsWidgets.Empty();

	for (const FSlot& CurrentSlot: Inventory->Slots)
	{
		USlotWidget* SlotWidget = CreateWidget<USlotWidget>(GetOwningPlayer(), SlotWidgetClass);
		check(SlotWidget != nullptr);

		SlotWidget->SetSlotData(CurrentSlot, this);

		SlotsWidgets.Add(SlotWidget);
		OnSlotWidgetCreated(SlotWidget);
	}

	UE_LOG(LogTemp, Warning, TEXT("OnInventoryUpdated()"));
}

void UInventoryPanel::OnInventoryWeightChanged()
{
	SetWeight(Inventory->CurrentWeight, Inventory->MaxWeight);
}
