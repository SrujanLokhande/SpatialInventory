// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Inventory/CellWidget.h"

#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/GridSlot.h"
#include "Items/ItemBase.h"
#include "UserInterface/Inventory/DraggedSlotWidget.h"
#include "UserInterface/Inventory/InventoryPanel.h"
#include "UserInterface/Inventory/SlotWidget.h"

UCellWidget::UCellWidget(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

void UCellWidget::SetCellData(const FPoint2D& InCoordinates, float InSize, UInventoryPanel* InParentWidget)
{
	ParentWidget = InParentWidget;
	Coordinates = InCoordinates;
	CellSize = InSize;

	OnCellDataReceived();
}

void UCellWidget::OnItemRotated()
{
	const UDraggedSlotWidget* DraggedSlotWidget = Cast<UDraggedSlotWidget>(CachedDragDropOperation->DefaultDragVisual);
	if (ParentWidget->Inventory->DoesItemFit(DraggedSlotWidget->InventorySlot.ItemBase->SizeInCells, Coordinates))
	{
		for (const FPoint2D& Cell: DraggedSlotWidget->InventorySlot.ItemBase->SizeInCells)
		{
			FPoint2D TargetCell = Cell + Coordinates;
			const int32 CellIndex = ParentWidget->GetCellIndex(TargetCell);

			if (CellIndex > INDEX_NONE)
			{
				ParentWidget->CellsWidgets[CellIndex]->SetCellColor(ValidPlacementColor);
			}
		}
	}
	else
	{
		for (const FPoint2D& Cell: DraggedSlotWidget->InventorySlot.ItemBase->SizeInCells)
		{
			FPoint2D TargetCell = Cell + Coordinates;
			const int32 CellIndex = ParentWidget->GetCellIndex(TargetCell);

			if (CellIndex > INDEX_NONE)
			{
				ParentWidget->CellsWidgets[CellIndex]->SetCellColor(InvalidPlacementColor);
			}
		}
	}
}

FReply UCellWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bMouseWasDragging = false;
		SetCellColor(ClickedColor);
		return FReply::Handled();
	}
	else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		bMouseWasDragging = false;
		SetCellColor(ClickedColor);
		return FReply::Handled();
	}
	else
	{
		bMouseWasDragging = false;
		return FReply::Handled();
	}
}

FReply UCellWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bMouseWasDragging = false;
	SetCellColor(LastStateColor);
	return FReply::Handled();
}

void UCellWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);	

	SetCellColor(HoveredColor);
	LastStateColor = HoveredColor;
}

void UCellWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	SetCellColor(DefaultColor);
	LastStateColor = DefaultColor;
}

void UCellWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);

	CachedDragDropOperation = InOperation;

	const UDraggedSlotWidget* DraggedSlotWidget = Cast<UDraggedSlotWidget>(InOperation->DefaultDragVisual);
	DraggedSlotWidget->InventorySlot.ItemBase->OnItemRotated.AddDynamic(this, &ThisClass::OnItemRotated);

	for (UCellWidget* CellWidget: ParentWidget->CellsWidgets)
	{
		CellWidget->SetCellColor(CellWidget->DefaultColor);
	}

	if (ParentWidget->Inventory->IsFreeCell(Coordinates))
	{
		
	}

	if (ParentWidget->Inventory->DoesItemFit(DraggedSlotWidget->InventorySlot.ItemBase->SizeInCells, Coordinates))
	{
		for (const FPoint2D& Cell: DraggedSlotWidget->InventorySlot.ItemBase->SizeInCells)
		{
			FPoint2D TargetCell = Cell + Coordinates;
			const int32 CellIndex = ParentWidget->GetCellIndex(TargetCell);

			if (CellIndex > INDEX_NONE)
			{
				ParentWidget->CellsWidgets[CellIndex]->SetCellColor(ValidPlacementColor);
			}
		}
	}
	else
	{
		for (const FPoint2D& Cell: DraggedSlotWidget->InventorySlot.ItemBase->SizeInCells)
		{
			FPoint2D TargetCell = Cell + Coordinates;
			const int32 CellIndex = ParentWidget->GetCellIndex(TargetCell);

			if (CellIndex > INDEX_NONE)
			{
				ParentWidget->CellsWidgets[CellIndex]->SetCellColor(InvalidPlacementColor);
			}
		}
	}
}

void UCellWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	const UDraggedSlotWidget* DraggedSlotWidget = Cast<UDraggedSlotWidget>(InOperation->DefaultDragVisual);

	for (UCellWidget* CellWidget: ParentWidget->CellsWidgets)
	{
		CellWidget->SetCellColor(CellWidget->DefaultColor);
	}

	DraggedSlotWidget->InventorySlot.ItemBase->OnItemRotated.RemoveAll(this);
}

bool UCellWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	const UDraggedSlotWidget* DraggedSlotWidget = Cast<UDraggedSlotWidget>(InOperation->DefaultDragVisual);

	for (UCellWidget* CellWidget: ParentWidget->CellsWidgets)
	{
		CellWidget->SetCellColor(CellWidget->DefaultColor);
	}

	for (USlotWidget* SlotWidget: ParentWidget->SlotsWidgets)
	{
		UGridSlot* GridSlot = UWidgetLayoutLibrary::SlotAsGridSlot(SlotWidget);
		if (GridSlot)
		{
			GridSlot->SetLayer(1);
		}
	}

	DraggedSlotWidget->InventorySlot.ItemBase->OnItemRotated.RemoveAll(this);
	
	ParentWidget->Inventory->MoveItemOnSlot(DraggedSlotWidget->InventorySlot, Coordinates);
	
	return true;
}

void UCellWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	const UDraggedSlotWidget* DraggedSlotWidget = Cast<UDraggedSlotWidget>(InOperation->DefaultDragVisual);

	for (UCellWidget* CellWidget: ParentWidget->CellsWidgets)
	{
		CellWidget->SetCellColor(CellWidget->DefaultColor);
	}
	
	DraggedSlotWidget->InventorySlot.ItemBase->OnItemRotated.RemoveAll(this);
	ParentWidget->Inventory->Slots.Add(DraggedSlotWidget->InventorySlot);
}
