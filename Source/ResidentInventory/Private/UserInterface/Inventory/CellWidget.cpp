// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Inventory/CellWidget.h"

#include "Blueprint/DragDropOperation.h"
#include "Items/ItemBase.h"
#include "UserInterface/Inventory/DraggedSlotWidget.h"
#include "UserInterface/Inventory/GridWidget.h"

void UCellWidget::SetCellData(const FPoint2D& InCoordinates, float InSize, UGridWidget* InParentWidget)
{
	ParentWidget = InParentWidget;
	Coordinates = InCoordinates;
	CellSize = InSize;

	OnCellDataReceived();
}

FReply UCellWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bMouseWasDragging = false;
		SetCellColor(ClickedColor);
		return FReply::Handled();
	}
    
	bMouseWasDragging = false;
	return FReply::Handled();
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

	const UDraggedSlotWidget* DraggedSlotWidget = Cast<UDraggedSlotWidget>(InOperation->DefaultDragVisual);
	if (!DraggedSlotWidget || !ParentWidget) return;

	// Reset colors of all cells
	for (UCellWidget* CellWidget : ParentWidget->GetCellWidgets())
	{
		CellWidget->SetCellColor(CellWidget->DefaultColor);
	}

	// Check if item can be placed
	if (ParentWidget->GetInventory()->IsFreeCell(Coordinates))
	{
		TArray<FPoint2D> OccupiedCells = DraggedSlotWidget->GetItemReference()->GetOccupiedCells();
		if (ParentWidget->GetInventory()->DoesItemFit(OccupiedCells, Coordinates))
		{
			// Show valid placement
			for (const FPoint2D& Cell : OccupiedCells)
			{
				FPoint2D TargetCell = Cell + Coordinates;
				const int32 CellIndex = ParentWidget->GetCellIndex(TargetCell);
				if (CellIndex != INDEX_NONE)
				{
					ParentWidget->GetCellWidgets()[CellIndex]->SetCellColor(ValidPlacementColor);
				}
			}
		}
		else
		{
			// Show invalid placement
			for (const FPoint2D& Cell : OccupiedCells)
			{
				FPoint2D TargetCell = Cell + Coordinates;
				const int32 CellIndex = ParentWidget->GetCellIndex(TargetCell);
				if (CellIndex != INDEX_NONE)
				{
					ParentWidget->GetCellWidgets()[CellIndex]->SetCellColor(InvalidPlacementColor);
				}
			}
		}
	}
}

void UCellWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	if (!ParentWidget) return;

	// Reset all cell colors
	for (UCellWidget* CellWidget : ParentWidget->GetCellWidgets())
	{
		CellWidget->SetCellColor(CellWidget->DefaultColor);
	}
}

bool UCellWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	const UDraggedSlotWidget* DraggedSlotWidget = Cast<UDraggedSlotWidget>(InOperation->DefaultDragVisual);
	if (!DraggedSlotWidget || !ParentWidget) return false;

	// Reset cell colors
	for (UCellWidget* CellWidget : ParentWidget->GetCellWidgets())
	{
		CellWidget->SetCellColor(CellWidget->DefaultColor);
	}

	// Attempt to place item
	return ParentWidget->GetInventory()->MoveItem(DraggedSlotWidget->GetItemReference(), Coordinates);
}
