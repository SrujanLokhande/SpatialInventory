// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Inventory/SlotWidget.h"

#include "Blueprint/DragDropOperation.h"
#include "Items/ItemBase.h"

USlotWidget::USlotWidget(const FObjectInitializer& ObjectInitializer)
{
	bMouseWasDragging = false;
}

void USlotWidget::SetSlotData(const FSlot& InInventorySlot, UGridWidget* InParentWidget)
{
	InventorySlot = InInventorySlot;
	ParentWidget = InParentWidget;
	OnSlotDataReceived();
}

FReply USlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		FReply Reply = FReply::Handled();
		bMouseWasDragging = false;
		SetSlotColor(ClickedColor);
		NativeOnSlotLeftClick();
		OnSlotLeftClick();
        
		// Setup drag detection
		TSharedPtr<SWidget> SlotWidget = GetCachedWidget();
		if (SlotWidget.IsValid())
		{
			Reply.DetectDrag(SlotWidget.ToSharedRef(), EKeys::LeftMouseButton);
		}
		return Reply;
	}
	else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		bMouseWasDragging = false;
		SetSlotColor(ClickedColor);
		NativeOnSlotRightClick();
		OnSlotRightClick();
		return FReply::Handled();
	}

	return FReply::Handled();
}

FReply USlotWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bMouseWasDragging = false;
	SetSlotColor(LastStateColor);
	return FReply::Handled();
}

void USlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	SetSlotColor(HoveredColor);
	LastStateColor = HoveredColor;
}

void USlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	SetSlotColor(DefaultColor);
	LastStateColor = DefaultColor;
}

void USlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	bMouseWasDragging = true;
	OnDragStarted();

	// Create dragged slot widget
	UDraggedSlotWidget* DraggedSlot = CreateWidget<UDraggedSlotWidget>(GetOwningPlayer(), DraggedSlotWidgetClass);
	if (DraggedSlot)
	{
		DraggedSlot->SetDraggedSlotData(InventorySlot, ParentWidget);
        
		UDragDropOperation* DragDropOp = NewObject<UDragDropOperation>(GetTransientPackage());
		DragDropOp->DefaultDragVisual =  DraggedSlot;
		DragDropOp->Pivot = EDragPivot::TopLeft;
        
		OutOperation = DragDropOp;

		// Remove item from inventory while dragging
		ParentWidget->GetInventory()->RemoveItem(InventorySlot.ItemInstance);
	}
}

void USlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	SetSlotColor(DefaultColor);
	LastStateColor = DefaultColor;

	UDraggedSlotWidget* DraggedSlot = Cast<UDraggedSlotWidget>(InOperation->DefaultDragVisual);
	if (DraggedSlot)
	{
		// Return item to inventory at original position
		ParentWidget->GetInventory()->AddItem(DraggedSlot->GetItemReference(), InventorySlot.ItemInstance->TopLeftCoordinates);
	}

	OnDragCancelled();
}

bool USlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	UDraggedSlotWidget* DraggedSlot = Cast<UDraggedSlotWidget>(InOperation->DefaultDragVisual);
	if (!DraggedSlot) return false;

	// Handle stacking if items are the same type
	if (DraggedSlot->GetItemReference()->ID == InventorySlot.ItemInstance->ID &&
		InventorySlot.ItemInstance->NumericData.bIsStackable)
	{
		// Stack items
		ParentWidget->GetInventory()->StackItemStackOnSlot(
			FSlot(DraggedSlot->GetItemReference(), DraggedSlot->GetItemReference()->ItemQuantity, ParentWidget->GetInventory()),
			InventorySlot.ItemInstance->TopLeftCoordinates,
			DraggedSlot->GetItemReference()->ItemQuantity
		);
		return true;
	}

	return false;
}

void USlotWidget::NativeOnSlotLeftClick()
{
}

void USlotWidget::NativeOnSlotRightClick()
{
}
