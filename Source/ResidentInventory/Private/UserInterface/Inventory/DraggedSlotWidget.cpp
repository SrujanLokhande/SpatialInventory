// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Inventory/DraggedSlotWidget.h"

#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "Items/ItemBase.h"
#include "UserInterface/Inventory/CellWidget.h"
#include "UserInterface/Inventory/GridWidget.h"

UDraggedSlotWidget::UDraggedSlotWidget(const FObjectInitializer& ObjectInitializer)
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
		for (UCellWidget* CellWidget: ParentWidget->CellsWidgets)
		{
			CellWidget->SetCellColor(CellWidget->DefaultColor);
		}
		InventorySlot.ItemInstance->Rotate();
		OnRotate();
	}
}
