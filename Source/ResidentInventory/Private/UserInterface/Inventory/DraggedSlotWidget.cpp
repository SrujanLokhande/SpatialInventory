// Fill out your copyright notice in the Description page of Project Settings.

#include "UserInterface/Inventory/DraggedSlotWidget.h"

#include "Items/ItemBase.h"
#include "UserInterface/Inventory/CellWidget.h"
#include "UserInterface/Inventory/InventoryPanel.h"

UDraggedSlotWidget::UDraggedSlotWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	RotateInputAction = TEXT("RotateItem");
}

void UDraggedSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RotateItemCallback.BindDynamic(this, &ThisClass::OnRotateItem);
	ListenForInputAction(RotateInputAction, IE_Pressed, true, RotateItemCallback);
}

void UDraggedSlotWidget::NativeDestruct()
{
	Super::NativeDestruct();
	RotateItemCallback.Unbind();
}

void UDraggedSlotWidget::SetDraggedSlotData(const FSlot& InSlot, UInventoryPanel* InParentWidget)
{
	InventorySlot = InSlot;
	ParentWidget = InParentWidget;
	
	OnDraggedSlotDataReceived();
}

void UDraggedSlotWidget::OnRotateItem()
{
	for (UCellWidget* CellWidget: ParentWidget->CellsWidgets)
	{
		CellWidget->SetCellColor(CellWidget->DefaultColor);
	}
	
	InventorySlot.ItemBase->Rotate();
	OnDraggedSlotDataReceived();
	OnRotate();
}

