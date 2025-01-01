// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/InventoryComponent.h"
#include "SlotWidget.generated.h"

class UDraggedSlotWidget;
class UInventoryPanel;
/**
 * 
 */
UCLASS()
class RESIDENTINVENTORY_API USlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Slot")
	void SetSlotSize(const float NewSize);

	UFUNCTION(BlueprintCallable, Category = "Slot")
	void SetSlotData(const FSlot& InInventorySlot, UInventoryPanel* InParentWidget);

protected:

	//=============================================================================
	// PROPERTIES
	//=============================================================================

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Slot")
	UInventoryPanel* ParentWidget;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Slot")
	FSlot InventorySlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Slot")
	FSlateBrush DefaultColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Slot")
	FSlateBrush HoveredColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Slot")
	FSlateBrush ClickedColor;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Slot")
	FSlateBrush LastStateColor;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Slot")
	uint8 bMouseWasDragging : 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Slot")
	TSubclassOf<UDraggedSlotWidget> DraggedSlotWidgetClass;

	//=============================================================================
	// FUNCTIONS
	//=============================================================================
	
	USlotWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintImplementableEvent, Category = "Slot")
	void OnSlotDataReceived();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Slot")
	void SetSlotColor(const FSlateBrush& NewColor);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Slot")
	void OnDragStarted();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Slot")
	void OnDragCompleted(bool bCancelled);

	UFUNCTION(BlueprintImplementableEvent, Category = "Slot")
	void OnSlotRightClick();

	UFUNCTION(BlueprintImplementableEvent, Category = "Slot")
	void OnSlotLeftClick();
	
	void NativeOnSlotLeftClick();
	void NativeOnSlotRightClick();	
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
