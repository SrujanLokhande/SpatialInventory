// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/InventoryComponent.h"
#include "DraggedSlotWidget.generated.h"


class UGridWidget;
UCLASS()
class RESIDENTINVENTORY_API UDraggedSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UDraggedSlotWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, Category = "DraggedSlot|Input")
	FName RotateInputAction;
	
	UPROPERTY(BlueprintReadOnly, Category = "DraggedSlot")
	FSlot InventorySlot;

	UPROPERTY(BlueprintReadOnly, Category = "DraggedSlot")
	UGridWidget* ParentWidget;

	void SetDraggedSlotData(const FSlot& InSlot, UGridWidget* InParentWidget);
	void SetDraggedSlotSize(float Size);

	UFUNCTION(BlueprintPure, Category = "DraggedSlot")
	UItemBase* GetItemReference() const { return InventorySlot.ItemInstance; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DraggedSlot")
	FSlateBrush ValidPlacementColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DraggedSlot")
	FSlateBrush InvalidPlacementColor;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnRotateItem();

	UFUNCTION(BlueprintImplementableEvent, Category = "DraggedSlot")
	void OnDraggedSlotDataReceived();

	UFUNCTION(BlueprintImplementableEvent, Category = "DraggedSlot")
	void OnRotate();

private:
	FOnInputAction  RotateItemCallback;
};
