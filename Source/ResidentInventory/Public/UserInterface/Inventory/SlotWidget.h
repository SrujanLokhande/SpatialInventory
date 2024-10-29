// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/InventoryComponent.h"
#include "SlotWidget.generated.h"

class UGridWidget;
class UDragDropOperation;
class UDraggedSlotWidget;

UCLASS()
class RESIDENTINVENTORY_API USlotWidget : public UUserWidget
{
	GENERATED_BODY()

	public:
    USlotWidget(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(EditDefaultsOnly, Category = "Slot|Appearance")
    FLinearColor DefaultColor;

    UPROPERTY(EditDefaultsOnly, Category = "Slot|Appearance")
    FLinearColor HoveredColor;

    UPROPERTY(EditDefaultsOnly, Category = "Slot|Appearance")
    FLinearColor ClickedColor;

    UPROPERTY(EditDefaultsOnly, Category = "Slot|Classes")
    TSubclassOf<UDraggedSlotWidget> DraggedSlotWidgetClass;

    UPROPERTY(BlueprintReadOnly, Category = "Slot")
    FSlot InventorySlot;

    UPROPERTY(BlueprintReadOnly, Category = "Slot")
    UGridWidget* ParentWidget;

    void SetSlotData(const FSlot& InInventorySlot, UGridWidget* InParentWidget);

protected:
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
    virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

    UFUNCTION(BlueprintImplementableEvent, Category = "Slot")
    void OnSlotDataReceived();

    UFUNCTION(BlueprintImplementableEvent, Category = "Slot")
    void SetSlotColor(const FLinearColor& Color);

    UFUNCTION(BlueprintImplementableEvent, Category = "Slot")
    void OnDragStarted();

    UFUNCTION(BlueprintImplementableEvent, Category = "Slot")
    void OnDragCancelled();

    UFUNCTION(BlueprintImplementableEvent, Category = "Slot")
    void OnSlotLeftClick();

    UFUNCTION(BlueprintImplementableEvent, Category = "Slot")
    void OnSlotRightClick();

    virtual void NativeOnSlotLeftClick();
    virtual void NativeOnSlotRightClick();

private:
    FLinearColor LastStateColor;
    bool bMouseWasDragging;
};
