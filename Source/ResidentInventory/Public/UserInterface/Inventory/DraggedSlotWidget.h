// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/InventoryComponent.h"
#include "DraggedSlotWidget.generated.h"

class UInventoryPanel;
/**
 * 
 */
UCLASS()
class RESIDENTINVENTORY_API UDraggedSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(Transient, BlueprintReadOnly, Category = "DraggedSlot")
	FSlot InventorySlot;

	UFUNCTION(BlueprintCallable, Category = "DraggedSlot")
	void SetDraggedSlotData(const FSlot& InSlot, UInventoryPanel* InParentWidget);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DraggedSlot")
	void SetDraggedSlotSize(float NewSize);

protected:

	//=============================================================================
	// PROPERTIES
	//=============================================================================

	UPROPERTY(Transient, BlueprintReadOnly, Category = "DraggedSlot")
	UInventoryPanel* ParentWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DraggedSlot")
	FName RotateInputAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DraggedSlot")
	FSlateBrush ValidPlacementColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DraggedSlot")
	FSlateBrush InvalidPlacementColor;

	FOnInputAction RotateItemCallback;

	//=============================================================================
	// FUNCTIONS
	//=============================================================================

	UDraggedSlotWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DraggedSlot")
	void OnDraggedSlotDataReceived();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "DraggedSlot")
	void OnRotate();	
	
	UFUNCTION()
	void OnRotateItem();

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;	

};
