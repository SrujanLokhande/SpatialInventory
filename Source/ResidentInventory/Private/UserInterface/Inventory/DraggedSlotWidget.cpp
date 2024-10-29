// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Inventory/DraggedSlotWidget.h"

#include "Items/ItemBase.h"

UDraggedSlotWidget::UDraggedSlotWidget(const FObjectInitializer& ObjectInitializer)
{
	RotateInputAction = TEXT("RotateItem");
}

void UDraggedSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind rotation input
	RotateItemCallback.BindUFunction(this, FName("OnRotateItem"));
	ListenForInputAction(RotateInputAction, IE_Pressed, true, RotateItemCallback);
}

void UDraggedSlotWidget::NativeDestruct()
{
	Super::NativeDestruct();
	RotateItemCallback.Unbind();
}

void UDraggedSlotWidget::SetDraggedSlotData(const FSlot& InSlot, UGridWidget* InParentWidget)
{
	InventorySlot = InSlot;
	ParentWidget = InParentWidget;
	OnDraggedSlotDataReceived();
}

void UDraggedSlotWidget::SetDraggedSlotSize(float Size)
{
	OnDraggedSlotDataReceived();
}

void UDraggedSlotWidget::OnRotateItem()
{
	if (InventorySlot.ItemInstance && InventorySlot.ItemInstance->CanBeRotated())
	{
		InventorySlot.ItemInstance->Rotate();
		OnRotate();
	}
}
